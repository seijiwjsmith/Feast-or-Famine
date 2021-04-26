// Created by Ray 

#include "menu.hpp"
#include "render.hpp"

entt::entity Menu::createMenu(float x, float y, std::string menu_name, Menu_texture texture, int layer, vec2 scale)
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "menu" + menu_name;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		std::string texture_file_name = "title_screen.png";
		switch (texture)
		{
			case title_screen:
				texture_file_name = "title_screen_background.png";
				break;
			case controls:
				texture_file_name = "controls_background.png";
				break;
			case help_menu:
				texture_file_name = "help_menu.png";
				break;
			case pause_menu:
				texture_file_name = "pause_menu_background.png";
				break;
			case lost_game:
				texture_file_name = "lost_game.png";
				break;
			case title_screen_title:
				texture_file_name = "title_screen_title.png";
				break;
			case title_screen_title2:
				texture_file_name = "title_screen_title2.png";
				break;
			case title_screen_title2_or:
				texture_file_name = "title_screen_title2_or.png";
				break;
			case foreground_trees:
				texture_file_name = "foreground_trees.png";
				break;
			case sign_post:
				texture_file_name = "sign_post.png";
				break;
			case victory_background:
				texture_file_name = "victory_screen.png";
				break;
			default:
				texture_file_name = "title_screen.png";
				break;
		}
		RenderSystem::createSprite(resource, menu_texture_path(texture_file_name), "textured");
	}
	
	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = layer;

	// Setting initial motion values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.angle = 0.f;
	ui_element.tag = menu_name;
	ui_element.scale = scale * static_cast<vec2>(resource.texture.size);
	ui_element.position = { x, y };

	registry.emplace<Menu>(entity);

	return entity;
}

entt::entity Menu::createMenu(float x, float y, std::string menu_name, std::string texture_path, int layer, vec2 scale)
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "menu" + menu_name;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();		
		RenderSystem::createSprite(resource, textures_path(texture_path), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = layer;

	// Setting initial motion values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.angle = 0.f;
	ui_element.tag = menu_name;
	ui_element.scale = scale * static_cast<vec2>(resource.texture.size);
	ui_element.position = { x, y };

	registry.emplace<Menu>(entity);

	return entity;
}

