#pragma once


#include "common.hpp"

// Data structure for pebble-specific information
namespace HealthSystem {
	void createHealthBar(vec2 position, int health, int max_health, vec3 color = vec3(0.8,0.1,0.1));

	void updateHealthComponents(float elapsed_ms);


};
