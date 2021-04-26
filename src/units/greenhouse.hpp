#pragma once

#include "common.hpp"
#include "entt.hpp"

// Placeable GreenHouse unit 
struct GreenHouse
{
    // Creates all the associated render resources and default transform
    static entt::entity createGreenHouse(vec2 pos);
    int workers = 0;
    int food_production = workers * 5;
    int next_grow = 10;
};
