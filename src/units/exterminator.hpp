#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "render.hpp"
#include "units/unit.hpp"

// Placeable Exterminator unit 
struct Exterminator
{
    // Creates all the associated render resources and default transform
    static entt::entity createExterminator(vec2 pos);
    
    std::vector<entt::entity> flamethrowers; 
};
