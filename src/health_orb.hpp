#pragma once

#include "common.hpp"
#include "entt.hpp"

struct HealthOrb
{
	static entt::entity createHealthOrb(vec2 position, int food_amount);
	vec2 starting_point;
	float hover_distance;
	float shrink_delay_ms;
	float food_gain_amount;
	bool clicked = false;
};