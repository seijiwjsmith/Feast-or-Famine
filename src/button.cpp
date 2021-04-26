#include "button.hpp"
#include "render.hpp"

entt::entity MenuButton::create_button(float x, float y, MenuButtonType button_type, std::string button_text, vec2 scale, float angle)
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "menu_button" + std::to_string(button_type);
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		std::string texture_file_name = "empty_button.png";
		switch (button_type)
		{
			case new_game_button:
				texture_file_name = "signpost_arrow.png";
				break;
			case title_exit_button:
				texture_file_name = "signpost_arrow.png";
				break;
			case title_help_button:
				texture_file_name = "signpost_arrow.png";
				break;
			case load_game_button:
				texture_file_name = "signpost_arrow.png";
				break;			
			case exit_button:
				texture_file_name = "empty_button.png";
				break;
			case help_button:
				texture_file_name = "empty_button.png";
				break;
			case back_button:
				texture_file_name = "empty_button.png";
				break;
			case empty_button:
				texture_file_name = "empty_button.png";
				break;
			case restart_round_button:
				texture_file_name = "empty_button.png";
				break;
			case sandbox_button:
				texture_file_name = "signpost_arrow.png";
				break;
		    case survival_mode_button:
		        texture_file_name = "signpost_arrow.png";
                break;
			default:
				texture_file_name = "empty_button.png";
				break;
		}
		RenderSystem::createSprite(resource, menu_button_texture_path(texture_file_name), "textured");
	}
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = LAYER_MENU + 3;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = menu_button_ui_tag.at(button_type);
	ui_element.angle = angle;
	ui_element.scale = scale * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = vec2(x, y);

	auto& menu_button = registry.emplace<MenuButton>(entity);
	menu_button.button_type = button_type;

	// center the text 
	auto text_scale = 0.33f;
	auto x_offset = (ui_element.scale.x - (button_text.length() * text_scale * 27)) / 2;
	auto notoRegular = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
	auto& t = registry.emplace<Text>(entity, Text(button_text, notoRegular, vec2(ui_element.position.x - (ui_element.scale.x / 2) + x_offset, WINDOW_SIZE_IN_PX.y - ui_element.position.y - 7)));
	t.scale = text_scale;
	t.colour = { 0.0f, 0.0f, 0.0f };

	return entity;
}

MenuButtonType on_click_button(vec2 mouse_pos)
{
	auto view_menu_button = registry.view<UI_element, MenuButton>();
	for (auto [entity, ui_element, menu_button] : view_menu_button.each()) {
		if (sdBox(mouse_pos, ui_element.position, ui_element.scale / 2.0f) < 0.0f) {
			play_sound("ui/button_click/generic_button_click.wav");
			return menu_button.button_type;
		}
	}
	return MenuButtonType::no_menu_button_pressed;
}


entt::entity MenuButton::create_button_arrow()
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "title_button_arrow";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_button_texture_path("signpost_arrow_highlight.png"), "textured");
	}

	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = LAYER_MENU + 3;
	shaded_mesh_ref.show = false;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "title_button_arrow";
	ui_element.scale = vec2({ 1.2f, 1.2f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = { 0.f, 0.f };

	auto& sound = registry.emplace<SoundRef>(entity);
	//sound.sound_reference = Mix_LoadWAV(audio_path("ui/sign_post_hover.wav").c_str());
	sound.file_path = "ui/sign_post_hover.wav";
	sound.play_delay_ms = 0;
	sound.play_sound = false;

	return entity;
}