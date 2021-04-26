#pragma once

#include "entt.hpp"
#include "render.hpp"
#include "common.hpp"

struct Rope_attachement {
	entt::entity rope_rig;
};

struct RopeRig {
	entt::entity anchor;
	vec2 offset;
	std::vector<entt::entity> chain;
	static entt::entity createRope(entt::entity anchor, int length, vec2 offset = {0,0});
	static entt::entity createRopePart(vec2 pos, std::string name);
	static void delete_rope(entt::entity rope_entity);
};

struct RopeSystem {
	static void update_rig(entt::entity rope_rig);
	static void update_physics(entt::entity rope_rig, float elapsed_ms);
};