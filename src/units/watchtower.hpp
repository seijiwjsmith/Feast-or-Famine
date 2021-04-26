#pragma once

#include "common.hpp"
#include "entt.hpp"

// Placeable watchtower unit 
struct WatchTower
{
    // Creates all the associated render resources and default transform
    static entt::entity createWatchTower(vec2 pos);
};
