#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "render.hpp"
#include "units/unit.hpp"

// Placeable Robot unit 
struct Robot
{
    // Creates all the associated render resources and default transform
    static entt::entity createRobot(vec2 pos);

    std::vector<entt::entity> lasers;
};
