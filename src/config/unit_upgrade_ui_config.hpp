#include <map>
#include <vector>

// description cards for hunter path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_hunter_0 = {
    "Current Attack Damage: " + std::to_string(unit_damage.at(HUNTER)),
    "",
    "Upgraded Attack Damage: " + std::to_string(hunter_path_1.at(1)[2]),
    "Cost: " + std::to_string(hunter_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_hunter_1 = {
    "Current Attack Damage: " + std::to_string(hunter_path_1.at(1)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(hunter_path_1.at(2)[2]),
    "Cost: " + std::to_string(hunter_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_hunter_2 = {
    "Current Attack Damage: " + std::to_string(hunter_path_1.at(2)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(hunter_path_1.at(3)[2]),
    "Cost: " + std::to_string(hunter_path_1.at(2)[0])
};

// description cards for hunter path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_hunter_0 = {
    "Current Range: " + std::to_string(unit_attack_range.at(HUNTER)),
    "",
    "Upgraded Range: " + std::to_string(hunter_path_2.at(1)[2]),
    "Cost: " + std::to_string(hunter_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_hunter_1 = {
    "Current Range: " + std::to_string(hunter_path_2.at(1)[2]),
    "",
    "Upgraded Range: " + std::to_string(hunter_path_2.at(2)[2]),
    "Cost: " + std::to_string(hunter_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_hunter_2 = {
    "Current Range: " + std::to_string(hunter_path_2.at(2)[2]),
    "",
    "Upgraded Range: " + std::to_string(hunter_path_2.at(3)[2]),
    "Cost: " + std::to_string(hunter_path_2.at(2)[0])
};

// description cards for greenhouse path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_greenhouse_0 = {
    "Current Production: " + std::to_string(unit_damage.at(GREENHOUSE)),
    "",
    "Upgraded Production: " + std::to_string(greenhouse_path_1.at(1)[2]),
    "Cost: " + std::to_string(greenhouse_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_greenhouse_1 = {
    "Current Production: " + std::to_string(greenhouse_path_1.at(1)[2]),
    "",
    "Upgraded Production: " + std::to_string(greenhouse_path_1.at(2)[2]),
    "Cost: " + std::to_string(greenhouse_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_greenhouse_2 = {
    "Current Production: " + std::to_string(greenhouse_path_1.at(2)[2]),
    "",
    "Upgraded Production: " + std::to_string(greenhouse_path_1.at(3)[2]),
    "Cost: " + std::to_string(greenhouse_path_1.at(2)[0])
};

// description cards for greenhouse path 1 upgrades
const std::vector<std::string> path_2_upgrade_button_greenhouse_0 = {
    "Current Production: " + std::to_string(unit_damage.at(GREENHOUSE)),
    "",
    "Upgraded Production: " + std::to_string(greenhouse_path_2.at(1)[2]),
    "Cost: " + std::to_string(greenhouse_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_greenhouse_1 = {
    "Current Production: " + std::to_string(greenhouse_path_2.at(1)[2]),
    "",
    "Upgraded Production: " + std::to_string(greenhouse_path_2.at(2)[2]),
    "Cost: " + std::to_string(greenhouse_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_greenhouse_2 = {
    "Current Production: " + std::to_string(greenhouse_path_2.at(2)[2]),
    "",
    "Upgraded Production: " + std::to_string(greenhouse_path_2.at(3)[2]),
    "Cost: " + std::to_string(greenhouse_path_2.at(2)[0])
};

// description cards for exterminator path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_exterminator_0 = {
    "Current Attack Damage: " + std::to_string(unit_damage.at(EXTERMINATOR)),
    "",
    "Upgraded Attack Damage: " + std::to_string(exterminator_path_1.at(1)[2]),
    "Cost: " + std::to_string(exterminator_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_exterminator_1 = {
    "Current Attack Damage: " + std::to_string(exterminator_path_1.at(1)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(exterminator_path_1.at(2)[2]),
    "Cost: " + std::to_string(exterminator_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_exterminator_2 = {
    "Current Attack Damage: " + std::to_string(exterminator_path_1.at(2)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(exterminator_path_1.at(3)[2]),
    "Cost: " + std::to_string(exterminator_path_1.at(2)[0])
};

// description cards for exterminator path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_exterminator_0 = {
    "Shoot exploding missiles instead",
    "Current Range: " + std::to_string(unit_attack_range.at(EXTERMINATOR)),
    "",
    "Increased Range: " + std::to_string(GRID_CELL_SIZE * 4),
    "Cost: " + std::to_string(exterminator_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_exterminator_1 = {
    "Current Damage: " + std::to_string(exterminator_path_2.at(1)[2]),
    "",
    "Upgraded Damage: " + std::to_string(exterminator_path_2.at(2)[2]),
    "Cost: " + std::to_string(exterminator_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_exterminator_2 = {
    "Current Damage: " + std::to_string(exterminator_path_2.at(2)[2]),
    "",
    "Upgraded Damage: " + std::to_string(exterminator_path_2.at(3)[2]),
    "Cost: " + std::to_string(exterminator_path_2.at(2)[0])
};


// description cards for robot path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_robot_0 = {
    "Current Attack Damage: " + std::to_string(unit_damage.at(ROBOT)),
    "",
    "Upgraded Attack Damage: " + std::to_string(robot_path_1.at(1)[2]),
    "Cost: " + std::to_string(robot_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_robot_1 = {
    "Current Attack Damage: " + std::to_string(robot_path_1.at(1)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(robot_path_1.at(2)[2]),
    "Cost: " + std::to_string(robot_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_robot_2 = {
    "Current Attack Damage: " + std::to_string(robot_path_1.at(2)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(robot_path_1.at(3)[2]),
    "Cost: " + std::to_string(robot_path_1.at(2)[0])
};

// description cards for robot path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_robot_0 = {
    "Like they say, 2 is better than 1",
    "",
    "Next upgrade : +1 laser",
    "Cost: " + std::to_string(robot_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_robot_1 = {
    "What can go wrong with more?",
    "",
    "Next upgrade : +1 laser",
    "Cost: " + std::to_string(robot_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_robot_2 = {
    "Last time I ask for more",
    "",
    "Next upgrade : +1 laser",
    "Cost: " + std::to_string(robot_path_2.at(2)[0])
};


// description cards for priestess path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_priestess_0 = {
    "Current Attack Buff: " + std::to_string((int) unit_damage.at(PRIESTESS)),
    "",
    "Upgraded Attack Buff: " + std::to_string((int) priestess_path_1.at(1)[2]),
    "Cost: " + std::to_string((int) priestess_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_priestess_1 = {
    "Current Attack Buff: " + std::to_string((int) priestess_path_1.at(1)[2]),
    "",
    "Upgraded Attack Buff: " + std::to_string((int) priestess_path_1.at(2)[2]),
    "Cost: " + std::to_string((int) priestess_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_priestess_2 = {
    "Current Attack Buff: " + std::to_string((int) priestess_path_1.at(2)[2]),
    "",
    "Upgraded Attack Buff: " + std::to_string((int) priestess_path_1.at(3)[2]),
    "Cost: " + std::to_string((int) priestess_path_1.at(2)[0])
};

// description cards for priestess path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_priestess_0 = {
    "Current Attack Speed Buff: " + std::to_string((int) ((unit_attack_interval_ms.at(PRIESTESS) - 1) * 100)) + "%",
    "",
    "Upgraded Attack Speed Buff: " + std::to_string((int) ((priestess_path_2.at(1)[2] - 1) * 100)) + "%",
    "Cost: " + std::to_string((int) priestess_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_priestess_1 = {
    "Current Attack Speed Buff: " + std::to_string((int) ((priestess_path_2.at(1)[2] - 1)*100)) + "%",
    "",
    "Upgraded Attack Speed Buff: " + std::to_string((int) ((priestess_path_2.at(2)[2]-1)*100)) + "%",
    "Cost: " + std::to_string((int) priestess_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_priestess_2 = {
    "Current Attack Speed Buff: " + std::to_string((int) ((priestess_path_2.at(2)[2]-1)*100)) + "%",
    "",
    "Upgraded Attack Speed Buff: " + std::to_string((int) ((priestess_path_2.at(3)[2]-1)*100)) + "%",
    "Cost: " + std::to_string((int) priestess_path_2.at(2)[0])
};


// description cards for snowmachine path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_snowmachine_0 = {
    "Snowballs!",
    "",
    "Next upgrade: +1 snowball",
    "Cost: " + std::to_string(snowmachine_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_snowmachine_1 = {
    "Snowballs!",
    "",
    "Next upgrade: +1 snowball",
    "Cost: " + std::to_string(snowmachine_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_snowmachine_2 = {
    "Snowballs!",
    "",
    "Next upgrade: +1 snowball",
    "Cost: " + std::to_string(snowmachine_path_1.at(2)[0])
};

// description cards for snowmachine path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_snowmachine_0 = {
    "No more snowballs!",
    "Creates ice field to slow enemies",
    "Range: " + std::to_string(GRID_CELL_SIZE * 2),
    "Cost: " + std::to_string(snowmachine_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_snowmachine_1 = {
    "You shall be slowed!",
    "",
    "Upgraded Slow: " + std::to_string(snowmachine_path_2.at(2)[2]),
    "Cost: " + std::to_string(snowmachine_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_snowmachine_2 = {
    "You shall (pretty much) not ",
    "pass"
    "",
    "Upgraded Slow: " + std::to_string(snowmachine_path_2.at(3)[2]),
    "Cost: " + std::to_string(snowmachine_path_2.at(2)[0])
};

// description cards for wall path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_wall_0 = {
    "Reinforced Wall",
    "",
    "Upgraded Health: " + std::to_string(wall_path_1.at(1)[2]),
    "Cost: " + std::to_string(wall_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_wall_1 = {
    "Barricade"
    "",
    "Upgraded Health: " + std::to_string(wall_path_1.at(2)[2]),
    "Cost: " + std::to_string(wall_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_wall_2 = {
    "Great Wall of China",
    "",
    "Upgraded Health: " + std::to_string(wall_path_1.at(3)[2]),
    "Cost: " + std::to_string(wall_path_1.at(2)[0])
};

// description cards for wall path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_wall_0 = {
    "Current Range: " + std::to_string(unit_attack_range.at(WALL)),
    "",
    "Upgraded Range: " + std::to_string(wall_path_2.at(1)[2]),
    "Cost: " + std::to_string(wall_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_wall_1 = {
    "Current Range: " + std::to_string(wall_path_2.at(1)[2]),
    "",
    "Upgraded Range: " + std::to_string(wall_path_2.at(2)[2]),
    "Cost: " + std::to_string(wall_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_wall_2 = {
    "Current Range: " + std::to_string(wall_path_2.at(2)[2]),
    "",
    "Upgraded Range: " + std::to_string(wall_path_2.at(3)[2]),
    "Cost: " + std::to_string(wall_path_2.at(2)[0])
};


const std::map<std::string, std::string> upgrade_image_paths = {
    {"path_1_upgrade_button_hunter_0", "damage_upgrade.png" },
    {"path_1_upgrade_button_hunter_1", "damage_upgrade.png" },
    {"path_1_upgrade_button_hunter_2", "damage_upgrade.png" },
    {"path_1_upgrade_button_hunter_3", "damage_upgrade.png" },
    {"path_2_upgrade_button_hunter_0", "range_upgrade.png" },
    {"path_2_upgrade_button_hunter_1", "range_upgrade.png" },
    {"path_2_upgrade_button_hunter_2", "range_upgrade.png" },
    {"path_2_upgrade_button_hunter_3", "range_upgrade.png" },

    {"path_1_upgrade_button_greenhouse_0", "farm_upgrade.png" },
    {"path_1_upgrade_button_greenhouse_1", "facility_upgrade.png" },
    {"path_1_upgrade_button_greenhouse_2", "greenhouse_upgrade.png" },
    {"path_1_upgrade_button_greenhouse_3", "greenhouse_upgrade.png" },
    {"path_2_upgrade_button_greenhouse_0", "Food" },
    {"path_2_upgrade_button_greenhouse_1", "Food" },
    {"path_2_upgrade_button_greenhouse_2", "Food" },
    {"path_2_upgrade_button_greenhouse_3", "Food" },

    {"path_1_upgrade_button_exterminator_0", "fire_upgrade_0.png" },
    {"path_1_upgrade_button_exterminator_1", "fire_upgrade_1.png" },
    {"path_1_upgrade_button_exterminator_2", "fire_upgrade_2.png" },
    {"path_1_upgrade_button_exterminator_3", "fire_upgrade_2.png" },
    {"path_2_upgrade_button_exterminator_0", "missile_upgrade.png" },
    {"path_2_upgrade_button_exterminator_1", "explosion_upgrade_1.png" },
    {"path_2_upgrade_button_exterminator_2", "explosion_upgrade_2.png" },
    {"path_2_upgrade_button_exterminator_3", "explosion_upgrade_2.png" },

    {"path_1_upgrade_button_robot_0", "robot_damage_upgrade.png" },
    {"path_1_upgrade_button_robot_1", "robot_damage_upgrade.png" },
    {"path_1_upgrade_button_robot_2", "robot_damage_upgrade_2.png" },
    {"path_1_upgrade_button_robot_3", "robot_damage_upgrade_2.png" },
    {"path_2_upgrade_button_robot_0", "robot_laser_upgrade.png" },
    {"path_2_upgrade_button_robot_1", "robot_laser_upgrade.png" },
    {"path_2_upgrade_button_robot_2", "robot_laser_upgrade.png" },
    {"path_2_upgrade_button_robot_3", "robot_laser_upgrade.png" },

    {"path_1_upgrade_button_priestess_0", "attack_buff_upgrade.png" },
    {"path_1_upgrade_button_priestess_1", "attack_buff_upgrade.png" },
    {"path_1_upgrade_button_priestess_2", "attack_buff_upgrade.png" },
    {"path_1_upgrade_button_priestess_3", "attack_buff_upgrade.png" },
    {"path_2_upgrade_button_priestess_0", "attack_speed_upgrade.png" },
    {"path_2_upgrade_button_priestess_1", "attack_speed_upgrade.png" },
    {"path_2_upgrade_button_priestess_2", "attack_speed_upgrade.png" },
    {"path_2_upgrade_button_priestess_3", "attack_speed_upgrade.png" },

    {"path_1_upgrade_button_snowmachine_0", "snowball_upgrade.png" },
    {"path_1_upgrade_button_snowmachine_1", "snowball_upgrade.png" },
    {"path_1_upgrade_button_snowmachine_2", "snowball_upgrade.png" },
    {"path_1_upgrade_button_snowmachine_3", "snowball_upgrade.png" },
    {"path_2_upgrade_button_snowmachine_0", "ice_field_upgrade.png" },
    {"path_2_upgrade_button_snowmachine_1", "ice_field_upgrade.png" },
    {"path_2_upgrade_button_snowmachine_2", "ice_field_upgrade.png" },
    {"path_2_upgrade_button_snowmachine_3", "ice_field_upgrade.png" },

    {"path_1_upgrade_button_wall_0", "health_upgrade.png" },
    {"path_1_upgrade_button_wall_1", "health_upgrade.png" },
    {"path_1_upgrade_button_wall_2", "health_upgrade.png" },
    {"path_1_upgrade_button_wall_3", "health_upgrade.png" },
    {"path_2_upgrade_button_wall_0", "Tank" },
    {"path_2_upgrade_button_wall_1", "Tank" },
    {"path_2_upgrade_button_wall_2", "Tank" },
    {"path_2_upgrade_button_wall_3", "Tank" }
};