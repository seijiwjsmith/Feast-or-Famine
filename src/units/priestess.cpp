// Header
#include "priestess.hpp"
#include "aura.hpp"
#include <iostream>

entt::entity Priestess::createPriestess(vec2 pos)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = unit_str.at(PRIESTESS);
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("units/priestess.png"), "unit");
    }
    
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_UNITS;

    // Initialize the position component
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = pos;
    // Then we scale it to whatever size is needed
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1.5);
    motion.standing = true;
    motion.velocity = grid_to_pixel_velocity(vec2(0, 0.1));

    auto& unit = registry.emplace<Unit>(entity);
    unit = priestess_unit;

    registry.emplace<Priestess>(entity);
    registry.emplace<Selectable>(entity);
    registry.emplace<HighlightBool>(entity);
    registry.emplace<HitReaction>(entity);
    return entity;
}

void Priestess::updateBuffs() {

    for (auto e_unit : registry.view<Unit>())
    {
        if (registry.has<Priestess>(e_unit)) continue;
        auto& unit_motion = registry.get<Motion>(e_unit);

        float largest_damage_buff = 0.f;
        float largest_attack_speed_buff = 1.0f;
        for (auto e_priestess : registry.view<Priestess>()) {
            auto& priestess_motion = registry.get<Motion>(e_priestess);
            auto& priestess_unit = registry.get<Unit>(e_priestess);

            float distance = length(unit_motion.position - priestess_motion.position);
            if (distance < priestess_unit.attack_range)
            {
                largest_damage_buff = largest_damage_buff < priestess_unit.damage ? priestess_unit.damage : largest_damage_buff;
                largest_attack_speed_buff = largest_attack_speed_buff < priestess_unit.attack_interval_ms ? priestess_unit.attack_interval_ms : largest_attack_speed_buff;
            }
        }
        auto& unit = registry.get<Unit>(e_unit);
        unit.damage_buff = largest_damage_buff;
        unit.attack_speed_buff = largest_attack_speed_buff;
    }
}
