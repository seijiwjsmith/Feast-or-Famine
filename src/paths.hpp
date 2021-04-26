#pragma once

#include "common.hpp"

struct Path {
	static void createPathLine(vec2 position1, vec2 position2, vec3 color, int monster_type, int num);
	static void createPath(std::vector<ivec2> path_coords, int monster_type, int size, int num);
	static vec3 getPathColor(int monster_type);
	static std::string getPathColorString(int monster_type);
	int monster_type;
};
