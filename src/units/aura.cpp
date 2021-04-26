// Header
#include "aura.hpp"

entt::entity Aura::createAura(vec2 pos, float range, entt::entity emitter)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "aura";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        // the circle has radius of 100 px
        RenderSystem::createSprite(resource, textures_path("units/aura.png"), "aura");
    }
    
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_UNITS - 1;

    // Initialize the position component
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = pos;
    // Then we scale it to whatever size is needed
    motion.scale = 2.28f * vec2(range, range);

    auto& aura = registry.emplace<Aura>(entity);
    aura.emitter = emitter;

    return entity;
}