#pragma once

#include "entt.hpp"
#include "rig.hpp"
#include "rope.hpp"

struct Animation_timing {
	float timing_scale = 1.0f; // only affects initialization of animations

	float loop_start = 0.0f;
	float t_fireball = 0.4f;
	float loop_end = 1.2f;


	
};
struct Attack_bool{
	bool value = true;
};


struct DragonRig {
	static entt::entity createDragon(int round_number);
};