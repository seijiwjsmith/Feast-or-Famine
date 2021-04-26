#include "entt.hpp"
#include "common.hpp"
#include "config/ai_config.hpp"
#include <cassert>
#include <iostream>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <debug.hpp>
#include <BehaviorTree.hpp>
#include <monsters/mob.hpp>
#include <world.hpp>
#include <rig.hpp>
#include <monsters/spring_boss.hpp>
#include <units/robot.hpp>
#include <units/exterminator.hpp>
#include <units/snowmachine.hpp>
#include <units/aura.hpp>

#pragma once

// A general decorator with lambda condition
class BTIfCondition : public BTNode
{
public:
	BTIfCondition(std::shared_ptr<BTNode> child, std::function<bool(entt::entity)> condition)
		: m_child(std::move(child)), m_condition(condition) {
	}

	virtual void init(entt::entity e) override {
		m_child->init(e);
	}

	virtual BTState process(entt::entity e) override {
		if (m_condition(e))
			return m_child->process(e);
		else
			return BTState::Failure;
	}
private:
	std::shared_ptr<BTNode> m_child;
	std::function<bool(entt::entity)> m_condition;
};

class BTSelector : public BTNode {
public:
	BTSelector(std::vector<std::shared_ptr<BTIfCondition>> children) :
		m_children(std::move(children)) {}

	void init(entt::entity e) override {
		for (auto child : m_children) {
			child->init(e);
		}
	}

	BTState process(entt::entity e) override {
		for (auto child : m_children) {
			BTState state = child->process(e);
			if (state == BTState::Failure) continue;
			else {
				return state;
			}
		}
		return BTState::Failure;
	}
private:
	std::vector<std::shared_ptr<BTIfCondition>> m_children;
};

// Leaf nodes
class DoNothing : public BTNode {
public:
	void init(entt::entity e) override {
		// do nothing
		(void)e;
	}

	BTState process(entt::entity e) override {
		auto& monster = registry.get<Monster>(e);
		monster.collided = false;
		return BTState::Moving;
	}
};

class Grow : public BTNode {
public:
	Grow() noexcept {
		visited = std::map<entt::entity, bool>();
	}

	void init(entt::entity e) override {
		visited[e] = true;
	}

	BTState process(entt::entity e) {
		auto& motion = registry.get<Motion>(e);
		auto& monster = registry.get<Monster>(e);

		if (visited[e]) {
			motion.scale *= 1.3;
			monster.damage *= 2;
			visited[e] = false;
			monster.collided = false;
			return BTState::Moving;
		}
		return BTState::Failure;
	}
private:
	std::map<entt::entity, bool> visited;
};

class Stop : public BTNode {
public:
	Stop() noexcept {
		frames_to_stop = std::map<entt::entity, int>();
		vel = std::map<entt::entity, vec2>();
	}

	void init(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);
		frames_to_stop[e] = 40;
		vel[e] = motion.velocity;
	}

	BTState process(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);

		if (frames_to_stop[e] > 0) {
			frames_to_stop[e] -= 1;
			motion.velocity *= 0;
			return BTState::Stopped;
		}

		else {
			motion.velocity = vel[e];
			return BTState::Moving;
		}
	}
private:
	std::map<entt::entity, int> frames_to_stop;
	std::map<entt::entity, vec2> vel;
};

