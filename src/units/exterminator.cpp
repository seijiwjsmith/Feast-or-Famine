// Header
#include "exterminator.hpp"

entt::entity Exterminator::createExterminator(vec2 pos)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = unit_str.at(EXTERMINATOR);
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("units/exterminator.png"), "unit");
    }
    
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_UNITS;

    // Initialize the position component
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = pos;
    // Then we scale it to whatever size is needed
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1);

    auto& unit = registry.emplace<Unit>(entity);
    unit = exterminator_unit;

    registry.emplace<Exterminator>(entity);
    registry.emplace<Selectable>(entity);
    registry.emplace<HighlightBool>(entity);
    registry.emplace<HitReaction>(entity);
    return entity;
}