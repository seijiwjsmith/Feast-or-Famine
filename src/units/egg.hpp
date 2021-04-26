#pragma once

#include "entt.hpp"
#include "common.hpp"
#include "grid_map.hpp"

struct Egg
{
	static entt::entity createEgg(GridMap& current_map);
};