#pragma once
// Created by Ray 

#include "entt.hpp"
#include "common.hpp"
#include "grid_map.hpp"

struct Village
{
	static entt::entity createVillage(GridMap& current_map);
};