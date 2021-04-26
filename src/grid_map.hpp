#pragma once

#include <iostream>
#include "common.hpp"
#include "grid_node.hpp"

class GridMap
{
public:
    static entt::entity createGridMap();

    entt::entity& getEntityAtCoord(ivec2 grid_coord);
    GridNode& getNodeAtCoord(ivec2 grid_coord);
    void setGridTerrain(ivec2 grid_coord, terrain_type terrain);
    void setGridOccupancy(ivec2 grid_coord, unit_type occupancy, entt::entity& occupying_entity);
    std::vector<entt::entity> getNodeEntitiesFromCoords(std::vector<ivec2>& grid_coords);
    std::vector<GridNode> getNodesFromCoords(std::vector<ivec2>& grid_coords);

private:
    std::vector<std::vector<entt::entity>> node_entity_matrix = std::vector<std::vector<entt::entity>>(MAP_SIZE_IN_COORD.x, std::vector<entt::entity>(MAP_SIZE_IN_COORD.y));
    std::vector<std::vector<GridNode>> node_matrix = std::vector<std::vector<GridNode>>(MAP_SIZE_IN_COORD.x, std::vector<GridNode>(MAP_SIZE_IN_COORD.y));
};