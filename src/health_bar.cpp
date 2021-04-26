// Header

#include "entt.hpp"
#include "render.hpp"
#include "common.hpp"
#include <cmath>
#include <iostream>

#include "render_components.hpp"
#include "health_bar.hpp"
#include "world.hpp"

namespace HealthSystem
{

	//NOTE: currently, health is not percentage based.
	void createHealthBar(vec2 position,  int health, int max_health, vec3 color) {
		auto entity = registry.create();

		std::string key = "health bar";
		ShadedMesh& resource = cache_resource(key);
		if (resource.effect.program.resource == 0) {
			// create a procedural circle
			constexpr float z = -0.1f;

			// Corner points
			ColoredVertex v;
			v.position = { -0.5,-0.5,z };
			v.color = color;
			resource.mesh.vertices.push_back(v);
			v.position = { -0.5,0.5,z };
			v.color = color;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,0.5,z };
			v.color = color;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,-0.5,z };
			v.color = color;
			resource.mesh.vertices.push_back(v);

			// Two triangles
			resource.mesh.vertex_indices.push_back(0);
			resource.mesh.vertex_indices.push_back(1);
			resource.mesh.vertex_indices.push_back(3);
			resource.mesh.vertex_indices.push_back(1);
			resource.mesh.vertex_indices.push_back(2);
			resource.mesh.vertex_indices.push_back(3);

			RenderSystem::createColoredMesh(resource, "colored_mesh");
		}

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		ShadedMeshRef& shaded_mesh =  registry.emplace<ShadedMeshRef>(entity, resource);
		shaded_mesh.layer = LAYER_UI;
		// Create motion
		auto& motion = registry.emplace<Motion>(entity);
		motion.angle = 0.f;
		motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
		motion.position = position + vec2(0, 50);
		motion.scale = vec2((float)health/ (float)max_health * 50.0f, 5);

		registry.emplace<HealthComponent>(entity);


	}

	void updateHealthComponents(float elapsed_ms) {
		auto view_health_bars = registry.view<HealthComponent>();
		for (auto [entity] : view_health_bars.each()) {
			registry.destroy(entity);
		}
		auto view_hit_reactions = registry.view<HitReaction, Motion, Monster>();
		for (auto [entity, hit, motion, monster] : view_hit_reactions.each()) {
			hit.counter_ms -= elapsed_ms;
			if (hit.counter_ms > 0) {
				createHealthBar(motion.position, monster.health,monster.max_health );
			}
		}
        auto view_unit_hit_reactions = registry.view<HitReaction, Motion, Unit>();
        for (auto [entity, hit, motion, unit] : view_unit_hit_reactions.each()) {
            hit.counter_ms -= elapsed_ms;
            if (hit.counter_ms > 0) {
                createHealthBar(motion.position, unit.health, unit.max_health);
            }
        }
    }


}
