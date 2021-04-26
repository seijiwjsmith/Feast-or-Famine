#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "render.hpp"
#include "units/unit.hpp"

// Placeable snowmachine unit 
struct SnowMachine
{
    // Creates all the associated render resources and default transform
    static entt::entity createSnowMachine(vec2 pos);

    std::vector<entt::entity> snowfields;
};
