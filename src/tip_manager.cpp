#include <tip_manager.hpp>

// create a unit description card when mouse hovers over a unit build button
TipManager::TipManager()
{
	tip_index = 0;
	greenhouse_tip = true;
	hunter_tip = true;
	wall_tip = true;
	tower_tip = true;
}

entt::entity TipCard::createTipCard(int x, int y, std::vector<std::string> text_lines)
{
	auto entity = registry.create();
	// card background
	std::string key = "tip_card_bg";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("UI-texture-0.png"), "ui_transparent");
	}
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = LAYER_MENU;



	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "tip_card";
	ui_element.scale = vec2({ 5.f, 1.6f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = vec2(x, y);

	// title text
	float line_size = 35; // relative to the text size
	float left_margin = 3;
	std::string title_string = "Tutorial tip:";
	auto title_text_scale = 0.4f;
	auto notoRegular = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");

	auto x_offset = (ui_element.scale.x - (title_string.length() * title_text_scale * 27)) / 2;
	float top_margin = 5;

	auto y_title_offset = ui_element.scale.y / 2 - title_text_scale * line_size - top_margin;
	vec2 title_text_position = get_center_text_position(ui_element.scale, ui_element.position, title_text_scale, title_string);

	auto& title = registry.emplace<Text>(entity, Text(title_string, notoRegular, vec2(title_text_position.x, title_text_position.y + y_title_offset)));
	title.scale = title_text_scale;
	title.colour = { 0.f, 0.f, 0.f };

	// description text
	auto des_text_scale = 0.35f;
	std::vector<std::string> description_strings = text_lines; // todo
	float des_x_positin = ui_element.position.x - (ui_element.scale.x / 2) + left_margin;
	float des_y_position = WINDOW_SIZE_IN_PX.y - ui_element.position.y + ui_element.scale.y / 2 - 50;
	float des_line_size = des_text_scale * 55;
	int line_counter = 0;
	for (std::string line : description_strings)
	{
		auto des_entity = registry.create();
		vec2 description_text_position = vec2(des_x_positin, des_y_position - des_line_size * line_counter);
		auto& des_text = registry.emplace<Text>(des_entity, Text(line, notoRegular, description_text_position));
		des_text.scale = des_text_scale;
		des_text.colour = { 0.f, 0.f, 0.f };
		registry.emplace<TipCard>(des_entity);
		line_counter++;
	}

	registry.emplace<TipCard>(entity);
	return entity;
}