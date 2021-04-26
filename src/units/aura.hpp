#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "render.hpp"

// Range indicator
struct Aura
{
    // Creates all the associated render resources and default transform
    static entt::entity createAura(vec2 pos, float range, entt::entity emitter);

    entt::entity emitter;
};
