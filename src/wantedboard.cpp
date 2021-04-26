// Header
#include "paths.hpp"
#include "entt.hpp"
#include "render.hpp"

#include <cmath>
#include <iostream>
#include <wantedboard.hpp>
#include <ui.hpp>

entt::entity WantedBoard::createWantedBoard() {
	auto entity = registry.create();
	// card background
	std::string key = "wanted_board";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, wanted_board_texture_path("wanted_board.png"), "textured");
	}

	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = LAYER_UI + 1;
	shaded_mesh.show = false;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "wanted_board";
	ui_element.scale = static_cast<vec2>(resource.texture.size);
	ui_element.position = vec2(WINDOW_SIZE_IN_PX.x /2 , WINDOW_SIZE_IN_PX.y / 2 - 5);
	
	auto& board = registry.emplace<WantedBoard>(entity);

	board.wanted_title = WantedTitle::createWantedSign();

	return entity;
}

entt::entity WantedTitle::createWantedSign() {
	auto entity = registry.create();
	// card background
	std::string key = "wanted_sign";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, wanted_board_texture_path("wanted_title.png"), "textured");
	}

	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = LAYER_MENU;
	shaded_mesh.show = false;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "wanted_sign";
	ui_element.scale = static_cast<vec2>(resource.texture.size);
	ui_element.position = vec2(WINDOW_SIZE_IN_PX.x / 2, 1.5*ui_element.scale.y + UI_TOP_BAR_HEIGHT);
	return entity;
}

void WantedBoard::updateWantedBoardDisplay(entt::entity wanted_board_entity, bool show) {
	auto& board_meshref = registry.get<ShadedMeshRef>(wanted_board_entity);
	board_meshref.show = show;

	auto& wantedboard = registry.get<WantedBoard>(wanted_board_entity);
	auto& title_mesh_ref = registry.get<ShadedMeshRef>(wantedboard.wanted_title);
	title_mesh_ref.show = show;

	for (auto entity : wantedboard.wanted_entries)
	{
		auto& entries_meshref = registry.get<ShadedMeshRef>(entity);
		entries_meshref.show = show;

		auto& wanted_entries = registry.get<WantedEntry>(entity);
		for (auto info_entity : wanted_entries.monster_info) {
			if (registry.has<ShadedMeshRef>(info_entity)) {
				ShadedMeshRef& shaded_mesh_ref = registry.view<ShadedMeshRef>().get<ShadedMeshRef>(info_entity);
				shaded_mesh_ref.show = show;
			}
			else if (registry.has<Text>(info_entity)) {
				registry.get<Text>(info_entity).show = show;
			}
		}
	}
}

void WantedBoard::updateWantedEntries(entt::entity wanted_board, std::vector<int> current_round_monster_types, int round_number, float reward_multiplier)
{
	auto& board = registry.get<WantedBoard>(wanted_board);
	for (auto entry : board.wanted_entries) {
		auto& wantedentry = registry.get<WantedEntry>(entry);
		for (auto info_entity : wantedentry.monster_info)
		{
			registry.destroy(info_entity);
		}
		registry.destroy(entry);
	}

	board.wanted_entries = std::vector<entt::entity>();
	UI_element ui_element = registry.get<UI_element>(wanted_board);
	
	if (round_number == 16) {
		vec2 position = vec2(ui_element.position.x - ui_element.scale.x / 4.f, ui_element.position.y + 50.f);
		board.wanted_entries.push_back(WantedEntry::createWantedEntry(position, FINAL_BOSS, round_number, reward_multiplier));
	}
	else {

		for (int i = 0; i < current_round_monster_types.size(); i++)
		{
			vec2 position;
			if (i % 2 == 0)
			{
				position = vec2(ui_element.position.x - ui_element.scale.x / 4.f, ui_element.position.y + 50.f);
			}
			else
			{
				position = vec2(ui_element.position.x + ui_element.scale.x / 4.f, ui_element.position.y + 50.f);
			}
			board.wanted_entries.push_back(WantedEntry::createWantedEntry(position, current_round_monster_types[i], round_number, reward_multiplier));
		}
	}
}

entt::entity WantedEntry::createWantedEntry(vec2 position, int monster_type, int round_number, float reward_multiplier)
{
	auto entity = registry.create();
	// card background
	std::string key = "wanted_entry";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, wanted_board_texture_path("wanted_entry.png"), "textured");
	}

	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = LAYER_MENU;
	shaded_mesh.show = false;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "wanted_entry";
	ui_element.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 5.5, 1.0);
	ui_element.position = position;
	registry.emplace<WantedEntry>(entity);

	createMonsterIcon(entity, ui_element.position, ui_element.scale, monster_type);
	createMonsterText(entity, ui_element.position, ui_element.scale, monster_type, round_number, reward_multiplier);

	return entity;
}

