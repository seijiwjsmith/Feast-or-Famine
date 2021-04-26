#include <map>
#include "projectile.hpp"

// this is for initial values; when the entity is created

const std::map<unit_type, int> unit_damage = {
	{HUNTER, 20},
	//{WATCHTOWER, 10},
	{GREENHOUSE, 60},
	{EXTERMINATOR, 15},
	{ROBOT, 20},
	{PRIESTESS, 5},
	{SNOWMACHINE, 50},
	{WALL, 0}
};

const std::map<unit_type, float> unit_attack_interval_ms = {
	{HUNTER, 1000.f},
	//{WATCHTOWER, 800.f},
	{GREENHOUSE, 0.f},
	{EXTERMINATOR, 2000.f},
	{ROBOT, 2000.f},
	{PRIESTESS, 1.25f},
	{SNOWMACHINE, 1500.f},
	{WALL, 0.f}
};

const std::map<unit_type, int> unit_attack_range = {
	{HUNTER, GRID_CELL_SIZE * 3},
	//{WATCHTOWER, 400},
	{GREENHOUSE, 0},
	{EXTERMINATOR, GRID_CELL_SIZE * 1.5},
	{ROBOT, 250},
	{PRIESTESS, GRID_CELL_SIZE * 2.5},
	{SNOWMACHINE, GRID_CELL_SIZE * 4},
	{WALL, 0}
};

const std::map<unit_type, int> unit_num_projectiles = {
	{HUNTER, 1},
	//{WATCHTOWER, 1},
	{GREENHOUSE, 0},
	{EXTERMINATOR, 1},
	{ROBOT, 1},
	{PRIESTESS, 0},
	{SNOWMACHINE, 1},
	{WALL, 0}
};

typedef entt::entity(*fnc_pointer)(entt::entity e_unit, entt::entity e_monster, int damage);

const std::map<unit_type, fnc_pointer> unit_create_projectiles = {
	{HUNTER, Projectile::createProjectile},
	//{WATCHTOWER, Projectile::createProjectile},
	{GREENHOUSE, NULL},
	{EXTERMINATOR, Flamethrower::createFlamethrower},
	{ROBOT, LaserBeam::createLaserBeam},
	{PRIESTESS, NULL},
	{SNOWMACHINE, Snowball::createSnowball},
	{WALL, NULL}
};

const std::map<unit_type, bool> unit_rotate = {
	{HUNTER, false},
	//{WATCHTOWER, false},
	{GREENHOUSE, false},
	{EXTERMINATOR, false},
	{ROBOT, false},
	{PRIESTESS, false},
	{SNOWMACHINE, false},
	{WALL, false}
};


const std::map<unit_type, int> unit_cost = {
	{HUNTER, 100},
	//{WATCHTOWER, 200},
	{GREENHOUSE, 250},
	{EXTERMINATOR, 250},
	{ROBOT, 500},
	{PRIESTESS, 500},
	{SNOWMACHINE, 100},
	{WALL, 100}
};

const std::map<unit_type, int> unit_sell_price = {
	{HUNTER, 100},
	//{WATCHTOWER, 150},
	{GREENHOUSE, 250},
	{EXTERMINATOR, 250},
	{ROBOT, 500},
	{PRIESTESS, 500},
	{SNOWMACHINE, 100},
	{WALL, 100}
};

const std::map<unit_type, int> unit_health = {
	{HUNTER, 100},
	//{WATCHTOWER, 100},
	{GREENHOUSE, 100},
	{EXTERMINATOR, 120},
	{ROBOT, 150},
	{PRIESTESS, 100},
	{SNOWMACHINE, 100},
	{WALL, 100}
};