// Header
#include "paths.hpp"
#include "entt.hpp"
#include "render.hpp"

#include <cmath>
#include <iostream>
#include <ui.hpp>

void Path::createPathLine(vec2 position1, vec2 position2, vec3 color, int monster_type, int num) {
	auto entity = registry.create();

	std::string key = "path" + std::to_string(monster_type);
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		// create a procedural circle
		constexpr float z = -0.1f;

		// Corner points
		ColoredVertex v;
		v.position = { -0.5,-0.5,z };
		v.color = color;
		resource.mesh.vertices.push_back(v);
		v.position = { -0.5,0.5,z };
		v.color = color;
		resource.mesh.vertices.push_back(v);
		v.position = { 0.5,0.5,z };
		v.color = color;
		resource.mesh.vertices.push_back(v);
		v.position = { 0.5,-0.5,z };
		v.color = color;
		resource.mesh.vertices.push_back(v);

		// Two triangles
		resource.mesh.vertex_indices.push_back(0);
		resource.mesh.vertex_indices.push_back(1);
		resource.mesh.vertex_indices.push_back(3);
		resource.mesh.vertex_indices.push_back(1);
		resource.mesh.vertex_indices.push_back(2);
		resource.mesh.vertex_indices.push_back(3);

		RenderSystem::createColoredMesh(resource, "colored_mesh");
	}


	vec2 dir = position2 - position1;
	vec2 n_dir = normalize(dir);
	vec2 x_axis = vec2(1, 0);
	float angle = atan2(dir.y, dir.x);

	float len = length(position1 - position2) + 5;

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = LAYER_UNITS - 1;

	auto& motion = registry.emplace<Motion>(entity);
	motion.angle = angle;
	motion.position = (position1 + position2) / 2.0f;
	motion.scale = vec2(len, 8);

	registry.emplace<HighlightBool>(entity);
	auto path = registry.emplace<Path>(entity);
	path.monster_type = monster_type;
}

void Path::createPath(std::vector<ivec2> path_coords, int monster_type, int size, int num) {
	vec3 color = getPathColor(monster_type);
	for (int i = 1; i < path_coords.size(); i++)
	{
		createPathLine(coord_to_pixel(path_coords[i - 1]), coord_to_pixel(path_coords[i]), color, monster_type, (num + i) % size);
	}
}

vec3 Path::getPathColor(int monster_type) {
	if (monster_type == MOB) {
		return { 1,1,1 };
	}
	else if (monster_type == SPRING_BOSS) {
		return { 0.6, 1.0, 0.2 };
	}
	else if (monster_type == SUMMER_BOSS) {
		return { 1, 1, 0 };
	}
	else if (monster_type == FALL_BOSS) {
		return { 1, 0.7, 0.1 };
	}
	else if (monster_type == WINTER_BOSS) {
		return { 0.5, 0.5, 1 };
	}
	else if (monster_type == FINAL_BOSS) {
		return { 1, 0, 0 };
	}
	else if (monster_type == BURROW_BOSS) {
		return { 0, 0.4, 0.8 };
	}
	else if (monster_type == SPIDER) {
		return { 128, 0, 128 };
	}
	else if (monster_type == TALKY_BOI) {
		return { 128, 128, 128 };
	}
	else {
		return { 0,0,0 };
	}
}

std::string Path::getPathColorString(int monster_type) {
	if (monster_type == MOB) {
		return "WHITE";
	}
	else if (monster_type == SPRING_BOSS) {
		return "GREEN";
	}
	else if (monster_type == SUMMER_BOSS) {
		return "YELLOW";
	}
	else if (monster_type == FALL_BOSS) {
		return "ORANGE";
	}
	else if (monster_type == WINTER_BOSS) {
		return "BLUE";
	}
	else if (monster_type == FINAL_BOSS) {
		return "Cyan";
	}
	else if (monster_type == BURROW_BOSS) {
		return "Blue";
	}
	else if (monster_type == SPIDER) {
		return "Purple";
	}
	else if (monster_type == TALKY_BOI) {
		return "Gray";
	}
	else {
		return "None";
	}
}