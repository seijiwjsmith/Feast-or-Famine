#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "grid_map.hpp"

// Fall boss that only appears in fall
struct FallBoss
{
    // Creates all the associated render resources and default transform
    static entt::entity createFallBossEntt(int round_number);
};
