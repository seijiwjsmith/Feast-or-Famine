#include "health_orb.hpp"
#include "render.hpp""

entt::entity HealthOrb::createHealthOrb(vec2 position, int food_amount)
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "health_orb";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, textures_path("/monsters/health_orb.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = LAYER_UI + 1;

	// Setting initial ui_element values
	Motion& motion = registry.emplace<Motion>(entity);
	motion.scale = vec2({ 0.6f, 0.6f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	motion.position = position;
	// hover speed
	motion.velocity = { 0.f, -30.f };

	auto& health_drop = registry.emplace<HealthOrb>(entity);
	health_drop.starting_point = position;
	health_drop.shrink_delay_ms = 10000.f;
	health_drop.hover_distance = 30.f;
	// set to false if want to make players manually click the health orbs
	health_drop.clicked = true;
	health_drop.food_gain_amount = food_amount;

	auto& sound = registry.emplace<SoundRef>(entity);
	sound.on_impact_destory = true;
	//sound.sound_reference = Mix_LoadWAV(audio_path("ui/health_gain_sound.wav").c_str());
	sound.file_path = "ui/health_gain_sound.wav";
	sound.play_sound = false;

	return entity;
}