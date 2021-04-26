#include "egg.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity Egg::createEgg(GridMap& current_map)
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "egg";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, textures_path("units/egg.png"), "egg");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = LAYER_UNITS;

	// Setting initial motion values
	Motion& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
	motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1);
    motion.position = coord_to_pixel(vec2(0, MAP_SIZE_IN_COORD.y - 1));
    current_map.setGridOccupancy(pixel_to_coord(motion.position), EGG, entity);

	registry.emplace<Egg>(entity);

	return entity;
}