#pragma once
// Created by Ray 

#include "entt.hpp"
#include "common.hpp"

struct Camera
{
	static entt::entity createCamera();
};

struct MouseMovement {
	vec2 mouse_pos = { 0.f, 0.f };
	vec2 mouse_start = { 0.f, 0.f };
	unsigned int is_pan_state = 0;
};