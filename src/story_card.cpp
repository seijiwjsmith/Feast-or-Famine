#include "story_card.hpp"
#include "render.hpp"
#include <iostream>

const int KERNING = 17;
const std::string DELIMITER = "@";
const int LINE_SPACING = 40;
const int STORY_TEXT_X_OFFSET = -400;
const int STORY_TEXT_Y_OFFSET = 100;

StoryCard::StoryCard(std::string story_card_text, int round_number)
{
	ivec2 pos = ivec2(MAP_SIZE_IN_PX.x - 530, MAP_SIZE_IN_PX.y + UI_TOP_BAR_HEIGHT - 300);
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "story_card";
	ShadedMesh& resource = cache_resource(key);
	resource = ShadedMesh();
	RenderSystem::createSprite(resource, data_path() + "/textures/story/story_text_box.png", "textured");

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = LAYER_MENU;

	// Setting initial motion values
	Motion& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = vec2(0, 0);
	motion.scale = vec2(850.f, 300.f);
	motion.position = pos;

	// text
	auto font = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
	// Level number
	auto level_text_entity = registry.create();
	auto level_text_scale = 0.9f;
	auto level_x_offset = -370;
	auto level_y_offset = 165 - UI_TOP_BAR_HEIGHT;
	std::string speaker_name = "Villager Timmie";
	if (round_talky_boi_sprite.at(round_number) == DAVE_SPRITE) {
		speaker_name = "Villager Davie";
	}

	auto& t_level = registry.emplace<Text>(level_text_entity, Text(speaker_name, font, vec2(pos.x + level_x_offset, pos.y + level_y_offset)));
	t_level.scale = level_text_scale;
	t_level.colour = { 1.0f, 1.0f, 1.0f };
	registry.emplace<StoryCardText>(level_text_entity);

	StoryCardBase& base = registry.emplace<StoryCardBase>(entity);
	base.story_text_scale = .6f;
	base.line_offset = pos.y + 100 - UI_TOP_BAR_HEIGHT;
	base.x_offset = pos.x + level_x_offset;
	base.font = font;
	base.card_text = story_card_text;
}

void StoryCardBase::write_character()
{
	if (card_text.length() == 0) {
		return;
	}
	if (card_text.at(0) == '@') {
		line_offset -= LINE_SPACING;
		char_offset = 0;
	}
	else {
		auto story_text_entity = registry.create();
		auto& t_story = registry.emplace<Text>(story_text_entity, Text(std::string(1, card_text.at(0)), font, vec2(x_offset + char_offset, line_offset)));
		t_story.scale = story_text_scale;
		t_story.colour = { 1.0f, 1.0f, 1.0f };
		registry.emplace<StoryCardText>(story_text_entity);

		char_offset += KERNING;
	}

	card_text.erase(0, 1);
}
