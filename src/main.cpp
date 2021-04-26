
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

// internal
#include "common.hpp"
#include "world.hpp"
#include "entt.hpp"
//#include "tiny_ecs.hpp"
#include "sound_system.hpp"
#include "render.hpp"
#include "physics.hpp"
#include "ai.hpp"
#include "debug.hpp"
#include "health_bar.hpp"

using Clock = std::chrono::high_resolution_clock;
// Note, here the window will show a width x height part of the game world, measured in px. 
// You could also define a window to show 1.5 x 1 part of your game world, where the aspect ratio depends on your window size.

struct Description {
	std::string name;
	Description(const char* str) : name(str) {};
};


// Entry point
int main()
{
	// Initialize the main systems
	PhysicsSystem physics;
	AISystem ai(&physics);
	
	WorldSystem world(WINDOW_SIZE_IN_PX, &physics);
	RenderSystem renderer(*world.window);
	SoundSystem sound_system(&world);

	world.screen_sprite = &renderer.screen_sprite;
	
	world.game_setup();
	world.create_start_menu();
    
	// Set all states to default
	//world.restart();
	auto t = Clock::now();
	// Variable timestep loop
    float elapsed_ms = ELAPSED_MS;
    
    static const GLfloat g_vertex_buffer_data[] = {
         -0.5f, -0.5f, 0.0f,
          0.5f, -0.5f, 0.0f,
         -0.5f,  0.5f, 0.0f,
          0.5f,  0.5f, 0.0f,
    };
    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);

    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 2 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float actual_ms = static_cast<float>((std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count()) / 1000.f;

		while (actual_ms < elapsed_ms) {
			now = Clock::now();
			actual_ms = static_cast<float>((std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count()) / 1000.f;
		}
		t = now;

		DebugSystem::clearDebugComponents();
		HealthSystem::updateHealthComponents(elapsed_ms);
		if (world.game_state == WorldSystem::story_card) {
			world.animate_speaker(elapsed_ms * world.speed_up_factor);
		}
		else if (world.game_state == WorldSystem::in_game) {
			if (world.player_state == WorldSystem::set_up_stage) {
				world.set_up_step(elapsed_ms * world.speed_up_factor);
				physics.step(elapsed_ms * world.speed_up_factor);
			}
			else if (world.player_state == WorldSystem::battle_stage) {
				ai.step(elapsed_ms * world.speed_up_factor);
				world.step(elapsed_ms * world.speed_up_factor);
				physics.step(elapsed_ms * world.speed_up_factor);
			}
		}
		else if (world.game_state == WorldSystem::darken_screen)
		{
			world.darken_screen_step(elapsed_ms);
		}
		else if (world.game_state == WorldSystem::lost_game_screen)
		{
			world.animation_step(elapsed_ms);
		}
		else if (world.game_state == WorldSystem::victory_screen)
		{
			world.animation_step(elapsed_ms);
		}
		else if (world.game_state == WorldSystem::GameState::start_menu)
		{
			world.title_screen_step(elapsed_ms);
			physics.title_screen_step(elapsed_ms);
		}
		sound_system.step(actual_ms);
		renderer.draw(billboard_vertex_buffer, particles_position_buffer);
	}
	//

	return EXIT_SUCCESS;
}
