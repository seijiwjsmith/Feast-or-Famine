// internal
#include "physics.hpp"
#include "entt.hpp"
#include "debug.hpp"
#include <iostream>
#include <projectile.hpp>
#include "rig.hpp"
#include <monsters/spider.hpp>
#include <particle.hpp>
#include <units/priestess.hpp>
#include "rope.hpp"
#include "world.hpp"
#include <monsters/dragon_rig.hpp>

const size_t POTENTIAL_COLLISION_RANGE = 150;

// Returns the local bounding coordinates scaled by the current size of the entity 
vec2 get_bounding_box(const Motion& motion)
{
	// fabs is to avoid negative scale due to the facing direction.
	
	return { abs(motion.boundingbox.x), abs(motion.boundingbox.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You don't
// need to try to use this technique.
bool collides(const Motion& motion1, const Motion& motion2)
{
	
	auto dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	float other_r = std::sqrt(std::pow(get_bounding_box(motion1).x/2.0f, 2.f) + std::pow(get_bounding_box(motion1).y/2.0f, 2.f));
	float my_r = std::sqrt(std::pow(get_bounding_box(motion2).x/2.0f, 2.f) + std::pow(get_bounding_box(motion2).y/2.0f, 2.f));
	float r = max(other_r, my_r);
	if (dist_squared < r * r)
		return true;
	return false;
}

std::vector<vec2> get_box_vertices(entt::entity entity)
{
	auto& motion = registry.get<Motion>(entity);
	std::vector<vec2> points;

	if (registry.has<LaserBeam>(entity)) {
		float x_direction = cos(motion.angle) * 125;
		float y_direction = sin(motion.angle) * 125;

		points.push_back(motion.position + vec2(x_direction, y_direction));
		points.push_back(motion.position - vec2(x_direction, y_direction));
	}
	else if (registry.has<IceField>(entity)) {
		for (float i = 0; i < 2 * PI; i += PI / 4)
		{
			points.push_back(motion.position + 148.f * vec2(cos(i), sin(i)));
		}
	}
	else {
		points.push_back(motion.position + motion.boundingbox / 2.f);
		points.push_back(motion.position + vec2(motion.boundingbox.x, -motion.boundingbox.y) / 2.f);
		points.push_back(motion.position - motion.boundingbox / 2.f);
		points.push_back(motion.position + vec2(-motion.boundingbox.x, motion.boundingbox.y) / 2.f);
	}
	return points;
}

std::vector<vec2> get_norms(std::vector<vec2> vertices)
{
	std::vector<vec2> norms;
	for (int i = 0; i < vertices.size(); i++) {
		vec2 current_point = vertices[i];
		vec2 next_point = vertices[(i + 1) % vertices.size()];
		vec2 edge = next_point - current_point;

		vec2 norm;
		if (edge.x >= 0) {
			if (edge.y >= 0) norm = { edge.y, -edge.x };
			else norm = { -edge.y, edge.x };
		}
		else {
			if (edge.y >= 0) norm = { -edge.y, edge.x };
			else norm = { edge.y, -edge.x };
		}
		norms.push_back(norm);
	}
	return norms;
}

vec2 projectShape(std::vector<vec2> vertices, vec2 axis)
{
	float min = INFINITY;
	float max = -INFINITY;
	for (vec2 v : vertices) {
		float projection = dot(v, axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
	return vec2(min, max);
}

bool checkProjection(std::vector<vec2> poly_a_vertices, std::vector<vec2> poly_b_vertices,
	std::vector<vec2> poly_a_norms, std::vector<vec2> poly_b_norms)
{
	for (vec2 norm : poly_a_norms) {
		vec2 projection1 = projectShape(poly_a_vertices, norm);
		vec2 projection2 = projectShape(poly_b_vertices, norm);
		if ((projection1.y < projection2.x) || (projection2.y < projection1.x))
			return false;
	}

	for (vec2 norm : poly_b_norms) {
		vec2 projection1 = projectShape(poly_a_vertices, norm);
		vec2 projection2 = projectShape(poly_b_vertices, norm);
		if ((projection1.y < projection2.x) || (projection2.y < projection1.x))
			return false;
	}

	return true;
}
 
bool collidesSAT(entt::entity entity1, entt::entity entity2)
{
	std::vector<vec2> polygon_a = get_box_vertices(entity1);
	std::vector<vec2> polygon_b = get_box_vertices(entity2);
	
	std::vector<vec2> poly_a_norms = get_norms(polygon_a);
	std::vector<vec2> poly_b_norms = get_norms(polygon_b);

	return checkProjection(polygon_a, polygon_b, poly_a_norms, poly_b_norms);
}

// Precise Collisions with two convex objects 
bool preciseCollides(entt::entity monster, entt::entity projectile)
{
	auto& monster_motion = registry.get<Motion>(monster);

	std::vector<vec2> projectile_vertices = get_box_vertices(projectile);
	std::vector<vec2> projectile_norms = get_norms(projectile_vertices);

	auto& rig = registry.get<Rig>(monster);
	Transform transform;
	transform.mat = mat3(1.0f);
	transform.translate(monster_motion.position);
	transform.rotate(monster_motion.angle);

	for (auto chain : rig.chains) {

		for (auto part : chain.chain_vector) {
			auto mesh_ref = registry.get<ShadedMeshRef>(part).reference_to_cache->mesh.vertices;

			const auto& entity_transform = registry.get<Transform>(part);
			Transform temp_transform;
			temp_transform.mat = transform.mat * entity_transform.mat;

			//rig vertices
			std::vector<vec2> rig_vertices;
			for (int i = 0; i < mesh_ref.size(); i++) {
				auto& v = mesh_ref[i];
				vec3 g = temp_transform.mat * vec3(v.position.x, v.position.y, 1.0f);
				rig_vertices.push_back(vec2(g.x, g.y));
			}

			std::vector<vec2> rig_norms = get_norms(rig_vertices);

			if (checkProjection(rig_vertices, projectile_vertices, rig_norms, projectile_norms)) {
				return true;
			}
		}
	}

	return false;
}


void PhysicsSystem::step(float elapsed_ms)
{
	// Move entities based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.

	float step_seconds = 1.0f * (elapsed_ms / 1000.f);

	update_projectiles(elapsed_ms);

	for(auto entity: registry.view<Motion>()) {
	    auto& motion = registry.get<Motion>(entity);
		if (registry.has<Monster>(entity)) {
		    auto& monster = registry.get<Monster>(entity);
		    if (monster.state != ATTACK) {
                motion.position += step_seconds * motion.velocity * monster.speed_multiplier;
		    }
		}
		else {
			motion.position += step_seconds * motion.velocity;
			if (registry.has<Priestess>(entity)) {
			    if (length(coord_to_pixel(pixel_to_coord(motion.position)) - motion.position) >= (float)GRID_CELL_SIZE / 20) {
			        motion.velocity *= -1;
			    }
			}
		}
	}

	// Check for collisions between all moving entities

	auto view_motion = registry.view<Motion>();

	for (unsigned int i = 0; i < view_motion.size(); i++)
	{
		for (unsigned int j = i + 1; j < view_motion.size(); j++)
		{
			Motion& motion_i = registry.get<Motion>(view_motion[i]);
			entt::entity entity_i = view_motion[i];
			Motion& motion_j = registry.get<Motion>(view_motion[j]);
			entt::entity entity_j = view_motion[j];

			// If either entity is already dying, do not consider collisions
			if (registry.has<EntityDeath>(entity_i) || registry.has<EntityDeath>(entity_j)) continue;

			// considers collisions between only if the entities are projectiles and monsters
			if ((registry.has<Projectile>(entity_i) && registry.has<Monster>(entity_j)) || (registry.has<Projectile>(entity_j) && registry.has<Monster>(entity_i)))
			{
				// considers collisions if entities are within a certain range
				if (length(motion_i.position - motion_j.position) < POTENTIAL_COLLISION_RANGE || registry.has<DragonRig>(entity_i) || registry.has<DragonRig>(entity_j)) {
				
					// convex polygon precise collision
					if (registry.has<Rig>(entity_i) || registry.has<Rig>(entity_j)) {
						// notify Observers - ORDER MATTERS
						if (registry.has<Projectile>(entity_i)) {
							// precise collision between complex rig and convex object - ORDER MATTERS
							if (preciseCollides(entity_j, entity_i)) {
								notifyObservers(entity_i, entity_j);
							}
						}
						else {
							if (preciseCollides(entity_i, entity_j)) {
								notifyObservers(entity_j, entity_i);
							}
						}
					}
					
					// box-to-box polygon collision
					else {
						// checks if collides using Separating Axis Theorem
						if (collidesSAT(entity_i, entity_j))
						{
							// notify Observers - ORDER MATTERS!
							if (registry.has<Projectile>(entity_i)) {
								notifyObservers(entity_i, entity_j);
							}
							else {
								notifyObservers(entity_j, entity_i);
							}
						}
					}
				}
			}
		}
	}

	//rope rig constraint updates
	auto view_ropeRigs = registry.view<RopeRig>();
	for (auto entity : view_ropeRigs) {
		RopeSystem::update_physics(entity, elapsed_ms);
		RopeSystem::update_rig(entity);
	
	}

	if (DebugSystem::in_debug_mode)
	{
		for (auto [entity, motion] : view_motion.each())
		{
			if (registry.has<Rig>(entity)) {
				DebugSystem::display_rig_vertices(entity);
			}
			if (!registry.has<GridNode>(entity) 
				&& !registry.has<HealthComponent>(entity) 
				&& !registry.has<DebugComponent>(entity)
				) {//&& !registry.has<RigPart>(entity)

				DebugSystem::createBox(motion.position, motion.boundingbox);
			}
		}
	}
	
}

void PhysicsSystem::update_projectiles(float elapsed_ms)
{
	float step_seconds = 1.0f * (elapsed_ms / 1000.f);

	for (auto entity : registry.view<Snowball>()) {
		auto& motion = registry.get<Motion>(entity);
		motion.angle += 0.2f;
		auto& rock = registry.get<Snowball>(entity);
		if (rock.current_step >= rock.bezier_points.size() - 1 || rock.bezier_points.empty()) {
			continue;
		}
		motion.velocity = 1 / step_seconds * rock.bezier_points[rock.current_step];
		rock.current_step += 1;
	}

	for (auto entity : registry.view<Flamethrower>()) {
		auto& flamethrower = registry.get<Flamethrower>(entity);
		flamethrower.active_timer -= elapsed_ms;
		auto& motion_p = registry.get<Motion>(entity);
		auto& motion_u = registry.get<Motion>(flamethrower.e_unit);
		float x_direction = cos(motion_u.angle) * 60;
		float y_direction = sin(motion_u.angle) * 60;

		motion_p.angle = motion_u.angle + PI;
		motion_p.position = motion_u.position + vec2(x_direction, y_direction);

		if (flamethrower.active_timer < 0)
			registry.destroy(entity);
	}

	for (auto entity : registry.view<LaserBeam>()) {
		auto& laserBeam = registry.get<LaserBeam>(entity);
		laserBeam.active_timer -= elapsed_ms;
		if (registry.valid(laserBeam.e_unit)) {
			auto& motion_p = registry.get<Motion>(entity);
			auto& motion_m = registry.get<Motion>(laserBeam.e_unit);
			vec2 direction = normalize(motion_m.position - laserBeam.unit_pos);

			motion_p.angle = atan2(direction.y, direction.x);
			motion_p.position = laserBeam.unit_pos + direction * abs(motion_p.scale.x) / 2.f;
		}

		if (laserBeam.active_timer < 0)
			registry.destroy(entity);
	}

	for (auto entity : registry.view<Explosion>()) {
		auto& laserBeam = registry.get<Explosion>(entity);
		laserBeam.active_timer -= elapsed_ms;
		
		if (laserBeam.active_timer < 0)
			registry.destroy(entity);
	}

	for (auto entity : registry.view<IceField>()) {
		auto& icefield = registry.get<IceField>(entity);
		icefield.active_timer -= elapsed_ms;

		if (icefield.active_timer < 0)
			registry.destroy(entity);
	}
}

void PhysicsSystem::title_screen_step(float elapsed_ms)
{
	float step_seconds = 1.0f * (elapsed_ms / 1000.f);
	for (auto thisParticle : registry.view<ParticleSystem>()) {
		auto& p1_motion = registry.get<Motion>(thisParticle);
		
		vec2 align_vector = {0,0};
		int align_total = 0;

		vec2 separation_vector = { 0,0 };
		int separation_total = 0;

		vec2 cohesion_vector = { 0,0 };
		int cohesion_total = 0;

		for (auto otherParticle : registry.view<ParticleSystem>()) {
			auto p2_motion = registry.get<Motion>(otherParticle);
			float distance = length(p1_motion.position - p2_motion.position);

			if (thisParticle != otherParticle && distance < 40) {
				align_vector += p2_motion.velocity;
				align_total += 1;
			}

			if (thisParticle != otherParticle && distance < 30) {
				separation_vector += (p2_motion.position - p1_motion.position) / (distance * distance);
				separation_total += 1;
			}

			if (thisParticle != otherParticle && distance < 120) {
				cohesion_vector += p2_motion.position;
				cohesion_total += 1;
			}
		}
		// This will generate a number from 0.0 to 1.0, inclusive.
		float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		
		float limit = 200.f;
		if (align_total > 0) {
			align_vector /= align_total;
			align_vector *= (limit / length(align_vector));
			//align_vector -= vec2(p1_motion.velocity.x * r1, p1_motion.velocity.y * r2);
			align_vector -= p1_motion.velocity;
			align_vector = 4.f * normalize(align_vector);
		}

		if (separation_total > 0) {
			separation_vector /= separation_total;
			separation_vector *= (limit / length(separation_vector));
			separation_vector -= p1_motion.velocity;
			separation_vector = 20.f * normalize(separation_vector);
		}

		if (cohesion_total > 0) {
			cohesion_vector /= cohesion_total;
			cohesion_vector *= (limit / length(cohesion_vector));
			cohesion_vector -= p1_motion.velocity;
			cohesion_vector = 0.4f * normalize(cohesion_vector);
		}

		p1_motion.acceleration += align_vector;
		p1_motion.acceleration += separation_vector;
		p1_motion.acceleration += cohesion_vector;
		p1_motion.acceleration *= rand() % 10 + 1;

		p1_motion.position += step_seconds * p1_motion.velocity;
		p1_motion.velocity += step_seconds * p1_motion.acceleration;
		//p1_motion.velocity *= length(p1_motion.velocity) > 50? (50 / length(p1_motion.velocity)) : 1;
		p1_motion.acceleration *= 0;
	}
}

PhysicsSystem::Collision::Collision(entt::entity& other)
{
	this->other = other;
}

void PhysicsSystem::notifyObservers(entt::entity entity_i, entt::entity entity_j) {
	for (int i = 0; i < observerlist.size(); i++) {	
		observerlist[i]->updateProjectileMonsterCollision(entity_i, entity_j);
	}
}

void PhysicsSystem::attach(Observer* obs) {
	this->observerlist.push_back(obs);
}