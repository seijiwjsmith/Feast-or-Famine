#include "grid_map.hpp"
#include "ai.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>

// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
entt::entity GridMap::createGridMap()
{
    auto entity = registry.create();
    // maintain a GridMap registry (we might want to have multiple maps later)
    auto& map = registry.emplace<GridMap>(entity);

    std::string key = "map";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("map/gridmap.png"), key);
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    //ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_MAP;

    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
    motion.position = WINDOW_SIZE_IN_PX/2;
    // Setting initial values, scale is 1
    motion.scale = (vec2)WINDOW_SIZE_IN_PX;

    // fill node_entity_matrix with default type grid node
    for (int x = 0; x < MAP_SIZE_IN_COORD.x; x++){
        for (int y = 0; y < MAP_SIZE_IN_COORD.y; y++){
            terrain_type terrain = TERRAIN_DEFAULT;
            map.node_entity_matrix[x][y] = GridNode::createGridNode(terrain, vec2(x, y));
            map.node_matrix[x][y] = registry.get<GridNode>(map.node_entity_matrix[x][y]);
        }
    }

    AISystem::MapAI::setRandomMapPathTerran(map, FOREST_COORD, VILLAGE_COORD);
    return entity;
}

void GridMap::setGridTerrain(ivec2 grid_coord, terrain_type terrain) {
    if (!is_inbounds(grid_coord)) {
        std::cout<<"Debug: out of bounds"<< std::endl;
        return;
    }
    auto& node = getNodeAtCoord(grid_coord);
    auto& entity = getEntityAtCoord(grid_coord);
    node.setTerrain(entity, terrain);
}

void GridMap::setGridOccupancy(ivec2 grid_coord, unit_type occupancy, entt::entity& occupying_entity) {
    if (!is_inbounds(grid_coord)) {
        std::cout<<"Debug: out of bounds"<< std::endl;
        return;
    }
    auto& motion = registry.get<Motion>(occupying_entity);
    vec2 scale = motion.scale;
    if (registry.has<Animate>(occupying_entity)) {
        auto& animate = registry.get<Animate>(occupying_entity);
        scale /= vec2(animate.frame_num, 1);
    }

    ivec2 over_hang = ((ivec2)scale - (ivec2)GRID_CELL_SIZE) / (ivec2)GRID_CELL_SIZE / 2;
    over_hang = ivec2((int)ceil(over_hang.x), (int)ceil(over_hang.y));
    for (int i = grid_coord.x - over_hang.x; i <= grid_coord.x + over_hang.x; i++) {
        for (int j = grid_coord.y - over_hang.y; j <= grid_coord.y + over_hang.y; j++) {
            ivec2 current_coord = ivec2(i, j);
            if (is_inbounds(current_coord)) {
                auto& node = getNodeAtCoord(current_coord);
                node.setOccupancy(occupancy, occupying_entity);
            }
        }
    }
}

GridNode& GridMap::getNodeAtCoord(ivec2 grid_coord) {
    return this->node_matrix[grid_coord.x][grid_coord.y];
}

entt::entity& GridMap::getEntityAtCoord(ivec2 grid_coord) {
    return this->node_entity_matrix[grid_coord.x][grid_coord.y];
}

std::vector<entt::entity> GridMap::getNodeEntitiesFromCoords(std::vector<ivec2>& grid_coords){
    std::vector<entt::entity>  path_nodes;
    for(ivec2 grid_coord : grid_coords) {
        entt::entity node = this->node_entity_matrix[grid_coord.x][grid_coord.y];
        path_nodes.push_back(node);
    }
    return path_nodes;
}

std::vector<GridNode> GridMap::getNodesFromCoords(std::vector<ivec2>& grid_coords){
    std::vector<GridNode> path_nodes;
    for(ivec2 grid_coord : grid_coords) {
        GridNode node = this->node_matrix[grid_coord.x][grid_coord.y];
        path_nodes.push_back(node);
    }
    return path_nodes;
}