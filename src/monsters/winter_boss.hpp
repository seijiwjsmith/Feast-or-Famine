#pragma once

#include "common.hpp"
#include "entt.hpp"

// Winter boss that only appears in winter
struct WinterBoss
{
    // Creates all the associated render resources and default transform
    static entt::entity createWinterBossEntt(int round_number);
};