void WantedEntry::createMonsterIcon(entt::entity entry, vec2 position, vec2 scale, int monster_type)
{
	auto entity = registry.create();
	// card background
	std::string key = "icon" + std::to_string(monster_type);
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		switch (monster_type) {
		case MOB:
			RenderSystem::createSprite(resource, wanted_board_texture_path("mob_icon.png"), "textured");
			break;
		case SPRING_BOSS:
			RenderSystem::createSprite(resource, wanted_board_texture_path("spring_boss_icon.png"), "textured");
			break;
		case SUMMER_BOSS:
			RenderSystem::createSprite(resource, wanted_board_texture_path("summer_boss_icon.png"), "textured");
			break;
		case FALL_BOSS:
			RenderSystem::createSprite(resource, wanted_board_texture_path("fall_boss_icon.png"), "textured");
			break;
		case WINTER_BOSS:
			RenderSystem::createSprite(resource, wanted_board_texture_path("winter_boss_icon.png"), "textured");
			break;
		case FINAL_BOSS:
			RenderSystem::createSprite(resource, wanted_board_texture_path("dragon_icon.png"), "textured");
			break;
		default:
			RenderSystem::createSprite(resource, wanted_board_texture_path("mob_icon.png"), "textured");
			break;
		}
	}

	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = LAYER_MENU + 1;
	shaded_mesh.show = false;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "wanted_entry_icon";
	ui_element.scale = static_cast<vec2>(resource.texture.size);
	ui_element.position = vec2(position.x, position.y - scale.y / 4);
	
	registry.get<WantedEntry>(entry).monster_info.push_back(entity);
}

void WantedEntry::createMonsterText(entt::entity entry, vec2 position, vec2 scale, int monster_type, int round_number, float reward_multiplier)
{
	auto& wantedEntry = registry.get<WantedEntry>(entry);
	
	if (monster_type == FINAL_BOSS) {
		auto Marlboro = TextFont::load("data/fonts/Marlboro/Marlboro.ttf");

		auto des_text_scale = 0.65f;
		float des_x_position = position.x;
		float des_y_position = position.y - scale.y / 5;
		float des_line_size = des_text_scale * 55;
		int line_counter = 0;
		for (std::string line : dragon_text)
		{
			auto des_entity = registry.create();
			float center_x_offset = line.length() * 12 / 2;
			vec2 description_text_position = vec2(des_x_position - center_x_offset, des_y_position - des_line_size * line_counter);
			auto& des_text = registry.emplace<Text>(des_entity, Text(line, Marlboro, description_text_position));
			des_text.scale = des_text_scale;
			des_text.show = false;

			wantedEntry.monster_info.push_back(des_entity);
			line_counter++;
		}
	}
	else {
		std::vector<std::string> description_strings;
		MonsterInfo info = wanted_descriptions.at(monster_type);

		if (monster_type == MOB) info.health += sum_to_n(round_number) * MONSTER_SCALE_HEALTH_FACTOR;
		else info.health += sum_to_n(round_number) * BOSS_SCALE_HEALTH_FACTOR;

		description_strings.push_back(template_text[0] + std::to_string(info.health));
		description_strings.push_back(template_text[1] + std::to_string(info.speed));
		description_strings.push_back(template_text[2] + std::to_string(info.damage));
		description_strings.push_back(template_text[3] + info.path_color);
		description_strings.push_back(template_text[4] + std::to_string((int)(info.reward * reward_multiplier)));

		auto Marlboro = TextFont::load("data/fonts/Marlboro/Marlboro.ttf");

		auto des_text_scale = 0.65f;
		float des_x_position = position.x;
		float des_y_position = position.y - scale.y / 5;
		float des_line_size = des_text_scale * 55;
		int line_counter = 0;
		for (std::string line : description_strings)
		{
			auto des_entity = registry.create();
			float center_x_offset = line.length() * 12 / 2;
			vec2 description_text_position = vec2(des_x_position - center_x_offset, des_y_position - des_line_size * line_counter);
			auto& des_text = registry.emplace<Text>(des_entity, Text(line, Marlboro, description_text_position));
			des_text.scale = des_text_scale;
			if (line_counter == 3) {
				des_text.colour = Path::getPathColor(monster_type);
			}
			else {
				des_text.colour = { 0.f, 0.f, 0.f };
			}
			des_text.show = false;

			wantedEntry.monster_info.push_back(des_entity);
			line_counter++;
		}
	}
}

