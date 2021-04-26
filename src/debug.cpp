// Header
#include "debug.hpp"
#include "entt.hpp"
#include "render.hpp"

#include <cmath>
#include <iostream>

#include "render_components.hpp"
#include "rig.hpp"

namespace DebugSystem
{
	void createLine(vec2 position, vec2 scale, vec3 col) {
		auto entity = registry.create();

		std::string key = "thick_line";
		ShadedMesh& resource = cache_resource(key);
		if (resource.effect.program.resource == 0) {
			// create a procedural circle
			constexpr float z = -0.1f;
			

			// Corner points
			ColoredVertex v;
			v.position = { -0.5,-0.5,z };
			v.color = col;
			resource.mesh.vertices.push_back(v);
			v.position = { -0.5,0.5,z };
			v.color = col;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,0.5,z };
			v.color = col;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,-0.5,z };
			v.color = col;
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

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		ShadedMeshRef& shaded_mesh =  registry.emplace<ShadedMeshRef>(entity, resource);
		shaded_mesh.layer = LAYER_DEBUG;

		// Create motion
		auto& motion = registry.emplace<Motion>(entity);
		motion.angle = 0.f;
		motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
		motion.position = position;
		motion.scale = scale;

		registry.emplace<DebugComponent>(entity);

	}

	void createDirectedLine(vec2 position1, vec2 position2, float width ) {
		auto entity = registry.create();

		std::string key = "thick_line";
		ShadedMesh& resource = cache_resource(key);
		if (resource.effect.program.resource == 0) {
			// create a procedural circle
			constexpr float z = -0.1f;
			vec3 red = { 0.8,0.1,0.1 };

			// Corner points
			ColoredVertex v;
			v.position = { -0.5,-0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { -0.5,0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,-0.5,z };
			v.color = red;
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

		float len = length(position1 -position2);

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
		shaded_mesh.layer = LAYER_DEBUG;
		// Create motion
		auto& motion = registry.emplace<Motion>(entity);
		motion.angle = angle;
		motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
		motion.position = (position1 + position2) / 2.0f;
		motion.scale = vec2(len, width);

		registry.emplace<DebugComponent>(entity);

	}
	void createPoint(vec2 position, float size, vec3 col ) {
		auto entity = registry.create();

		std::string key = "debug_point" + std::to_string( col.x) + std::to_string(col.y) + std::to_string(col.z);
		ShadedMesh& resource = cache_resource(key);
		if (resource.effect.program.resource == 0) {
			// create a procedural circle
			constexpr float z = -0.1f;


			// Corner points
			ColoredVertex v;
			v.position = { -0.5,-0.5,z };
			v.color = col;
			resource.mesh.vertices.push_back(v);
			v.position = { -0.5,0.5,z };
			v.color = col;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,0.5,z };
			v.color = col;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,-0.5,z };
			v.color = col;
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

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
		shaded_mesh.layer = LAYER_DEBUG;

		// Create motion
		auto& motion = registry.emplace<Motion>(entity);
		motion.angle = 0.f;
		motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
		motion.position = position;
		motion.scale = vec2(size,size);

		registry.emplace<DebugComponent>(entity);
	}
	void createBox(vec2 position, vec2 size) {
		auto scale_horizontal_line = size;
		scale_horizontal_line.y *= 0.05f;
		auto scale_vertical_line = size;
		scale_vertical_line.x *= 0.05f;
		createLine(position + vec2(0, size.y / 2.0), scale_horizontal_line);
		createLine(position + vec2(size.x / 2.0, 0.0), scale_vertical_line);

		createLine(position - vec2(0, size.y / 2.0), scale_horizontal_line);
		createLine(position - vec2(size.x / 2.0, 0.0), scale_vertical_line);
	}

	void display_rig_vertices(entt::entity character) {

		Motion root_motion = registry.get<Motion>(character);
		auto& rig = registry.get<Rig>(character);

		Transform transform;
		transform.mat = mat3(1.0f);
		transform.translate(root_motion.position);
		transform.rotate(root_motion.angle);

		for (auto chain : rig.chains) {
			
			for (auto part : chain.chain_vector) {
				auto mesh_ref = registry.get<ShadedMeshRef>(part).reference_to_cache->mesh.vertices;

				const auto& entity_transform = registry.get<Transform>(part);
				Transform temp_transform;
				temp_transform.mat = transform.mat * entity_transform.mat;

				//pivot point visualized
				auto& motion = registry.get<Motion>(part);
				vec3 a = temp_transform.mat * vec3(motion.origin.x, -motion.origin.y, 1.0f);
				DebugSystem::createPoint(a, 5.0f, vec3(0.1,0.9,0.1));
	
				//rig vertices
				for (int i = 0; i < mesh_ref.size(); i++) {
					auto& v = mesh_ref[i];
					vec3 g = temp_transform.mat * vec3(v.position.x, v.position.y, 1.0f);
					DebugSystem::createPoint(g, 5.0f);
				}
			}
		}
	}

	void clearDebugComponents() {
		auto view_debug = registry.view<DebugComponent>();
		for (auto [entity] : view_debug.each()) {
			registry.destroy(entity);
		}
	}

	bool in_debug_mode = false;
}
