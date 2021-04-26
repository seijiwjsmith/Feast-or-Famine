#include "entt.hpp"
#include "common.hpp"
#include <vector>
#include "text.hpp"
#include "ui.hpp"
#include "render.hpp"
#include "units/unit.hpp"
#include "config/unit_upgrade_ui_config.hpp"

struct UI_description_card
{
    static entt::entity create_UI_description_card(entt::entity);
};

const std::map<int, std::string> unit_name_str = {
    {hunter_button, "Hunter"},
    {watchtower_button, "Watchtower" },
    {green_house_button, "Greenhouse"},
    {wall_button, "Wall"},
    {exterminator_button, "Exterminator" },
    {robot_button, "Robot" },
    {priestess_button, "Priestess" },
    {snowmachine_button, "Snowmachine" }
};

const std::vector<std::string> hunter_description = {
    "Slow attack speed and single target",
    "",
    "Attack: " + std::to_string(unit_damage.at(HUNTER)),
    "Attack speed:" + std::to_string(1000.f / unit_attack_interval_ms.at(HUNTER)),
    "Attack range:" + std::to_string(unit_attack_range.at(HUNTER))
};

//const std::vector<std::string> watchtower_description = {
//    "Fast attack speed and long range.",
//    "",
//    "Attack: " + std::to_string(watchtower_unit.damage),
//    "Attack speed:" + std::to_string(1000.f / watchtower_unit.attack_interval_ms),
//    "Attack range:" + std::to_string(watchtower_unit.attack_range)
//};

const std::vector<std::string> greenhouse_description = {
    "Produce extra " + std::to_string(unit_damage.at(GREENHOUSE)) + " food each round."
};

const std::vector<std::string> wall_description = {
    "Blocks the path of certain monsters.",
    "",
    "Health: " + std::to_string(unit_health.at(WALL))
};

const std::vector<std::string> exterminator_description = {
    "Burn down enemies in a small cone",
    "",
    "Attack: " + std::to_string(unit_damage.at(EXTERMINATOR)),
    "Attack speed:" + std::to_string(1000.f / unit_attack_interval_ms.at(EXTERMINATOR)),
    "Attack range:" + std::to_string(unit_attack_range.at(EXTERMINATOR))
};

const std::vector<std::string> robot_description = {
    "Laserbeams. Enough said.",
    "",
    "Attack: " + std::to_string(unit_damage.at(ROBOT)),
    "Attack speed:" + std::to_string(1000.f / unit_attack_interval_ms.at(ROBOT)),
    "Attack range:" + std::to_string(unit_attack_range.at(ROBOT))
};

const std::vector<std::string> priestess_description = {
    "Allies in range gain attack damage and speed",
    "",
    "Attack granted: " + std::to_string(unit_damage.at(PRIESTESS)),
    "Attack speed granted: " + std::to_string((int) ((unit_attack_interval_ms.at(PRIESTESS) - 1) * 100)) + "%",
    "Aura range:" + std::to_string(unit_attack_range.at(PRIESTESS))
};

const std::vector<std::string> snowmachine_description = {
    "Slow enemies down with snowballs",
    "",
    "Slow percentage: " + std::to_string(unit_damage.at(SNOWMACHINE)) + "%",
    "Attack speed: " + std::to_string(1000.f / unit_attack_interval_ms.at(SNOWMACHINE)),
    "Attack range:" + std::to_string(unit_attack_range.at(SNOWMACHINE))
};

const std::map<int, std::vector<std::string>> unit_description_str = {
    {hunter_button, hunter_description },
    //{watchtower_button, watchtower_description },
    {green_house_button, greenhouse_description},
    {wall_button, wall_description},
    {exterminator_button, exterminator_description },
    {robot_button, robot_description },
    {priestess_button, priestess_description},
    {snowmachine_button, snowmachine_description}
};




