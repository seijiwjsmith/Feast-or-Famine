#pragma once
// Created by Ray 

#include "entt.hpp"
#include "grid_map.hpp"

struct Forest
{
    static entt::entity createForest(GridMap& current_map);
};