//class Attack : public BTNode {
//public:
//    Attack() noexcept {
//        vel = std::map<entt::entity, vec2>();
//        next_attack = std::map<entt::entity, int>();
//        attackable_entities = std::vector<entt::entity>();
//    }
//
//    void init(entt::entity e) override {
//        auto& motion = registry.get<Motion>(e);
//        vel[e] = motion.velocity;
//        next_attack[e] = 0;
//        first_process = true;
//    }
//
//    BTState process(entt::entity e) override {
//        auto& motion = registry.get<Motion>(e);
//
//        // add all attackable units to list
//        if (first_process) {
//            ivec2 coord = pixel_to_coord(motion.position);
//            for (ivec2 nbr : neighbor_map.at(DIRECT_NBRS)) {
//                ivec2 nbr_coord = coord + nbr;
//                if (!is_inbounds(nbr_coord)) {
//                    continue;
//                }
//                auto& node = WorldSystem::current_map.getNodeAtCoord(nbr_coord);
//                if (node.occupancy != NONE
//                    && registry.has<Unit>(node.occupying_entity)
//                    && registry.get<Unit>(node.occupying_entity).health > 0) {
//                    attackable_entities.emplace_back(node.occupying_entity);
//                }
//            }
//            first_process = false;
//        }
//
//        motion.velocity *= 0;
//        if (next_attack[e] > 0) {
//            next_attack[e]--;
//            return BTState::Attack;
//        }
//
//        // attack a unit from attackable list
//        auto& monster = registry.get<Monster>(e);
//        auto& entity_to_attack = attackable_entities.back();
//        auto& unit = registry.get<Unit>(entity_to_attack);
//        next_attack[e] = monster.effect_interval;
//
//        // TODO: create on hit and damaged(hp<=0) appearances for unit
//        unit.health -= monster.damage;
//
//        auto &hit_reaction = registry.get<HitReaction>(entity_to_attack);
//        hit_reaction.counter_ms = 750;
//
//        if (unit.health <= 0) {
//            attackable_entities.pop_back();
//        }
//
//        // continue moving if no more attackables
//        if (attackable_entities.empty()) {
//            motion.velocity = vel[e];
//            first_process = true;
//            return BTState::Failure;
//        }
//        return BTState::Attack;
//    }
//private:
//    std::map<entt::entity, vec2> vel;
//    std::map<entt::entity, int> next_attack;
//    std::vector<entt::entity> attackable_entities;
//    bool first_process;
//};

class Run : public BTNode {
public:
	Run() noexcept {
		visited = std::map<entt::entity, bool>();
	}

	void init(entt::entity e) {
		visited[e] = true;
	} 

	BTState process(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);
		if (visited[e]) {
			motion.velocity *= 1.5;
			visited[e] = false;
			auto& monster = registry.get<Monster>(e);
			monster.collided = false;
			return BTState::Moving;
		}
		return BTState::Failure;
	}
private:
	std::map<entt::entity, bool> visited;
};

class Knockback : public BTNode {
public:
	Knockback() noexcept { }

	void init(entt::entity e) override {
		// do nothing
		(void)e;
	}

	BTState process(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);
		auto& monster = registry.get<Monster>(e);
		if (monster.collided) {
			motion.position -= 0.25f * motion.velocity;
			monster.collided = false;
		}
		return BTState::Failure;
	}
};

class Dragon : public BTNode { // todo probably delete from here and ai
public:
	Dragon() noexcept {}

	void init(entt::entity e) override {
		// do nothing
		(void)e;
	}

	BTState process(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);

		// stop when dragon is at edge of screen
		if (motion.position.x < 46) {
			motion.position += motion.velocity * WorldSystem::speed_up_factor;
		}

		return BTState::Dragon;
	}
};

class Fireball : public BTNode {
public:
	Fireball() noexcept {}

	void init(entt::entity e) override {
		// do nothing
		(void)e;
	}
	
	BTState process(entt::entity e) override {
		auto& monster = registry.get<Monster>(e);
		auto& motion = registry.get<Motion>(e);

		motion.position += motion.velocity * WorldSystem::speed_up_factor;

		if (motion.position.x + GRID_CELL_SIZE >= coord_to_pixel(VILLAGE_COORD).x) {
			WorldSystem::health -= monster.damage;
			motion.velocity *= 0;
			registry.destroy(e);
			return BTState::Fireball;
		}

		auto& node = WorldSystem::current_map.getNodeAtCoord(pixel_to_coord(motion.position));
		if (registry.valid(node.occupying_entity) && registry.has<Unit>(node.occupying_entity)) {
			auto& unit = registry.get<Unit>(node.occupying_entity);
			unit.health -= monster.damage;
			HitPointsText::create_hit_points_text(monster.damage, node.occupying_entity, { 1.f, 0.8, 0.f });
			play_sound("projectile/flamethrower.wav");
			registry.destroy(e);
			if (unit.health <= 0) {
				node.setOccupancy(NONE, node.occupying_entity);
				remove_unit_entity(unit.type, node.occupying_entity);
			}
		}

		return BTState::Fireball;
	}
};



