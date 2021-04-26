// Created by Ray 

#include "camera.hpp"
#include "render.hpp"

entt::entity Camera::createCamera()
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "camera";

	// Setting initial motion values
	Motion& motion = registry.emplace<Motion>(entity);
	motion.position = {0.f, 0.f};
	motion.angle = 0.f;
	motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
	motion.scale = { 1.f, 1.f };

	registry.emplace<MouseMovement>(entity);

	return entity;
}