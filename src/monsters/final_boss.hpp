#pragma once

#include "common.hpp"
#include "entt.hpp"

// Final boss that appears in the last round
struct FinalBoss
{
    // Creates all the associated render resources and default transform
    static entt::entity createFinalBossEntt();
};
