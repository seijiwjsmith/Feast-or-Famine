#pragma once

#include <vector>
#include <queue>

#include "common.hpp"
#include "entt.hpp"
#include "Observer.hpp"
#include "physics.hpp"
#include <BehaviorTree.cpp>

class AISystem : public Observer
{
public:
	AISystem(PhysicsSystem* physics);
	~AISystem();

	void step(float elapsed_ms);
	void updateProjectileMonsterCollision(entt::entity monster);

	struct MapAI {
        static bool is_walkable(GridMap& current_map, ivec2 coord)
        {
            return is_inbounds(coord);
        }
        static std::vector<ivec2> findPathBFS(GridMap& current_map,
                                              ivec2 start_coord = FOREST_COORD,
                                              ivec2 goal_coord = VILLAGE_COORD,
                                              bool is_valid(GridMap&, ivec2) = is_walkable,
                                              int neighbors = ALL_NBRS);
        static std::vector<ivec2> findPathAStar(GridMap& current_map, int monster_type,
                                                ivec2 start_coord = FOREST_COORD,
                                                ivec2 goal_coord = VILLAGE_COORD,
                                                bool is_valid(GridMap&, ivec2) = is_walkable);
        static void setRandomMapWeatherTerrain(GridMap& map, int weather);
        static void setRandomWeatherTerrain(GridMap& map, int max_rerolls, int weather);
        static void setRandomMapPathTerran(GridMap& map, ivec2 start_coord, ivec2 end_coord, terrain_type terrain = TERRAIN_PAVEMENT);
	};

    static vec2 calculate_position(entt::entity animal, float time);

	struct MonstersAI {
		static std::shared_ptr<BTSelector> createBehaviorTree();
	};

private:
	// PhysicsSystem handle
	PhysicsSystem* physics;

    void updateUnitTarget(entt::entity e_unit) const;
};