class Walk : public BTNode {
public:
	Walk() noexcept {}

	void init(entt::entity e) override { 
		// do nothing
		(void)e;
	}

	BTState process(entt::entity e) override {
		return increment_monster_step(e);
	}
};

// helper for increment_monster_step
void remove_unit_entity(unit_type type, entt::entity e_unit)
{
	if (type == PRIESTESS) {
		for (auto e_aura : registry.view<Aura>()) {
			auto& aura = registry.get<Aura>(e_aura);
			if (aura.emitter == e_unit) {
				registry.destroy(e_aura);
			}
		}
	}

	else if (type == ROBOT)
	{
		auto& robot = registry.get<Robot>(e_unit);
		for (auto projectile : robot.lasers)
			if (registry.valid(projectile)) {
				registry.destroy(projectile);
			}
	}
	else if (type == EXTERMINATOR)
	{
		auto& exterminator = registry.get<Exterminator>(e_unit);
		for (auto projectile : exterminator.flamethrowers)
			if (registry.valid(projectile)) {
				registry.destroy(projectile);
			}
	}
	else if (type == SNOWMACHINE)
	{
		auto& snowmachine = registry.get<SnowMachine>(e_unit);
		for (auto projectile : snowmachine.snowfields)
			if (registry.valid(projectile)) {
				registry.destroy(projectile);
			}
	}
	else if (type == HUNTER)
	{
		play_sound("units/dying.wav");
	}
	registry.destroy(e_unit);
}

BTState handle_monster_attack(entt::entity entity, Monster& monster, GridNode& next_node) {
    if (monster.state == ATTACK) {
        auto atk_entity = next_node.occupying_entity;
        if (next_node.occupancy == NONE) {
            auto& animate = registry.get<Animate>(entity);
            if(monster.slow_walk) {
                animate.update_interval = 2;
            }
            monster.state = WALK;
            monster.sprite = monster.walk_sprite;
            monster.frames = monster.walk_frames;
            monster.setSprite(entity);
            return BTState::Moving;
        }
        auto& atk_unit = registry.get<Unit>(atk_entity);
        if (atk_unit.health <= 0) {
            next_node.setOccupancy(NONE, atk_entity);
            remove_unit_entity(atk_unit.type, atk_entity);
            WorldSystem::set_AI_paths = false;
			return BTState::Attack;
        }
        monster.next_attack -= 1;
        if (monster.next_attack < 0) {
            monster.next_attack = monster.effect_interval;

            auto& hit_reaction = registry.get<HitReaction>(atk_entity);
            hit_reaction.counter_ms = hit_reaction.counter_interval;
            hit_reaction.hit_bool = true;
            atk_unit.health -= monster.damage;
            HitPointsText::create_hit_points_text(monster.damage, atk_entity, { 1.f, 0.8, 0.f });
			//play sound
			int sound_num = rand() % 4 + 1;
			play_sound("monsters/bite_" + std::to_string(sound_num) + ".wav");
			return BTState::Attack;
        }
    }

	return BTState::Moving;
}

