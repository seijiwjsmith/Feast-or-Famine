// Created by Ray

#include "forest.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity Forest::createForest(GridMap& current_map)
{
    auto entity = registry.create();
    // Create rendering primitives
    std::string key = "forest";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0) {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("units/forest.png"), "map");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_MONSTERS + SPRING_BOSS - 1;

    // Setting initial motion values
    Motion& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 3);
    motion.position = coord_to_pixel(FOREST_COORD) + 0.5f * vec2(GRID_CELL_SIZE, GRID_CELL_SIZE);
    current_map.setGridOccupancy(FOREST_COORD, FOREST, entity);

    Food& food = registry.emplace<Food>(entity);
    food.food = 100;
    food.food_production_speed = 10;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0;
    animate.state = 0;
    animate.frame_num = 1;
    animate.state_num = 1;

    registry.emplace<Forest>(entity);

    return entity;
}