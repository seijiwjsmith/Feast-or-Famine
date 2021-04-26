#pragma once

#include "common.hpp"
#include "entt.hpp"//#include "tiny_ecs.hpp"
#include "render_components.hpp"

struct InstancedMesh;
struct ShadedMesh;

// OpenGL utilities
void gl_has_errors();

// System responsible for setting up OpenGL and for rendering all the 
// visual entities in the game
class RenderSystem
{
public:
	// Initialize the window
	RenderSystem(GLFWwindow& window);

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw(GLuint billboard_vertex_buffer, GLuint particles_position_buffer);

	// Expose the creating of visual representations to other systems
	static void createSprite(ShadedMesh& mesh_container, std::string texture_path, std::string shader_name);
	static void createColoredMesh(ShadedMesh& mesh_container, std::string shader_name);
	static void createTexturedMesh(ShadedMesh& texmesh, std::string texture_path, std::string shader_name);
	// show/ hide entity
	static void hide_entity(entt::entity entity);
	static void show_entity(entt::entity entity);

	ShadedMesh screen_sprite;


private:
	// Initialize the screeen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the water shader
	void initScreenTexture();

	// Internal drawing functions for each entity type
	void drawTexturedMesh(entt::entity entity, const mat3& projection);
    
	void drawToScreen();
    
    void drawParticle(GLuint billboard_vertex_buffer, GLuint particles_position_buffer, const mat3 &projection);
    
	void animate(entt::entity entity);

	// Window handle
	GLFWwindow& window;

	// Screen texture handles
	GLuint frame_buffer;
	GLResource<RENDER_BUFFER> depth_render_buffer_id;
	//entt::entity screen_state_entity;
};
