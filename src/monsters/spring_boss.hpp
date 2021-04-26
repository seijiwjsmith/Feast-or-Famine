#pragma once

#include "common.hpp"
#include "entt.hpp"

// Spring boss that only appears in spring
struct SpringBoss
{
    // Creates all the associated render resources and default transform
    static entt::entity createSpringBossEntt(int round_number);
};
