#pragma once

#include "common.hpp"
#include "config/monster_config.hpp"
#include "world.hpp"

struct WantedBoard {
	static entt::entity createWantedBoard();
    static void updateWantedEntries(entt::entity wanted_board, std::vector<int> current_round_monster_types, int round_number, float reward_multiplier);
    static void updateWantedBoardDisplay(entt::entity wanted_board_entity, bool show);
    entt::entity wanted_title;
    std::vector<entt::entity> wanted_entries;
};

struct WantedTitle {
    static entt::entity createWantedSign();
};

struct WantedEntry {
	static entt::entity createWantedEntry(vec2 position, int monster_type, int round_number, float reward_multiplier);
	static void createMonsterIcon(entt::entity entry, vec2 position, vec2 scale, int monster_type);
	static void createMonsterText(entt::entity entry, vec2 position, vec2 scale, int monster_type, int round_number, float reward_multiplier);
	std::vector<entt::entity> monster_info;
};

inline std::string wanted_board_texture_path(const std::string& name) { return data_path() + "/textures/wanted_board/" + name; };

const std::vector<std::string> template_text = {
    "Health: ",
    "Speed: ",
    "Attack: ",
    "Path Color: ",
    "Bounty: "
};

struct MonsterInfo {
    int health;
    int speed;
    int damage;
    std::string path_color;
    int reward;
};

const MonsterInfo mob_text{
    monster_health.at(MOB),
    (int)length(monster_velocities.at(MOB)) * GRID_CELL_SIZE,
    monster_damage.at(MOB),
    Path::getPathColorString(MOB),
    monster_reward.at(MOB)
};

const MonsterInfo spring_boss_text{
    monster_health.at(SPRING_BOSS),
    (int)length(monster_velocities.at(SPRING_BOSS)) * GRID_CELL_SIZE,
    monster_damage.at(SPRING_BOSS),
    Path::getPathColorString(SPRING_BOSS),
    monster_reward.at(SPRING_BOSS)
};

const MonsterInfo summer_boss_text{
    monster_health.at(SUMMER_BOSS),
    (int)length(monster_velocities.at(SUMMER_BOSS)) * GRID_CELL_SIZE,
    monster_damage.at(SUMMER_BOSS),
    Path::getPathColorString(SUMMER_BOSS),
    monster_reward.at(SUMMER_BOSS)
};

const MonsterInfo fall_boss_text{
    monster_health.at(FALL_BOSS),
    (int)length(monster_velocities.at(FALL_BOSS)) * GRID_CELL_SIZE,
    monster_damage.at(FALL_BOSS),
    Path::getPathColorString(FALL_BOSS),
    monster_reward.at(FALL_BOSS)
};

const MonsterInfo winter_boss_text{
    monster_health.at(WINTER_BOSS),
    (int)length(monster_velocities.at(WINTER_BOSS)) * GRID_CELL_SIZE,
    monster_damage.at(WINTER_BOSS),
    Path::getPathColorString(WINTER_BOSS),
    monster_reward.at(WINTER_BOSS)
};

const std::vector<std::string> dragon_text = {
    "Health: 50000",
    "Bounty: 10000",
    "",
    "  Shoots fireballs",
    "dealing 100 damage",
    " to units and village"
};

const std::map<int, MonsterInfo> wanted_descriptions = {
    {MOB, mob_text},
    {SPRING_BOSS, spring_boss_text },
    {SUMMER_BOSS, summer_boss_text},
    {FALL_BOSS, fall_boss_text},
    {WINTER_BOSS, winter_boss_text },
    /*{SPIDER, spider_text },
    {FINAL_BOSS, "Dragon Path" },
    {FIREBALL_BOSS, "Fireball Path" },
    {BURROW_BOSS, "Mole Path" },
    {TALKY_BOI, "Helge Path" }*/
};