#pragma once
#include <map>
#include "config/unit_config.hpp"
#include "config/unit_upgrade_configs.hpp"
#include "projectile.hpp"

struct Unit {
	unit_type type;
	int damage;
	float attack_interval_ms;
	float next_projectile_spawn;
	int attack_range;
	int path_1_upgrade;
	int path_2_upgrade;

	int num_projectiles;
	entt::entity(*create_projectile)(entt::entity e_unit, entt::entity e_monster, int damage);

	bool rotate;
	int upgrade_path_1_cost = 0;
	int upgrade_path_2_cost = 0;
	int cost = 0;
	int sell_price = 0;
	int health = 100;
	int max_health = 100;
	int damage_buff;
	float attack_speed_buff;
    bool is_active = true; // units created as mouse indicators should not be actively firing
	void(*play_attack_sound);
};

const Unit hunter_unit = {
	HUNTER,                                 //type
	unit_damage.at(HUNTER),		            //damage
	unit_attack_interval_ms.at(HUNTER),     //attack_interval_ms
	0,		                                //next_projectile_spawn
	unit_attack_range.at(HUNTER),	        //attack_range
	0,		                                //path_1_upgrade
	0,                                      //path_2_upgrade
	unit_num_projectiles.at(HUNTER),        //num_projectiles
	unit_create_projectiles.at(HUNTER),     //create projectile
	unit_rotate.at(HUNTER),	                //rotate
	hunter_path_1.at(0)[0],                 //upgrade_path_1_cost
	hunter_path_2.at(0)[0],                 //upgrade_path_2_cost
	unit_cost.at(HUNTER),	                //cost
	unit_sell_price.at(HUNTER),	            //sell_price
	unit_health.at(HUNTER),	                //health
	unit_health.at(HUNTER),	                //max_health
	0,                                      //damage_buff
	1                                       //attack_speed_buff
};

//const Unit watchtower_unit = {
//	WATCHTOWER,                                 //type
//	unit_damage.at(WATCHTOWER),		            //damage
//	unit_attack_interval_ms.at(WATCHTOWER),     //attack_interval_ms
//	0,		                                    //next_projectile_spawn
//	unit_attack_range.at(WATCHTOWER),	        //attack_range
//	0,		                                    //path_1_upgrade
//	0,                                          //path_2_upgrade
//	unit_num_projectiles.at(WATCHTOWER),        //num_projectiles
//	unit_create_projectiles.at(WATCHTOWER),     //create projectile
//	unit_rotate.at(WATCHTOWER),	                //rotate
//	hunter_path_1.at(0)[0],                     //upgrade_path_1_cost
//	hunter_path_2.at(0)[0],                     //upgrade_path_2_cost
//	unit_cost.at(WATCHTOWER),	                //cost
//	unit_sell_price.at(WATCHTOWER),	            //sell_price
//	unit_health.at(WATCHTOWER),	                //health
//	unit_health.at(WATCHTOWER),	                //max_health
//	0,                                          //damage_buff
//	1                                           //attack_speed_buff
//};

const Unit greenhouse_unit = {
	GREENHOUSE,                                 //type
	unit_damage.at(GREENHOUSE),		            //damage
	unit_attack_interval_ms.at(GREENHOUSE),     //attack_interval_ms
	0,		                                    //next_projectile_spawn
	unit_attack_range.at(GREENHOUSE),	        //attack_range
	0,		                                    //path_1_upgrade
	0,                                          //path_2_upgrade
	unit_num_projectiles.at(GREENHOUSE),        //num_projectiles
	unit_create_projectiles.at(GREENHOUSE),     //create projectile
	unit_rotate.at(GREENHOUSE),	                //rotate
	greenhouse_path_1.at(0)[0],                 //upgrade_path_1_cost
	0,                                          //upgrade_path_2_cost
	unit_cost.at(GREENHOUSE),	                //cost
	unit_sell_price.at(GREENHOUSE),	            //sell_price
	unit_health.at(GREENHOUSE),	                //health
	unit_health.at(GREENHOUSE),	                //max_health
	0,                                          //damage_buff
	1                                           //attack_speed_buff
};

const Unit exterminator_unit = {
	EXTERMINATOR,                                 //type
	unit_damage.at(EXTERMINATOR),		          //damage
	unit_attack_interval_ms.at(EXTERMINATOR),     //attack_interval_ms
	0,		                                      //next_projectile_spawn
	unit_attack_range.at(EXTERMINATOR),	          //attack_range
	0,		                                      //path_1_upgrade
	0,                                            //path_2_upgrade
	unit_num_projectiles.at(EXTERMINATOR),        //num_projectiles
	unit_create_projectiles.at(EXTERMINATOR),     //create projectile
	unit_rotate.at(EXTERMINATOR),	              //rotate
	exterminator_path_1.at(0)[0],                 //upgrade_path_1_cost
	exterminator_path_2.at(0)[0],                 //upgrade_path_2_cost
	unit_cost.at(EXTERMINATOR),	                  //cost
	unit_sell_price.at(EXTERMINATOR),	          //sell_price
	unit_health.at(EXTERMINATOR),	              //health
	unit_health.at(EXTERMINATOR),	              //max_health
	0,                                            //damage_buff
	1                                             //attack_speed_buff
};

