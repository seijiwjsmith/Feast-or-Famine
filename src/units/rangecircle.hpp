#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "render.hpp"

// Range indicator
struct RangeCircle
{
    // Creates all the associated render resources and default transform
    static entt::entity createRangeCircle(vec2 pos, float range);
};
