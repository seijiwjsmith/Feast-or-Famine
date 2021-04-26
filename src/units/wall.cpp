// Created by Ray 

#include "wall.hpp"
#include "render.hpp"
#include "unit.hpp"

entt::entity Wall::createWall(vec2 position/*, bool rotate*/) // rotation should be based on direction of path
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "wall"/* + rotate*/;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		
		/*if(rotate) 
			RenderSystem::createSprite(resource, textures_path("units/wall90.png"), "textured");
		else*/
			RenderSystem::createSprite(resource, textures_path("units/wall.png"), "unit");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = LAYER_UNITS;

	// Setting initial motion values
	Motion& motion = registry.emplace<Motion>(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
	motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1);
	motion.standing = true;
	auto& unit = registry.emplace<Unit>(entity);
	unit = wall_unit;

	registry.emplace<Wall>(entity);
	registry.emplace<Selectable>(entity);
	registry.emplace<HighlightBool>(entity);
    registry.emplace<HitReaction>(entity);
	return entity;
}