#include "grid_node.hpp"

const std::string NODE_SHADER = "map";
std::string terrain_texture_path(int terrain) { return "map/" + terrain_str.at(terrain) + ".png"; };

entt::entity GridNode::createGridNode(terrain_type terrain, vec2 coord)
{
    // get up node components
    auto entity = registry.create();
    auto& node = registry.emplace<GridNode>(entity);
    node.terrain = terrain;
    node.coord = coord;

    // set up mesh components
    const std::string& key = terrain_str.at(terrain);
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(terrain_texture_path(terrain)), NODE_SHADER);
    }
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_MAP + 2;

    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
    motion.position = coord_to_pixel(coord);
    // Setting initial values, scale is 1
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1, 1);

    Animate& animate = registry.emplace<Animate>(entity);
    animate.update_interval = 2;
    registry.emplace<HighlightBool>(entity); //component that stores whether this gridnode should be highlighted

    return entity;
}

void GridNode::setTerrain(entt::entity entity, terrain_type new_terrain) {
    this->terrain = new_terrain;
    const std::string& key = terrain_str.at(new_terrain);

    auto& shaded_mesh_ref = registry.get<ShadedMeshRef>(entity);
    ShadedMesh& resource = cache_resource(key);

    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(terrain_texture_path(new_terrain)), NODE_SHADER);
    }
    else
    {
        resource.texture.load_from_file(textures_path(terrain_texture_path(new_terrain)));
    }
    shaded_mesh_ref.reference_to_cache = &resource;

    auto& animate = registry.get<Animate>(entity);
    auto& motion = registry.get<Motion>(entity);
    animate.frame_num = 1;

    if (new_terrain == TERRAIN_FIRE) {
        animate.frame_num = 2;
    }
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1, animate.frame_num);
}

void GridNode::setOccupancy(unit_type new_occupancy, entt::entity& entity) {
    this->occupancy = new_occupancy;
    if (new_occupancy != NONE) {
        this->occupying_entity = entity;
    }
}
