#pragma once

#include "common.hpp"
#include "entt.hpp"

// Mob monster that appears in all seasons
struct Mob
{
    // Creates all the associated render resources and default transform
    static entt::entity createMobEntt(int round_number);
};
