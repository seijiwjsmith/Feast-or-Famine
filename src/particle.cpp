
#include "particle.hpp"
#include "render.hpp"
#include "common.hpp"
#include "world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>

std::array<GLfloat, MAX_PARTICLES*2>ParticleSystem::g_particule_position_size_data;
int ParticleSystem::PARTICLE_COUNT = 0;

entt::entity ParticleSystem::createParticle(vec2 velocity, vec2 position, float life, std::string texture, std::string shader) {
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = texture;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(texture), shader);
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_PARTICLES;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);

    motion.angle = 0;
    motion.velocity = velocity;
    motion.position = position;
    motion.scale *= vec2({1.0, 1.0});
    // Setting initial values, scale is negative to make it face the opposite way
    
    auto& particle = registry.emplace<ParticleSystem>(entity);
    particle.life = life;

    return entity;
}

void ParticleSystem::updateParticle() {
//    g_particule_position_size_data = new GLfloat[MAX_PARTICLES * 2]; // TODO initialize outside
    PARTICLE_COUNT = 0;
    for (auto particle_view : registry.view<ParticleSystem>()) {
        auto& particle_m = registry.get<Motion>(particle_view);
        g_particule_position_size_data[2*PARTICLE_COUNT+0] = particle_m.position.x;
        g_particule_position_size_data[2*PARTICLE_COUNT+1] = particle_m.position.y;
        PARTICLE_COUNT++;
    }
}
