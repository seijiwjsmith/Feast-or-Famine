#pragma once

#include "common.hpp"
#include "entt.hpp"
#include <Observer.hpp>

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
	std::vector<class Observer*> observerlist;

public:
	void step(float elapsed_ms);
	void title_screen_step(float elapsed_ms);
	void notifyObservers(entt::entity entity_i, entt::entity entity_j);
	void attach(Observer* obs);

	// Stucture to store collision information
	struct Collision
	{
		// Note, the first object is stored in the ECS container.entities
		//ECS::Entity other; // the second object involved in the collision
		//Collision(ECS::Entity& other);

		entt::entity other;
		Collision(entt::entity& other);
	};
private:
	void update_projectiles(float elapsed_ms);
};
