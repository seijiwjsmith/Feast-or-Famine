#include <string>
#include <map>

const int MONSTER_SCALE_HEALTH = 5;
const int BOSS_SCALE_HEALTH = 10;

const float MONSTER_SCALE_HEALTH_FACTOR = 1.25f;
const float BOSS_SCALE_HEALTH_FACTOR = 1.75f;

const float MONSTER_DAMAGE_SCALE_FACTOR = 5;
const float BOSS_DAMAGE_SCALE_FACTOR = 10;

const std::map<int, vec2> monster_velocities = {
	{ MOB, {2.5, 0}},
	{ SPIDER, {1.5, 0}},
	{ SPRING_BOSS, {3.0, 0}},
	{ SUMMER_BOSS, {2, 0}},
	{ FALL_BOSS, {1, 0}},
	{ WINTER_BOSS, {1, 0}},
	{ BURROW_BOSS, {3, 0}},
	{ FINAL_BOSS, {1, 0}},
	{ FIREBALL_BOSS, {4.5, 0.25}}
};

const std::map<int, int> monster_health = {
	{ MOB, 35},
	{ SPIDER, 80},
	{ SPRING_BOSS, 60},
	{ SUMMER_BOSS, 40},
	{ FALL_BOSS, 100},
	{ WINTER_BOSS, 120},
	{ BURROW_BOSS, 20},
	{ FINAL_BOSS, 5000},
	{ FIREBALL_BOSS, 400}
};

const std::map<int, int> monster_damage = {
	{ MOB, 10},
	{ SPIDER, 80},
	{ SPRING_BOSS, 20},
	{ SUMMER_BOSS, 30},
	{ FALL_BOSS, 40},
	{ WINTER_BOSS, 40},
	{ BURROW_BOSS, 10},
	{ FINAL_BOSS, 0},
	{ FIREBALL_BOSS, 100}
};

const std::map<int, int> monster_village_damage = {
	{ MOB, 10},
	{ SPIDER, 80},
	{ SPRING_BOSS, 20},
	{ SUMMER_BOSS, 30},
	{ FALL_BOSS, 40},
	{ WINTER_BOSS, 40},
	{ BURROW_BOSS, 10},
	{ FINAL_BOSS, 0},
	{ FIREBALL_BOSS, 100}
};


const std::map<int, int> monster_reward = {
	{ MOB, 20},
	{ SPIDER, 30},
	{ SPRING_BOSS, 30},
	{ SUMMER_BOSS, 50},
	{ FALL_BOSS, 60},
	{ WINTER_BOSS, 60},
	{ BURROW_BOSS, 10},
	{ FINAL_BOSS, 10000},
	{ FIREBALL_BOSS, 50}
};