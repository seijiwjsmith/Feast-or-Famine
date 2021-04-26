#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "render.hpp"
#include "units/unit.hpp"

// Placeable Hunter unit 
struct Hunter
{
    // Creates all the associated render resources and default transform
    static entt::entity createHunter(vec2 pos);
};
