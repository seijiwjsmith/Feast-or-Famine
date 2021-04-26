#pragma once

#include "common.hpp"
#include "entt.hpp"

// Mob monster that appears in all seasons
struct Ghost
{
    // Creates all the associated render resources and default transform
    static entt::entity createGhostEntt(vec2 position);
    float lifespan_ms = 800;
};
