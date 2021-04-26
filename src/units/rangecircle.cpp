// Header
#include "rangecircle.hpp"

entt::entity RangeCircle::createRangeCircle(vec2 pos, float range)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "RangeCircle";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        // the circle has radius of 100 px
        RenderSystem::createSprite(resource, textures_path("units/range_circle.png"), "unit");
    }
    
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_UNITS + 1;

    // Initialize the position component
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = pos;
    // Then we scale it to whatever size is needed
    motion.scale = 2.f * vec2(range, range);

    registry.emplace<RangeCircle>(entity);
    return entity;
}