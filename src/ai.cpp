// internal
#include "ai.hpp"
#include "entt.hpp"
#include <projectile.hpp>
#include <render_components.hpp>
#include <cstring>
#include <queue>
#include <BehaviorTree.cpp>
#include <BehaviorTree.hpp>
#include <monsters/spring_boss.hpp>
#include <monsters/summer_boss.hpp>
#include <monsters/fall_boss.hpp>
#include <monsters/winter_boss.hpp>
#include <monsters/final_boss.hpp>
#include <monsters/fireball_boss.hpp>
#include <units/unit.hpp>
#include <units/priestess.hpp>
#include <units/robot.hpp>
#include <units/exterminator.hpp>
#include <units/snowmachine.hpp>
#include <monsters/dragon_rig.hpp>

const size_t BULLET_UPGRADE = 2;
const size_t FLAMETHROWER_UPGRADE = 3;


AISystem::AISystem(PhysicsSystem* physics) 
{
	this->physics = physics;
	this->physics->attach(this);
}

AISystem::~AISystem() {}

// calculates the position of a monster after a given amount of time in milliseconds
vec2 AISystem::calculate_position(entt::entity animal, float time)
{
    auto& motion = registry.get<Motion>(animal);
    auto& monster = registry.get<Monster>(animal);
    float num_frames = round(time / (ELAPSED_MS * WorldSystem::speed_up_factor));

    vec2 speed = motion.velocity * (ELAPSED_MS * WorldSystem::speed_up_factor * monster.speed_multiplier) / 1000.f;
    
    int current_index = monster.current_path_index;
    vec2 current_pos = motion.position;
    ivec2 node;
    float distance = 0;
    while (num_frames > 0) {
        if (current_index + 1>= monster.path_coords.size()) {
            return coord_to_pixel(VILLAGE_COORD);
        }
        current_index += 1;
        node = monster.path_coords.at(current_index);
        distance += length(coord_to_pixel(node) - current_pos);
        speed = length(speed) * normalize(coord_to_pixel(node) - current_pos);
        float occurrance = 0;
        while (distance - length(speed) >= 0) {
            distance -= length(speed);
            num_frames -= 1;
            occurrance += 1;
            if (num_frames <= 0) return current_pos + speed * occurrance;
        }
        current_pos = coord_to_pixel(node);
    }
    return current_pos;
}

struct compare_distance_to_village {
    bool operator()(entt::entity const& e1, entt::entity const& e2) {
        return length(coord_to_pixel(VILLAGE_COORD) - registry.get<Motion>(e1).position) > length(coord_to_pixel(VILLAGE_COORD) - registry.get<Motion>(e2).position);
    }
};

void AISystem::step(float elapsed_ms)
{
	//(void)elapsed_ms; // placeholder to silence unused warning until implemented
	
	for (auto& e_unit : registry.view<Unit>()) {
		auto& unit = registry.get<Unit>(e_unit);
		// TODO: scale projectile spawn with the current speed of the game
		if (unit.is_active) {
            unit.next_projectile_spawn -= elapsed_ms;
            updateUnitTarget(e_unit);
		}
	}
}

