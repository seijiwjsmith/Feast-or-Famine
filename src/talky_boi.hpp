#pragma once

#include "common.hpp"
#include "entt.hpp"
#include <map>

inline std::string speakers_texture_path(const std::string& name) { return data_path() + "/textures/speakers/" + name; };

// Projectile from
struct TalkyBoi
{
    // Creates all the associated render resources and default transform
    static entt::entity createTalkyBoiEntt(int round_number);
   
};