const std::map<std::string, std::string> upgrade_short_descriptions = {
    {"path_1_upgrade_button_hunter_0", "Damage" },
    {"path_1_upgrade_button_hunter_1", "Damage" },
    {"path_1_upgrade_button_hunter_2", "Damage" },
    {"path_1_upgrade_button_hunter_3", "Damage" },
    {"path_2_upgrade_button_hunter_0", "Range" },
    {"path_2_upgrade_button_hunter_1", "Range" },
    {"path_2_upgrade_button_hunter_2", "Range" },
    {"path_2_upgrade_button_hunter_3", "Damage" },

    {"path_1_upgrade_button_greenhouse_0", "Food" },
    {"path_1_upgrade_button_greenhouse_1", "Food" },
    {"path_1_upgrade_button_greenhouse_2", "Food" },
    {"path_1_upgrade_button_greenhouse_3", "Food" },
    {"path_2_upgrade_button_greenhouse_0", "Food" },
    {"path_2_upgrade_button_greenhouse_1", "Food" },
    {"path_2_upgrade_button_greenhouse_2", "Food" },
    {"path_2_upgrade_button_greenhouse_3", "Food" },

    {"path_1_upgrade_button_exterminator_0", "Damage" },
    {"path_1_upgrade_button_exterminator_1", "Damage" },
    {"path_1_upgrade_button_exterminator_2", "Damage" },
    {"path_1_upgrade_button_exterminator_3", "Damage" },
    {"path_2_upgrade_button_exterminator_0", "Missile" },
    {"path_2_upgrade_button_exterminator_1", "Damage" },
    {"path_2_upgrade_button_exterminator_2", "Damage" },
    {"path_2_upgrade_button_exterminator_3", "Damage" },

    {"path_1_upgrade_button_robot_0", "Damage" },
    {"path_1_upgrade_button_robot_1", "Damage" },
    {"path_1_upgrade_button_robot_2", "Damage" },
    {"path_1_upgrade_button_robot_3", "Damage" },
    {"path_2_upgrade_button_robot_0", "Laser" },
    {"path_2_upgrade_button_robot_1", "Laser" },
    {"path_2_upgrade_button_robot_2", "Laser" },
    {"path_2_upgrade_button_robot_3", "Laser" },

    {"path_1_upgrade_button_priestess_0", "Buff" },
    {"path_1_upgrade_button_priestess_1", "BUff" },
    {"path_1_upgrade_button_priestess_2", "Buff" },
    {"path_1_upgrade_button_priestess_3", "Buff" },
    {"path_2_upgrade_button_priestess_0", "Faster" },
    {"path_2_upgrade_button_priestess_1", "Faster" },
    {"path_2_upgrade_button_priestess_2", "Faster" },
    {"path_2_upgrade_button_priestess_3", "Faster" },

    {"path_1_upgrade_button_snowmachine_0", "More!" },
    {"path_1_upgrade_button_snowmachine_1", "More!" },
    {"path_1_upgrade_button_snowmachine_2", "More!" },
    {"path_1_upgrade_button_snowmachine_3", "More!" },
    {"path_2_upgrade_button_snowmachine_0", "Ice" },
    {"path_2_upgrade_button_snowmachine_1", "Ice" },
    {"path_2_upgrade_button_snowmachine_2", "Ice" },
    {"path_2_upgrade_button_snowmachine_3", "Ice" },

    {"path_1_upgrade_button_wall_0", "Health" },
    {"path_1_upgrade_button_wall_1", "Health" },
    {"path_1_upgrade_button_wall_2", "Health" },
    {"path_1_upgrade_button_wall_3", "Health" },
    {"path_2_upgrade_button_wall_0", "Health" },
    {"path_2_upgrade_button_wall_1", "Tank" },
    {"path_2_upgrade_button_wall_2", "Tank" },
    {"path_2_upgrade_button_wall_3", "Tank" }
};