BTState increment_monster_step(entt::entity entity) {
	auto& monster = registry.get<Monster>(entity);
	auto& motion = registry.get<Motion>(entity);
	auto& current_path_coord = monster.path_coords.at(monster.current_path_index);

	// check that the monster is indeed within the current path node
	ivec2 coord = pixel_to_coord(motion.position);

	// if we are on the last node, stop the monster and remove entity
	// TODO: make disappearance fancier
	if (pixel_to_coord(motion.position) == VILLAGE_COORD
		|| monster.current_path_index >= monster.path_coords.size() - 1) {
		WorldSystem::deduct_health(monster.village_damage);
		motion.velocity *= 0;

		if (registry.has<Rig>(entity)) {
			Rig::delete_rig(entity); //rigs have multiple pieces to be deleted
		}
		else {
			registry.destroy(entity);
		}
		return BTState::Moving;
	}

	assert(monster.path_coords[monster.current_path_index] == current_path_coord);
	float time_step = (ELAPSED_MS / 1000.f) * WorldSystem::speed_up_factor;
	ivec2 next_path_coord = monster.path_coords.at(monster.current_path_index + 1);
	vec2 next_step_position = motion.position + time_step * motion.velocity * monster.speed_multiplier;
	ivec2 next_step_coord = pixel_to_coord(next_step_position);
    auto& next_node = WorldSystem::current_map.getNodeAtCoord(next_path_coord);
	// change direction if reached the middle of the this node
	if (!monster.current_node_visited) {
		if (length(coord_to_pixel(current_path_coord) - motion.position) <= length(motion.velocity * time_step * monster.speed_multiplier)) {
			vec2 move_direction = normalize((vec2)(next_path_coord - current_path_coord));
			motion.position = coord_to_pixel(current_path_coord);
			motion.velocity = length(motion.velocity) * move_direction;
			monster.current_node_visited = true;
			if (next_node.occupancy != NONE && registry.has<Unit>(next_node.occupying_entity)) {
                if (!registry.has<SpringBoss>(entity) || unit_create_projectiles.at(next_node.occupancy) != NULL) {
                    monster.state = ATTACK;
                    monster.sprite = monster.attack_sprite;
                    monster.frames = monster.attack_frames;
                    auto& animate = registry.get<Animate>(entity);
                    animate.update_interval = 1;
                    monster.setSprite(entity);
                }
			}
		}
	}

    auto state = handle_monster_attack(entity, monster, next_node);

    auto& curr_node = WorldSystem::current_map.getNodeAtCoord(current_path_coord);
    // fire and puddle effects on monsters
    if (monster.type != TALKY_BOI && monster.type != SPRING_BOSS) {
        if (curr_node.terrain != TERRAIN_DEFAULT && curr_node.terrain != TERRAIN_PAVEMENT && curr_node.terrain != TERRAIN_DRY) {
            monster.next_effect --;
        }
        if (monster.next_effect < 0) {
            monster.next_effect = monster.effect_interval;
            if (curr_node.terrain == TERRAIN_FIRE) {
                int fire_damage = 10;
                monster.health -= fire_damage;
                HitPointsText::create_hit_points_text(fire_damage, entity, { 1.f, 0.5, 0.f });
                auto& hit_reaction = registry.get<HitReaction>(entity);
                hit_reaction.counter_ms = hit_reaction.counter_interval;
                hit_reaction.hit_bool = true;
            }
            else if (curr_node.terrain == TERRAIN_PUDDLE) {
                EffectHitText::create_effect_hit_text("splash", entity, {0.f, 0.8, 0.8});
            }
            else if (curr_node.terrain == TERRAIN_MUD) {
                EffectHitText::create_effect_hit_text("slosh", entity, {0.8, 0.4, 0.1});
            }
            else if (curr_node.terrain == TERRAIN_ICE) {
                EffectHitText::create_effect_hit_text("slip", entity, {0.7, 1.0, 1.0});
            }
        }
    }

	// increment path index and apply terrain speed multiplier
	if (next_step_coord == next_path_coord) {
		monster.current_path_index++;
		monster.current_node_visited = false;
		terrain_type current_terran = curr_node.terrain;
		terrain_type next_terran = next_node.terrain;
		monster.speed_multiplier /= monster_move_speed_multiplier.at({monster.type, current_terran});
		monster.speed_multiplier *= monster_move_speed_multiplier.at({monster.type, next_terran});
	}

	if (DebugSystem::in_debug_mode)
	{
		DebugSystem::createDirectedLine(motion.position, coord_to_pixel(current_path_coord), 5);
		DebugSystem::createDirectedLine(coord_to_pixel(current_path_coord), coord_to_pixel(next_path_coord), 5);
	}

	return state;
}