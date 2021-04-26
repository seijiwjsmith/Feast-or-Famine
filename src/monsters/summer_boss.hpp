#pragma once

#include "common.hpp"
#include "entt.hpp"

// Summer boss that only appears in summer
struct SummerBoss
{
public:
    std::string sprite;
    // Creates all the associated render resources and default transform
    static entt::entity createSummerBossEntt(int round_number);
};