const std::map<std::string, std::string> selected_upgrade_title = {
    {"path_1_upgrade_button_hunter_0", "Damage Increase" },
    {"path_1_upgrade_button_hunter_1", "Damage Increase" },
    {"path_1_upgrade_button_hunter_2", "Damage Increase" },
    {"path_2_upgrade_button_hunter_0", "Range Increase" },
    {"path_2_upgrade_button_hunter_1", "Range Increase" },
    {"path_2_upgrade_button_hunter_2", "Range Increase" },

    {"path_1_upgrade_button_greenhouse_0", "Farm" },
    {"path_1_upgrade_button_greenhouse_1", "Facility" },
    {"path_1_upgrade_button_greenhouse_2", "Greenhouse" },
    {"path_2_upgrade_button_greenhouse_0", "Production Increase" },
    {"path_2_upgrade_button_greenhouse_1", "Production Increase" },
    {"path_2_upgrade_button_greenhouse_2", "Production Increase" },

    {"path_1_upgrade_button_exterminator_0", "Fiery Hot Flames" },
    {"path_1_upgrade_button_exterminator_1", "Hot and Crispy" },
    {"path_1_upgrade_button_exterminator_2", "10000 degrees" },
    {"path_2_upgrade_button_exterminator_0", "Weapon Change" },
    {"path_2_upgrade_button_exterminator_1", "Big Missile" },
    {"path_2_upgrade_button_exterminator_2", "Super Mega Death Missile" },

    {"path_1_upgrade_button_robot_0", "Hotter Beams" },
    {"path_1_upgrade_button_robot_1", "Beams of eventual death" },
    {"path_1_upgrade_button_robot_2", "Beams of Death" },
    {"path_2_upgrade_button_robot_0", "More Lasers!" },
    {"path_2_upgrade_button_robot_1", "More Lasers!" },
    {"path_2_upgrade_button_robot_2", "Laser Fiesta" },

    {"path_1_upgrade_button_priestess_0", "Bishop" },
    {"path_1_upgrade_button_priestess_1", "Archbishop" },
    {"path_1_upgrade_button_priestess_2", "Pope" },
    {"path_2_upgrade_button_priestess_0", "Praise the Light" },
    {"path_2_upgrade_button_priestess_1", "Adore the Light" },
    {"path_2_upgrade_button_priestess_2", "Bask in the Light" },

    {"path_1_upgrade_button_snowmachine_0", "More Snowballs!" },
    {"path_1_upgrade_button_snowmachine_1", "More Snowballs!" },
    {"path_1_upgrade_button_snowmachine_2", "More Snowballs!" },
    {"path_2_upgrade_button_snowmachine_0", "Blizzard" },
    {"path_2_upgrade_button_snowmachine_1", "Arctic Chill" },
    {"path_2_upgrade_button_snowmachine_2", "Absolute Zero" }, 

    {"path_1_upgrade_button_wall_0", "Health Increase" },
    {"path_1_upgrade_button_wall_1", "Health Increase" },
    {"path_1_upgrade_button_wall_2", "Health Increase" },
    {"path_2_upgrade_button_wall_0", "Attack Speed Increase" },
    {"path_2_upgrade_button_wall_1", "Attack Speed Increase" },
    {"path_2_upgrade_button_wall_2", "Attack Speed Increase" }
};

