#include "render.hpp"
#include "common.hpp"
#include "units/unit.hpp"
class GridNode {
public:
    ivec2 coord;
    terrain_type terrain = TERRAIN_DEFAULT;
    unit_type occupancy = NONE;
    entt::entity occupying_entity;
    int num_monsters = 0;

    static entt::entity createGridNode(terrain_type terrain, vec2 coord);
    void setTerrain(entt::entity entity, terrain_type new_terrain);
    void setOccupancy(unit_type new_occupancy, entt::entity& entity);
};