void AISystem::updateUnitTarget(entt::entity e_unit) const {// Attack mobs if in range of hunter

        auto& motion_h = registry.get<Motion>(e_unit);
        auto& unit = registry.get<Unit>(e_unit);

        // priority queue containing Motion of all monsters to the village
        std::priority_queue<entt::entity, std::vector<entt::entity>, compare_distance_to_village> priority_queue;

        for (auto monster : registry.view<Monster>()) {
            if (registry.has<FireballBoss>(monster)) {
                continue;
            }
            auto monster_position = registry.get<Motion>(monster).position;

            float distance_to_hunter = length(monster_position - motion_h.position);

            if (distance_to_hunter < unit.attack_range || registry.has<DragonRig>(monster)) {
                priority_queue.push(monster);
            }
        }

        if (!priority_queue.empty() && !motion_h.standing) {
            auto monster = priority_queue.top();
            auto& motion_monster = registry.get<Motion>(monster);
            vec2 direction = motion_monster.position - motion_h.position;
            motion_h.angle = atan2(direction.y, direction.x);
        }

        if (unit.next_projectile_spawn <= 0.f && unit.health > 0) {

            int num_spawned_prj = 0;
            std::vector<entt::entity> projectiles;

            while (num_spawned_prj < unit.num_projectiles && !priority_queue.empty())
            {
                auto monster = priority_queue.top();
                priority_queue.pop();
                auto& motion_monster = registry.get<Motion>(monster);

                vec2 direction = motion_monster.position - motion_h.position;
                if (!motion_h.standing) {
                    motion_h.angle = atan2(direction.y, direction.x);
                }

                auto projectile = unit.create_projectile(e_unit, monster, unit.damage + unit.damage_buff);
                projectiles.push_back(projectile);
                
                num_spawned_prj += 1;

            }

            if (unit.type == ROBOT)
            {
                auto& robot = registry.get<Robot>(e_unit);
                for (auto prj_entity : robot.lasers)
                {
                    if (registry.valid(prj_entity))
                        projectiles.push_back(prj_entity);
                }
                robot.lasers = projectiles;
            }
            else if (unit.type == EXTERMINATOR && unit.path_2_upgrade == 0)
            {
                auto& exterminator = registry.get<Exterminator>(e_unit);
                for (auto prj_entity : exterminator.flamethrowers)
                {
                    if (registry.valid(prj_entity))
                        projectiles.push_back(prj_entity);
                }
                exterminator.flamethrowers = projectiles;
            }
            else if (unit.type == SNOWMACHINE && unit.path_1_upgrade == 0 && unit.path_2_upgrade > 0)
            {
                auto& snowmachine = registry.get<SnowMachine>(e_unit);
                for (auto prj_entity : snowmachine.snowfields)
                {
                    if (registry.valid(prj_entity))
                        projectiles.push_back(prj_entity);
                }
                snowmachine.snowfields = projectiles;
            }

            if (num_spawned_prj >= 1)
                unit.next_projectile_spawn = unit.attack_interval_ms / unit.attack_speed_buff;
        }
}

void AISystem::updateProjectileMonsterCollision(entt::entity monster)
{
	if (registry.has<Monster>(monster))
	{
		auto& boss = registry.get<Monster>(monster);
		if (!boss.hit)
		{
			boss.hit = true;
//
//			if (boss.speed_multiplier > 1)
//			{
//				boss.sprite = boss.run_sprite;
//				boss.frames = boss.run_frames;
//			}
		}
	}
    else {
		auto& hit_reaction = registry.get<HitReaction>(monster);
		hit_reaction.hit_bool = true;
	}
}

float get_distance(ivec2 coord1, ivec2 coord2) {
    return length((vec2)(coord1 - coord2));
}

std::vector<ivec2> AISystem::MapAI::findPathBFS(GridMap& current_map, ivec2 start_coord, ivec2 goal_coord, bool is_valid(GridMap&, ivec2), int neighbor_type) {
    std::vector<ivec2> neighbors = neighbor_map.at(neighbor_type);
    std::vector<std::vector<bool>> visited(MAP_SIZE_IN_COORD.x, std::vector<bool> (MAP_SIZE_IN_COORD.y, false));
    std::vector<std::vector<ivec2>> parent(MAP_SIZE_IN_COORD.x,std::vector<ivec2> (MAP_SIZE_IN_COORD.y, vec2(-1, -1)));

    std::queue<ivec2> queue;
    visited[start_coord.x][start_coord.y] = true;
    queue.push(start_coord);

    while (!queue.empty()) {
        ivec2 current_coord = queue.front();

        // current node is the goal node, return path
         if (current_coord == goal_coord) {
             std::vector<ivec2> path_nodes = {current_coord};
             while(current_coord != start_coord) {
                 current_coord = parent[current_coord.x][current_coord.y];
                 path_nodes.emplace_back(current_coord);
             }
             std::reverse(path_nodes.begin(), path_nodes.end());
             return path_nodes;
        }
        queue.pop();
        // check neighbors
        for (int i = 0; i < neighbors.size(); i++) {
            ivec2 nbr_coord = current_coord + neighbors.at(i);
            if (!is_valid(current_map, nbr_coord) || visited[nbr_coord.x][nbr_coord.y]) {
                continue;
            }
            visited[nbr_coord.x][nbr_coord.y] = true;
            queue.emplace(nbr_coord);
            parent[nbr_coord.x][nbr_coord.y] = current_coord;
        }
    }
    // a path does not exist between start and end.
    // should NOT reach this point for monster travel path with random map generation is in place
    return std::vector<ivec2>();
}

struct search_node {
    ivec2 coord;
    float c;
    float h;
    float f;
    search_node(ivec2 coord, float c, float h) {
        this->coord = coord;
        this->c = c;
        this->h = h;
        this->f = c + h;
    }
};

// for min priority queue on f cost of nodes
struct compare_cost {
    bool operator()(search_node const& n1, search_node const& n2) {
        return n1.f > n2.f;
    }
};