const std::map<std::string, std::vector<std::string>> selected_upgrade_descriptions = {
    {"path_1_upgrade_button_hunter_0", path_1_upgrade_button_hunter_0 },
    {"path_1_upgrade_button_hunter_1", path_1_upgrade_button_hunter_1 },
    {"path_1_upgrade_button_hunter_2", path_1_upgrade_button_hunter_2 },
    {"path_2_upgrade_button_hunter_0", path_2_upgrade_button_hunter_0 },
    {"path_2_upgrade_button_hunter_1", path_2_upgrade_button_hunter_1 },
    {"path_2_upgrade_button_hunter_2", path_2_upgrade_button_hunter_2 },

    {"path_1_upgrade_button_greenhouse_0", path_1_upgrade_button_greenhouse_0 },
    {"path_1_upgrade_button_greenhouse_1", path_1_upgrade_button_greenhouse_1 },
    {"path_1_upgrade_button_greenhouse_2", path_1_upgrade_button_greenhouse_2 },
    {"path_2_upgrade_button_greenhouse_0", path_2_upgrade_button_greenhouse_0 },
    {"path_2_upgrade_button_greenhouse_1", path_2_upgrade_button_greenhouse_1 },
    {"path_2_upgrade_button_greenhouse_2", path_2_upgrade_button_greenhouse_2 },

    {"path_1_upgrade_button_exterminator_0", path_1_upgrade_button_exterminator_0 },
    {"path_1_upgrade_button_exterminator_1", path_1_upgrade_button_exterminator_1 },
    {"path_1_upgrade_button_exterminator_2", path_1_upgrade_button_exterminator_2 },
    {"path_2_upgrade_button_exterminator_0", path_2_upgrade_button_exterminator_0 },
    {"path_2_upgrade_button_exterminator_1", path_2_upgrade_button_exterminator_1 },
    {"path_2_upgrade_button_exterminator_2", path_2_upgrade_button_exterminator_2 },

    {"path_1_upgrade_button_robot_0", path_1_upgrade_button_robot_0 },
    {"path_1_upgrade_button_robot_1", path_1_upgrade_button_robot_1 },
    {"path_1_upgrade_button_robot_2", path_1_upgrade_button_robot_2 },
    {"path_2_upgrade_button_robot_0", path_2_upgrade_button_robot_0 },
    {"path_2_upgrade_button_robot_1", path_2_upgrade_button_robot_1 },
    {"path_2_upgrade_button_robot_2", path_2_upgrade_button_robot_2 },

    {"path_1_upgrade_button_priestess_0", path_1_upgrade_button_priestess_0 },
    {"path_1_upgrade_button_priestess_1", path_1_upgrade_button_priestess_1 },
    {"path_1_upgrade_button_priestess_2", path_1_upgrade_button_priestess_2 },
    {"path_2_upgrade_button_priestess_0", path_2_upgrade_button_priestess_0 },
    {"path_2_upgrade_button_priestess_1", path_2_upgrade_button_priestess_1 },
    {"path_2_upgrade_button_priestess_2", path_2_upgrade_button_priestess_2 },

    {"path_1_upgrade_button_snowmachine_0", path_1_upgrade_button_snowmachine_0 },
    {"path_1_upgrade_button_snowmachine_1", path_1_upgrade_button_snowmachine_1 },
    {"path_1_upgrade_button_snowmachine_2", path_1_upgrade_button_snowmachine_2 },
    {"path_2_upgrade_button_snowmachine_0", path_2_upgrade_button_snowmachine_0 },
    {"path_2_upgrade_button_snowmachine_1", path_2_upgrade_button_snowmachine_1 },
    {"path_2_upgrade_button_snowmachine_2", path_2_upgrade_button_snowmachine_2 },

    {"path_1_upgrade_button_wall_0", path_1_upgrade_button_wall_0 },
    {"path_1_upgrade_button_wall_1", path_1_upgrade_button_wall_1 },
    {"path_1_upgrade_button_wall_2", path_1_upgrade_button_wall_2 },
    {"path_2_upgrade_button_wall_0", path_2_upgrade_button_wall_0 },
    {"path_2_upgrade_button_wall_1", path_2_upgrade_button_wall_1 },
    {"path_2_upgrade_button_wall_2", path_2_upgrade_button_wall_2 }
};




const std::vector<std::string> clear_description = {
    "",
};

//const std::map<int, std::vector<std::string>> weather_description = {
//    {weather_type::CLEAR, clear_description },
//    {weather_type::DROUGHT, greenhouse_description},
//    {weather_type::SNOW, wall_description},
//    {weather_type::RAIN, exterminator_description },
//    {weather_type::FOG, robot_description },
//};



const std::vector<std::string> spring_description = {
    "Reward: x2",
};

const std::vector<std::string> summer_description = {
    "Reward: x1",
};

const std::vector<std::string> fall_description = {
    "Reward: x1",
};

const std::vector<std::string> winter_description = {
    "Reward: x0.5",
};

const std::vector<std::string> final_description = {
    "Reward: x1",
};

const std::map<int, std::string> season_description = {
    {season::SPRING, "Reward: x2" },
    {season::SUMMER, "Reward: x1"},
    {season::FALL, "Reward: x1"},
    {season::WINTER, "Reward: x0.5" },
    {season::FINAL,  "Reward: x1" },
};
