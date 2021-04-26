#pragma once

#include "common.hpp"
#include "entt.hpp"

class ParticleSystem
{
public:
    
    static entt::entity createParticle(vec2 velocity, vec2 position, float life, std::string texture, std::string shader);
    static void updateParticle();
    
    float life;
    static std::array<GLfloat, MAX_PARTICLES * 2> g_particule_position_size_data;
    static int PARTICLE_COUNT;

};
