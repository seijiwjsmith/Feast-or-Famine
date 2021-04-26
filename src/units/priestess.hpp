#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "render.hpp"
#include "units/unit.hpp"

// Placeable Priestess unit 
struct Priestess
{
    // Creates all the associated render resources and default transform
    static entt::entity createPriestess(vec2 pos);
    static void updateBuffs();
};