struct iterable_pq: std::priority_queue<search_node, std::vector<search_node>, compare_cost> {
    auto begin() const { return c.begin(); }
    auto end() const { return c.end(); }
};

// diagonal distance
float heuristic_diagonal_dist(GridMap& current_map, int monster_type, ivec2 from_coord, ivec2 to_coord) {
    int dx = abs(from_coord.x - to_coord.x);
    int dy = abs(from_coord.y - to_coord.y);
    float unit_move_cost = 1.f;
    // if calculating unit move (as opposed to heuristic over multiple grids), get the corresponding cost of that terrain
    if (length((vec2)(from_coord - to_coord)) <= sqrt(2)) {
        auto& node = current_map.getNodeAtCoord(to_coord);
        unit_move_cost = monster_move_cost.at({monster_type, node.terrain});
        if (node.occupancy != NONE && node.occupancy != FOREST &&  node.occupancy != VILLAGE) {
            unit_move_cost += monster_attack_cost.at(monster_type);
        }
    }
    float diag_cost = sqrt(2 * unit_move_cost);
    return unit_move_cost * (dx + dy) + (diag_cost - 2 * unit_move_cost) * min(dx, dy);
}

std::vector<ivec2> AISystem::MapAI::findPathAStar(GridMap& current_map, int monster_type, ivec2 start_coord, ivec2 goal_coord, bool is_valid(GridMap&, ivec2)) {
    std::vector<ivec2> neighbors;
    if (monster_type == MOB || monster_type == SUMMER_BOSS || monster_type == FALL_BOSS || monster_type == WINTER_BOSS) {
        neighbors = neighbor_map.at(DIRECT_NBRS);
    }
    else {
        neighbors = neighbor_map.at(ALL_NBRS);
    }

    std::vector<std::vector<search_node>> parent(MAP_SIZE_IN_COORD.x,std::vector<search_node> (MAP_SIZE_IN_COORD.y, {ivec2(-1, -1), INFINITY, INFINITY}));
    iterable_pq open;
    std::vector<search_node> closed;

    search_node start_node = {start_coord, 0, heuristic_diagonal_dist(current_map, monster_type, start_coord, goal_coord)};
    open.emplace(start_node);

    while (!open.empty()) {
        // get node with smallest f on open list
        auto current_node = open.top();
        open.pop();

        // check neighbors
        for (ivec2 neighbor : neighbors) {
            ivec2 nbr_coord = current_node.coord + neighbor;
            if (!is_valid(current_map, nbr_coord)) {
                continue;
            }
            // current node is the goal node, return path
            if (nbr_coord == goal_coord) {
                std::vector<ivec2> path_nodes = {nbr_coord, current_node.coord};
                while(current_node.coord != start_coord) {
                    current_node = parent[current_node.coord.x][current_node.coord.y];
                    path_nodes.emplace_back(current_node.coord);
                }
                std::reverse(path_nodes.begin(), path_nodes.end());
                return path_nodes;
            }

            search_node nbr_node = {nbr_coord, current_node.c + heuristic_diagonal_dist(current_map, monster_type, current_node.coord, nbr_coord),
                                    heuristic_diagonal_dist(current_map, monster_type, nbr_coord, goal_coord)};

            // skip nbr if already exists in open or closed list with lower f = c + h
            bool exists_lower = false;
            for (search_node node : open) {
                if (node.coord == nbr_coord && node.f < nbr_node.f) {
                    exists_lower = true;
                    break;
                }
            }
            if (!exists_lower) {
                for (search_node node : closed) {
                    if (node.coord == nbr_coord && node.f < nbr_node.f) {
                        exists_lower = true;
                        break;
                    }
                }
            }

            if (exists_lower) {
                continue;
            }
            parent[nbr_coord.x][nbr_coord.y] = {current_node.coord, current_node.c, current_node.h};
            open.emplace(nbr_node);
        }
        closed.emplace_back(current_node);
    }
    // a path does not exist between start and end.
    // should NOT reach this point for monster travel path with random map generation is in place
    return std::vector<ivec2>();
}

terrain_type get_random_weather_terrain(int weather) {
    std::map<terrain_type, float> weather_terrain_default_prob = season_terrain_prob.at(season);

    // multiply each prob with a rand number and weather multiplier
    for (auto& [terrain, prob] : weather_terrain_default_prob) {
        prob = uniform_dist(rng) * weather_terrain_prob_multiplier.at(std::pair(weather,terrain));
    }

    
    std::pair<terrain_type, float> max_prob;

    for (auto& [terrain, prob] : weather_terrain_default_prob) {
        if (prob > max_prob.second){
            max_prob.first = terrain;
            max_prob.second = prob;
        }
    }

    return max_prob.first;
}