entt::entity Menu::createLostMenu()
{
	auto background_entity = registry.create();
	// Create rendering primitives
	std::string key = "lost_game_screen";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("lost_screen.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(background_entity, resource);
	shaded_mesh_ref.layer = LAYER_MENU;

	// Setting initial motion values
	UI_element& ui_element = registry.emplace<UI_element>(background_entity);
	ui_element.tag = "lost_game_screen";
	//ui_element.scale = vec2({ WINDOW_SIZE_IN_PX.x / resource.texture.size.x, WINDOW_SIZE_IN_PX.y / resource.texture.size.y }) * static_cast<vec2>(resource.texture.size);
	ui_element.scale = vec2({ 0.5f, 0.5f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2 };
	registry.emplace<Menu>(background_entity);
	
	// rabbit animation
	auto rabbits_entity = registry.create();
	std::string rabbit_key = "rabbits_eating_animation";
	ShadedMesh& rabbit_resource = cache_resource(rabbit_key);
	if (rabbit_resource.effect.program.resource == 0) {
		rabbit_resource = ShadedMesh();
		RenderSystem::createSprite(rabbit_resource, menu_texture_path("rabbit_eating_anmiation.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& rabbit_shaded_mesh_ref = registry.emplace<ShadedMeshRef>(rabbits_entity, rabbit_resource);
	rabbit_shaded_mesh_ref.layer = LAYER_MENU + 1;
	auto& anime = registry.emplace<Animate>(rabbits_entity);
	anime.state_num = 1;
	anime.frame_num = 4;
	anime.frame = 0;
	anime.state = 0;

	SoundRef& sound_ref = registry.emplace<SoundRef>(rabbits_entity);
	sound_ref.file_path = "ui/lost_screen_bite_sound.wav";
	sound_ref.is_continuous = true;
	sound_ref.play_delay_ms = 7600;

	UI_element& rabbit_ui_element = registry.emplace<UI_element>(rabbits_entity);
	rabbit_ui_element.tag = "rabbits_eating_animation";
	rabbit_ui_element.scale = vec2({ 0.8f, 0.8f }) * static_cast<vec2>(rabbit_resource.texture.size);
	// rabbit animation is a bit shifted to the left, offset to center
	rabbit_ui_element.position = { WINDOW_SIZE_IN_PX.x / 2 + 30, WINDOW_SIZE_IN_PX.y / 2 - 50 };

	return background_entity;
}

// helper for createVictoryScreen
void create_victory_title()
{
	// grill forground
	auto entity = registry.create();
	std::string key = "victory_title";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("victory_title.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = LAYER_MENU + 1;

	// Setting initial motion values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = key;
	ui_element.scale = vec2({ 1.0f, 1.0f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 1 / 5};

}
// helper for createVictoryScreen
void create_grill_animation()
{
	//// grill animation
	auto e_grill_animation = registry.create();
	std::string e_grill_key = "grill_animation";
	ShadedMesh& resource = cache_resource(e_grill_key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("grill_animation.png"), "textured");
	}

	//// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& grill_anime_shaded_mesh_ref = registry.emplace<ShadedMeshRef>(e_grill_animation, resource);
	grill_anime_shaded_mesh_ref.layer = LAYER_MENU + 2;
	auto& anime = registry.emplace<Animate>(e_grill_animation);
	anime.state_num = 1;
	anime.frame_num = 10;
	anime.frame = 0;
	anime.state = 0;

	UI_element& grill_anime_ui_element = registry.emplace<UI_element>(e_grill_animation);
	grill_anime_ui_element.tag = e_grill_key;
	grill_anime_ui_element.scale = vec2({ 1.5f, 1.5f }) * static_cast<vec2>(resource.texture.size);
	grill_anime_ui_element.position = { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2 };
}
// helper for createVictoryScreen
void create_meat_animation()
{
	//// grill animation
	auto entity = registry.create();
	std::string key = "meat_animation";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("meat_animation.png"), "textured");
	}

	//// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = LAYER_MENU + 3;
	auto& anime = registry.emplace<Animate>(entity);
	anime.state_num = 1;
	anime.frame_num = 5;
	anime.frame = 0;
	anime.state = 0;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = key;
	ui_element.scale = vec2({ 1.5f, 1.5f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2 };
}
// helper for createVictoryScreen
void create_fire_animation()
{
	//// grill animation
	auto entity = registry.create();
	std::string key = "fire_animation";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("fire_animation.png"), "textured");
	}

	//// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& grill_anime_shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	grill_anime_shaded_mesh_ref.layer = LAYER_MENU + 4;
	auto& anime = registry.emplace<Animate>(entity);
	anime.state_num = 1;
	anime.frame_num = 5;
	anime.frame = 0;
	anime.state = 0;

	UI_element& grill_anime_ui_element = registry.emplace<UI_element>(entity);
	grill_anime_ui_element.tag = key;
	grill_anime_ui_element.scale = vec2({ 1.5f, 1.5f }) * static_cast<vec2>(resource.texture.size);
	grill_anime_ui_element.position = { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2 };
}
// helper for createVictoryScreen
void create_grill_foreground()
{
	// grill forground
	auto e_grill_foreground = registry.create();
	std::string key = "grill_foreground";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("grill_foreground.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(e_grill_foreground, resource);
	shaded_mesh_ref.layer = LAYER_MENU + 3;

	// Setting initial motion values
	UI_element& ui_element = registry.emplace<UI_element>(e_grill_foreground);
	ui_element.tag = key;
	//ui_element.scale = vec2({ WINDOW_SIZE_IN_PX.x / resource.texture.size.x, WINDOW_SIZE_IN_PX.y / resource.texture.size.y }) * static_cast<vec2>(resource.texture.size);
	ui_element.scale = vec2({ 1.5f, 1.5f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2 };
	
}
// helper for createVictoryScreen
void create_grill_background()
{
	// grill forground
	auto e_grill_background = registry.create();
	std::string key = "grill_background";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("grill_background.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(e_grill_background, resource);
	shaded_mesh_ref.layer = LAYER_MENU + 1;

	// Setting initial motion values
	UI_element& ui_element = registry.emplace<UI_element>(e_grill_background);
	ui_element.tag = key;
	//ui_element.scale = vec2({ WINDOW_SIZE_IN_PX.x / resource.texture.size.x, WINDOW_SIZE_IN_PX.y / resource.texture.size.y }) * static_cast<vec2>(resource.texture.size);
	ui_element.scale = vec2({ 1.5f, 1.5f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2 };

}

// helper for createVictoryScreen
void create_fire_wood()
{
	// fire wood
	auto entity = registry.create();
	std::string key = "fire_wood";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("fire_wood.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = LAYER_MENU + 3;

	// Setting initial motion values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = key;
	//ui_element.scale = vec2({ WINDOW_SIZE_IN_PX.x / resource.texture.size.x, WINDOW_SIZE_IN_PX.y / resource.texture.size.y }) * static_cast<vec2>(resource.texture.size);
	ui_element.scale = vec2({ 0.5f, 0.5f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = { WINDOW_SIZE_IN_PX.x / 2 + 5, WINDOW_SIZE_IN_PX.y / 2 };

}
// helper for createVictoryScreen
void create_fire_wood_animation()
{
	//// grill animation
	auto entity = registry.create();
	std::string key = "fire_wood_animation";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("fire_wood_animation.png"), "textured");
	}

	//// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& grill_anime_shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	grill_anime_shaded_mesh_ref.layer = LAYER_MENU + 4;
	auto& anime = registry.emplace<Animate>(entity);
	anime.state_num = 1;
	anime.frame_num = 5;
	anime.frame = 0;
	anime.state = 0;

	UI_element& grill_anime_ui_element = registry.emplace<UI_element>(entity);
	grill_anime_ui_element.tag = key;
	grill_anime_ui_element.scale = vec2({ 0.5f, 0.5f }) * static_cast<vec2>(resource.texture.size);
	grill_anime_ui_element.position = { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2 + 90};
}

entt::entity Menu::createVictoryScreen()
{
	auto background_entity = registry.create();
	// Create rendering primitives
	std::string key = "victory_screen";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("victory_screen.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(background_entity, resource);
	shaded_mesh_ref.layer = LAYER_MENU;

	// Setting initial motion values
	UI_element& ui_element = registry.emplace<UI_element>(background_entity);
	ui_element.tag = key;
	//ui_element.scale = vec2({ WINDOW_SIZE_IN_PX.x / resource.texture.size.x, WINDOW_SIZE_IN_PX.y / resource.texture.size.y }) * static_cast<vec2>(resource.texture.size);
	ui_element.scale = vec2({ 0.5f, 0.5f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2 };
	registry.emplace<Menu>(background_entity);

	create_victory_title();
	create_grill_animation();
	create_meat_animation();
	create_grill_foreground();
	create_grill_background();
	create_fire_animation();
	//create_fire_wood();
	create_fire_wood_animation();

	return background_entity;
}

entt::entity TitleEyes::createTitleEyes(vec2 position)
{
	auto entity = registry.create();
	std::string key = "title_eyes";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("eyes.png"), "textured");
	}
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = LAYER_MENU + 1;
	shaded_mesh_ref.show = false;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "eyes";
	ui_element.scale = vec2({ 1.1, 1.1}) * static_cast<vec2>(resource.texture.size);
	ui_element.position = position;

	auto eyes = registry.emplace<TitleEyes>(entity);
	eyes.blink_delay_ms = rand() % 5000 + 1000;
	eyes.blink_time_ms = 200;
	eyes.show = false;
	return entity;
}
