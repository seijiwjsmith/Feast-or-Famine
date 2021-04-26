#include <ui_description.hpp>
#include <iostream>

entt::entity UI_description_card::create_UI_description_card(entt::entity button_entity)
{

	auto entity = registry.create();
	// remove tooltip box for sandbox buttons 
	if (registry.has<UI_sandbox_button>(button_entity))
	{
		return entity;
	}
	// card background
	std::string key = "unit_description_card_bg";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("UI-texture-0.png"), "ui_transparent");
	}
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = LAYER_UI + 1;

	auto button_ui_element = registry.get<UI_element>(button_entity);
	int ui_bar_gap = 10;
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = button_ui_element.tag + "_description_card";
	if (registry.has<UI_season_wheel>(button_entity))
	{
		ui_element.scale = vec2({ 1.7f, 0.3f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
		ui_element.position = vec2(button_ui_element.position.x, button_ui_element.position.y + button_ui_element.scale.y / 2 + ui_element.scale.y / 2 + ui_bar_gap);
	}
	else
	{
		ui_element.scale = vec2({ 4.f, 1.6f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
		ui_element.position = vec2(button_ui_element.position.x, button_ui_element.position.y - button_ui_element.scale.y / 2 - ui_element.scale.y / 2 - ui_bar_gap);
	}
	

	std::string title_string;
	std::vector<std::string> description_strings;
	if (registry.has<UI_build_unit>(button_entity))
	{
		auto ui_build_unit = registry.get<UI_build_unit>(button_entity);
		title_string = ui_build_unit.unit_name;
		description_strings = ui_build_unit.descriptions;
	}
	else if (registry.has<UI_selected_unit>(button_entity))
	{
		auto selected_element = registry.get<UI_selected_unit>(button_entity);
		std::string key_string = button_ui_element.tag + "_" + selected_element.unit_type + "_" + std::to_string(selected_element.path_num);
		title_string = selected_upgrade_title.at(key_string);
		description_strings = selected_upgrade_descriptions.at(key_string);
	}
	else if (registry.has<UI_weather_icon>(button_entity))
	{
		//TODO
	}
	else if (registry.has<UI_season_wheel>(button_entity))
	{
		title_string = season_description.at(registry.get<UI_season_wheel>(button_entity).season);
	}
	/*else if (registry.has<Path>(button_entity))
	{
		auto monster_type = registry.get<Path>(button_entity).monster_type;
		title_string = selected_upgrade_title.at(key_string);
		description_strings = selected_upgrade_descriptions.at(key_string);
	}*/
	
	// text
	float line_size = 35; // reletive to the text size
	float left_margin = 3;
	// unit name text
	auto title_text_scale = 0.4f;
	auto notoRegular = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
	// center text
	auto x_offset = (ui_element.scale.x - (title_string.length() * title_text_scale * 27)) / 2;
	// place title text at the top
	float top_margin = 5;
	auto y_title_offset = ui_element.scale.y / 2 - title_text_scale * line_size - top_margin;
	vec2 title_text_position = get_center_text_position(ui_element.scale, ui_element.position, title_text_scale, title_string);
	auto& title = registry.emplace<Text>(entity, Text(title_string, notoRegular, vec2(title_text_position.x, title_text_position.y + y_title_offset)));
	title.scale = title_text_scale;
	title.colour = { 0.f, 0.f, 0.f };

	// description text

	auto des_text_scale = 0.35f;
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
		registry.emplace<UI_description_card>(des_entity);
		line_counter++;
	}


	registry.emplace<UI_description_card>(entity);
	return entity;
}

// create a unit description card when mouse hovers over a unit build button
//entt::entity UI_unit_description_card::createUI_unit_description_card(entt::entity button_entity)
//{
//	auto entity = registry.create();
//	// card background
//	std::string key = "unit_description_card_bg";
//	ShadedMesh& resource = cache_resource(key);
//	if (resource.effect.program.resource == 0) {
//		resource = ShadedMesh();
//		RenderSystem::createSprite(resource, ui_texture_path("UI-texture-0.png"), "ui_transparent");
//	}
//	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
//	shaded_mesh.layer = 98;
//
//	auto button_ui_element = registry.get<UI_element>(button_entity);
//	int ui_bar_gap = 10;
//	UI_element& ui_element = registry.emplace<UI_element>(entity);
//	ui_element.tag = button_ui_element.tag + "_description_card";
//	ui_element.scale = vec2({ 4.f, 1.6f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
//	ui_element.position = vec2(button_ui_element.position.x, button_ui_element.position.y - button_ui_element.scale.y / 2 - ui_element.scale.y / 2 - ui_bar_gap);
//
//	auto ui_build_unit = registry.get<UI_build_unit>(button_entity);
//	// text
//	float line_size = 35; // reletive to the text size
//	float left_margin = 3;
//	// unit name text
//	std::string title_string = ui_build_unit.unit_name;
//	auto title_text_scale = 0.4f;
//	auto notoRegular = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
//	// center text
//	auto x_offset = (ui_element.scale.x - (title_string.length() * title_text_scale * 27)) / 2;
//	// place title text at the top
//	float top_margin = 5;
//	auto y_title_offset = ui_element.scale.y / 2 - title_text_scale * line_size - top_margin;
//	vec2 title_text_position = get_center_text_position(ui_element.scale, ui_element.position, title_text_scale, title_string);
//	auto& title = registry.emplace<Text>(entity, Text(title_string, notoRegular, vec2(title_text_position.x, title_text_position.y + y_title_offset)));
//	title.scale = title_text_scale;
//	title.colour = { 0.f, 0.f, 0.f };
//
//	// description text
//
//	auto des_text_scale = 0.35f;
//	std::vector<std::string> description_strings = ui_build_unit.descriptions;
//	float des_x_positin = ui_element.position.x - (ui_element.scale.x / 2) + left_margin;
//	float des_y_position = WINDOW_SIZE_IN_PX.y - ui_element.position.y + ui_element.scale.y / 2 - 50;
//	float des_line_size = des_text_scale * 55;
//	int line_counter = 0;
//	for (std::string line : description_strings)
//	{
//		auto des_entity = registry.create();
//		vec2 description_text_position = vec2(des_x_positin, des_y_position - des_line_size * line_counter);
//		auto& des_text = registry.emplace<Text>(des_entity, Text(line, notoRegular, description_text_position));
//		des_text.scale = des_text_scale;
//		des_text.colour = { 0.f, 0.f, 0.f };
//		registry.emplace<UI_unit_description_card>(des_entity);
//		line_counter++;
//	}
//
//
//	registry.emplace<UI_unit_description_card>(entity);
//	return entity;
//}
//
//entt::entity UI_selected_description_card::createUI_selected_description_card(entt::entity button_entity)
//{
//	auto entity = registry.create();
//	// card background
//	std::string key = "unit_description_card_bg";
//	ShadedMesh& resource = cache_resource(key);
//	if (resource.effect.program.resource == 0) {
//		resource = ShadedMesh();
//		RenderSystem::createSprite(resource, ui_texture_path("UI-texture-0.png"), "ui_transparent");
//	}
//	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
//	shaded_mesh.layer = 98;
//
//	auto button_ui_element = registry.get<UI_element>(button_entity);
//	int ui_bar_gap = 10;
//	UI_element& ui_element = registry.emplace<UI_element>(entity);
//	ui_element.tag = button_ui_element.tag + "_description_card";
//	ui_element.scale = vec2({ 4.f, 1.6f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
//	ui_element.position = vec2(button_ui_element.position.x, button_ui_element.position.y - button_ui_element.scale.y / 2 - ui_element.scale.y / 2 - ui_bar_gap);
//
//	auto& selected_element = registry.get<UI_selected_unit>(button_entity);
//	// text
//	float line_size = 35; // reletive to the text size
//	float left_margin = 3;
//	// unit name text
//	std::string key_string = button_ui_element.tag + "_" + selected_element.unit_type + "_" + std::to_string(selected_element.path_num);
//	std::string title_string = selected_upgrade_title.at(key_string);
//	auto title_text_scale = 0.4f;
//	auto notoRegular = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
//	// center text
//	auto x_offset = (ui_element.scale.x - (title_string.length() * title_text_scale * 27)) / 2;
//	// place title text at the top
//	float top_margin = 5;
//	auto y_title_offset = ui_element.scale.y / 2 - title_text_scale * line_size - top_margin;
//	vec2 title_text_position = get_center_text_position(ui_element.scale, ui_element.position, title_text_scale, title_string);
//	auto& title = registry.emplace<Text>(entity, Text(title_string, notoRegular, vec2(title_text_position.x, title_text_position.y + y_title_offset)));
//	title.scale = title_text_scale;
//	title.colour = { 0.f, 0.f, 0.f };
//
//	// description text
//
//	auto des_text_scale = 0.35f;
//	std::vector<std::string> description_strings = selected_upgrade_descriptions.at(key_string);
//	float des_x_positin = ui_element.position.x - (ui_element.scale.x / 2) + left_margin;
//	float des_y_position = WINDOW_SIZE_IN_PX.y - ui_element.position.y + ui_element.scale.y / 2 - 50;
//	float des_line_size = des_text_scale * 55;
//	int line_counter = 0;
//	for (std::string line : description_strings)
//	{
//		auto des_entity = registry.create();
//		vec2 description_text_position = vec2(des_x_positin, des_y_position - des_line_size * line_counter);
//		auto& des_text = registry.emplace<Text>(des_entity, Text(line, notoRegular, description_text_position));
//		des_text.scale = des_text_scale;
//		des_text.colour = { 0.f, 0.f, 0.f };
//		registry.emplace<UI_unit_description_card>(des_entity);
//		line_counter++;
//	}
//
//
//	registry.emplace<UI_selected_description_card>(entity);
//	return entity;
//}