void AISystem::MapAI::setRandomMapWeatherTerrain(GridMap& map, int weather) {
    for (int i = 0; i < MAP_SIZE_IN_COORD.x; i++) {
        for (int j = 0; j < MAP_SIZE_IN_COORD.y; j++) {
            auto& node = map.getNodeAtCoord(ivec2(i,j));

            if (node.terrain != TERRAIN_PAVEMENT && node.occupancy == NONE) {
                map.setGridTerrain(ivec2(i, j), get_random_weather_terrain(weather));
            }
        }
    }
}

void AISystem::MapAI::setRandomWeatherTerrain(GridMap &map, int max_rerolls, int weather) {
    for (int i = 0; i < max_rerolls; i++) {
        ivec2 random_coord(uniform_dist(rng)*MAP_SIZE_IN_COORD.x,  uniform_dist(rng)*MAP_SIZE_IN_COORD.y);
        auto& node = map.getNodeAtCoord(random_coord);
        if (node.terrain != TERRAIN_PAVEMENT && node.occupancy == NONE) {
            map.setGridTerrain(random_coord, get_random_weather_terrain(weather));
        }
    }
}

// set path
bool is_valid_terrain_path(GridMap& current_map, ivec2 coord)
{
    if (is_inbounds(coord)) {
        auto terrain = current_map.getNodeAtCoord(coord).terrain;
        auto occupancy = current_map.getNodeAtCoord(coord).occupancy;
        return terrain != TERRAIN_PAVEMENT && occupancy == NONE;
    }
    return false;
}


int get_random_direct_index () {
    float num = uniform_dist(rng);
    // 10% chance for UP and LEFT
    if (num < 0.1) {
        return 0;
    }
    if (num < 0.2) {
        return 1;
    }
    // 40% chance for DOWN and RIGHT
    if (num < 0.6) {
        return 2;
    }
    return 3;
}

ivec2 get_random_neighbor(GridMap& map, ivec2 current_coord, ivec2 end_coord, int neighbor_type) {
    std::vector<ivec2> neighbors = neighbor_map.at(neighbor_type);
    bool visited[4] = {false, false, false, false};
    int count = 0;

    int index = get_random_direct_index();
    while (count < 4) {
        // randomly roll a direction until we get an unvisited direction
        while (visited[index]) {
            index = get_random_direct_index();;
        }
        count++;
        // return this neighbor if it's in bounds and has an open path to village (don't want to block itself in)
        ivec2 nbr_coord = current_coord + neighbors.at(index);
        visited[index] = true;
        if (nbr_coord == end_coord ||
            (is_inbounds(nbr_coord) && map.getNodeAtCoord(nbr_coord).terrain != TERRAIN_PAVEMENT
             && !AISystem::MapAI::findPathBFS(map, nbr_coord, VILLAGE_COORD, is_valid_terrain_path, neighbor_type).empty())) {
            return nbr_coord;
        }
    }
    // cannot find a valid neighbor; should NOT happen
    assert(false);
    return ivec2(0,0);
}

void AISystem::MapAI::setRandomMapPathTerran(GridMap& map, ivec2 start_coord, ivec2 end_coord, terrain_type terrain) {


    map.setGridTerrain(start_coord, terrain);

    // randomly step toward end_coord
    while (start_coord != end_coord) {
        start_coord = get_random_neighbor(map, start_coord, end_coord, DIRECT_NBRS);
        map.setGridTerrain(start_coord, terrain);
    }
}