const Unit priestess_unit = {
	PRIESTESS,                                  //type
	unit_damage.at(PRIESTESS),		            //damage
	unit_attack_interval_ms.at(PRIESTESS),      //attack_interval_ms
	0,		                                    //next_projectile_spawn
	unit_attack_range.at(PRIESTESS),	        //attack_range
	0,		                                    //path_1_upgrade
	0,                                          //path_2_upgrade
	unit_num_projectiles.at(PRIESTESS),         //num_projectiles
	unit_create_projectiles.at(PRIESTESS),      //create projectile
	unit_rotate.at(PRIESTESS),	                //rotate
	priestess_path_1.at(0)[0],                  //upgrade_path_1_cost
	priestess_path_2.at(0)[0],                  //upgrade_path_2_cost
	unit_cost.at(PRIESTESS),	                //cost
	unit_sell_price.at(PRIESTESS),	            //sell_price
	unit_health.at(PRIESTESS),	                //health
	unit_health.at(PRIESTESS),	                //max_health
	0,                                          //damage_buff
	1                                           //attack_speed_buff
};

const Unit snowmachine_unit = {
	SNOWMACHINE,                                 //type
	unit_damage.at(SNOWMACHINE),		         //damage
	unit_attack_interval_ms.at(SNOWMACHINE),     //attack_interval_ms
	0,		                                     //next_projectile_spawn
	unit_attack_range.at(SNOWMACHINE),	         //attack_range
	0,		                                     //path_1_upgrade
	0,                                           //path_2_upgrade
	unit_num_projectiles.at(SNOWMACHINE),        //num_projectiles
	unit_create_projectiles.at(SNOWMACHINE),     //create projectile
	unit_rotate.at(SNOWMACHINE),	             //rotate
	snowmachine_path_1.at(0)[0],                 //upgrade_path_1_cost
	snowmachine_path_2.at(0)[0],                 //upgrade_path_2_cost
	unit_cost.at(SNOWMACHINE),	                 //cost
	unit_sell_price.at(SNOWMACHINE),	         //sell_price
	unit_health.at(SNOWMACHINE),	             //health
	unit_health.at(SNOWMACHINE),	             //max_health
	0,                                           //damage_buff
	1                                            //attack_speed_buff
};

const Unit robot_unit = {
	ROBOT,                                 //type
	unit_damage.at(ROBOT),		           //damage
	unit_attack_interval_ms.at(ROBOT),     //attack_interval_ms
	0,		                               //next_projectile_spawn
	unit_attack_range.at(ROBOT),	       //attack_range
	0,		                               //path_1_upgrade
	0,                                     //path_2_upgrade
	unit_num_projectiles.at(ROBOT),        //num_projectiles
	unit_create_projectiles.at(ROBOT),     //create projectile
	unit_rotate.at(ROBOT),	               //rotate
	robot_path_1.at(0)[0],                 //upgrade_path_1_cost
	robot_path_2.at(0)[0],                 //upgrade_path_2_cost
	unit_cost.at(ROBOT),	               //cost
	unit_sell_price.at(ROBOT),	           //sell_price
	unit_health.at(ROBOT),	               //health
	unit_health.at(ROBOT),	               //max_health
	0,                                     //damage_buff
	1                                      //attack_speed_buff
};

const Unit wall_unit = {
	WALL,                                 //type
	unit_damage.at(WALL),		          //damage
	unit_attack_interval_ms.at(WALL),     //attack_interval_ms
	0,		                              //next_projectile_spawn
	unit_attack_range.at(WALL),	          //attack_range
	0,		                              //path_1_upgrade
	0,                                    //path_2_upgrade
	unit_num_projectiles.at(WALL),        //num_projectiles
	unit_create_projectiles.at(WALL),     //create projectile
	unit_rotate.at(WALL),	              //rotate
	wall_path_1.at(0)[0],                 //upgrade_path_1_cost
	0,                                    //upgrade_path_2_cost
	unit_cost.at(WALL),	                  //cost
	unit_sell_price.at(WALL),	          //sell_price
	unit_health.at(WALL),	              //health
	unit_health.at(WALL),	              //max_health
	0,                                    //damage_buff
	1                                     //attack_speed_buff
};

const std::map<unit_type, Unit> unit_configs = {
	{HUNTER, hunter_unit},
	//{WATCHTOWER, watchtower_unit},
	{GREENHOUSE, greenhouse_unit},
	{WALL, wall_unit},
	{EXTERMINATOR, exterminator_unit}
};

void upgrade_unit_path_1(entt::entity e_unit);

void upgrade_unit_path_2(entt::entity e_unit);