std::shared_ptr<BTSelector> AISystem::MonstersAI::createBehaviorTree() {
	std::shared_ptr <BTNode> donothing = std::make_unique<DoNothing>();
	std::shared_ptr <BTNode> grow = std::make_unique<Grow>();
	//std::shared_ptr <BTNode> stop = std::make_unique<Stop>();
	std::shared_ptr <BTNode> run = std::make_unique<Run>();
    std::shared_ptr <BTNode> knockback = std::make_unique<Knockback>();
//    std::shared_ptr <BTNode> attack = std::make_unique<Attack>();

	std::shared_ptr <BTIfCondition> conditional_donothing = std::make_unique<BTIfCondition>(
		donothing,
		[](entt::entity e) {return registry.has<Mob>(e); }
	);
	std::shared_ptr <BTIfCondition> conditional_grow = std::make_unique<BTIfCondition>(
		grow,
		[](entt::entity e) {return registry.has<WinterBoss>(e); }
	);
//    std::shared_ptr <BTIfCondition> conditional_attack = std::make_unique<BTIfCondition>(
//            attack,
//            [](entt::entity e) {
//                // must be spring boss to attack for now
//                if (!registry.has<SpringBoss>(e)) {
//                    return false;
//                }
//                // must be near the center of corresponding grid to attack
//                auto& motion = registry.get<Motion>(e);
//                ivec2 coord = pixel_to_coord(motion.position);
//                if (abs(length(coord_to_pixel(coord) - motion.position)) > length(motion.velocity) * ELAPSED_MS / 1000.f) {
//                    return false;
//                }
//
//                // can attack if next to a placeable unit that has health left
//                // TODO: allow various size monsters
//                for (ivec2 nbr : neighbor_map.at(DIRECT_NBRS)) {
//                    // check if neighbor in-bounds
//                    ivec2 nbr_coord = coord + nbr;
//                    if (!is_inbounds(nbr_coord)) {
//                        continue;
//                    }
//                    // check if containing at least one attackable unit
//                    auto& node = WorldSystem::current_map.getNodeAtCoord(nbr_coord);
//                    if (node.occupancy != NONE
//                            && registry.has<Unit>(node.occupying_entity)
//                            && registry.get<Unit>(node.occupying_entity).health > 0) {
//                       return true;
//                    }
//                }
//                return false;
//            }
//    );
	/*std::shared_ptr <BTIfCondition> conditional_stop = std::make_unique<BTIfCondition>(
		stop,
		[](entt::entity e) {return registry.has<SummerBoss>(e); }
	);*/
	std::shared_ptr <BTIfCondition> conditional_run = std::make_unique<BTIfCondition>(
		run,
		[](entt::entity e) {return registry.has<SpringBoss>(e); }
	);
    std::shared_ptr <BTIfCondition> conditional_run_sum = std::make_unique<BTIfCondition>(
        run,
        [](entt::entity e) {return registry.has<SummerBoss>(e); }
    );
    std::shared_ptr <BTIfCondition> conditional_knockback = std::make_unique<BTIfCondition>(
        knockback,
        [](entt::entity e) {return registry.has<FallBoss>(e); }
    );

	std::vector<std::shared_ptr<BTIfCondition>> cond_nodes;
	cond_nodes.push_back(conditional_donothing);
	cond_nodes.push_back(conditional_grow);
	//cond_nodes.push_back(conditional_stop);
	cond_nodes.push_back(conditional_run);
    cond_nodes.push_back(conditional_run_sum);
    cond_nodes.push_back(conditional_knockback);
    //cond_nodes.push_back(conditional_attack);

	std::shared_ptr<BTSelector> selector = std::make_unique<BTSelector>(cond_nodes);
    
    std::shared_ptr <BTIfCondition> conditional_collided = std::make_unique<BTIfCondition>(
        selector,
        [](entt::entity e) {return registry.get<Monster>(e).collided; }
    );

    std::shared_ptr<BTNode> walk = std::make_unique<Walk>();
    std::shared_ptr <BTIfCondition> conditional_walk = std::make_unique<BTIfCondition>(
        walk,
        // return true for all monsters except the final boss and its fireballs
        [](entt::entity e) {return !(registry.has<DragonRig>(e) || registry.has<FireballBoss>(e)); }
    );

    std::shared_ptr<BTNode> final_boss_walk = std::make_unique<Dragon>();
    std::shared_ptr <BTIfCondition> conditional_final_boss_walk = std::make_unique<BTIfCondition>(
        final_boss_walk,
        [](entt::entity e) {return registry.has<DragonRig>(e); }
    );

    std::shared_ptr<BTNode> fireball_walk = std::make_unique<Fireball>();
    std::shared_ptr <BTIfCondition> conditional_fireball_walk = std::make_unique<BTIfCondition>(
        fireball_walk,
        [](entt::entity e) {return registry.has<FireballBoss>(e); }
    );

    std::vector<std::shared_ptr<BTIfCondition>> walk_or_collide;
    walk_or_collide.push_back(conditional_collided);
    walk_or_collide.push_back(conditional_walk);
    walk_or_collide.push_back(conditional_final_boss_walk);
    walk_or_collide.push_back(conditional_fireball_walk);

    std::shared_ptr<BTSelector> root = std::make_unique<BTSelector>(walk_or_collide);

	return root;
}