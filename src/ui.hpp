#pragma once


#include "entt.hpp"
#include "common.hpp"
#include "render.hpp"
#include "units/unit.hpp"
#include "text.hpp"
#include <vector>
#include <string>
#include <json.hpp>

const float BUTTON_GAP = 13.f;

std::string button_to_string(int button); // breaks if enums change
Button UI_click_system();
void UI_highlight_system(vec2 mouse_pos);

entt::entity create_ui_text(vec2 position, std::string content, float scale = 0.3f, vec3 colour = { 1.0f,1.0f,1.0f });

struct UI_element {
	std::string tag;
	vec2 position;
	vec2 scale;
	float angle = 0;
};

struct UI_background
{
	static entt::entity createUI_background();
	static entt::entity createUI_top_bar();
};

struct UI_button
{
	static void fastforward_light_up();
	static void fastforward_light_off();
	static void wantedboard_update_on(entt::entity button);
	static void wantedboard_update_off(entt::entity button);

	static entt::entity createTips_button(vec2 pos);
	static entt::entity createStart_button(vec2 pos);
	static entt::entity createPause_button(vec2 pos);
	static entt::entity createMore_button(vec2 pos);
	static entt::entity create_add_monster_button(vec2 pos);
	static entt::entity create_rem_monster_button(vec2 pos);
	static entt::entity create_inc_m_speed_button(vec2 pos);
	static entt::entity create_dec_m_speed_button(vec2 pos);
	static entt::entity randomize_grid_map_button(vec2 pos);
	static entt::entity createFastforward_button(vec2 pos);
	static entt::entity createWantedBoard_button(vec2 pos);
	static entt::entity createUI_build_unit_button(int pos, Button button, size_t cost = 0, std::string tag = "default_ui_button_tag", bool show = true);
	static entt::entity createUI_button(int pos, Button button, std::string tag = "default_ui_button_tag", bool show = true);
	// wrapper for buttons that shows up when a unit is selected
	static entt::entity createUI_selected_unit_button(int pos, Button button, std::string tag = "default_ui_button_tag", bool show = true);
};


struct UI_sandbox_button {};

struct UI_banner
{
	static entt::entity createUI_Banner(std::string content, vec2 position, vec3 colour, float duration);
};

struct UI_selected_unit_portrait
{
	unit_type type;
	static entt::entity createUI_selected_unit_portrait(unit_type type);
};

inline std::string ui_texture_path(const std::string& name) { return data_path() + "/textures/ui/" + name; };
inline std::string ui_upgrade_texture_path(const std::string& name) { return data_path() + "/textures/ui/upgrade_ui/" + name; };
inline std::string units_texture_path(const std::string& name) { return data_path() + "/textures/units/" + name; };

void change_button_text(entt::entity button_entity, std::string button_text);

void aligne_text_right(entt::entity entity, float right_alignment_position);

struct UI_season_wheel {
	static void set_arrow(entt::entity season_wheel_arrow_entity, int season, int round);
	static void increment_arrow(entt::entity season_wheel_arrow_entity);
	static entt::entity createUI_season_wheel();
	static entt::entity createUI_season_wheel_arrow();
	static void get_season_sequence();
	int season;
	float reward_multiplier;
};

struct UI_weather_icon {
	static entt::entity createUI_weather_icon();
	static void change_weather_icon(entt::entity entity, int weather);
	int weather;
};

struct UI_selected_unit
{
	std::vector<entt::entity> button_components;
	int path_num; 
	std::string unit_type; 
	static entt::entity createUI_selected_unit_upgrade_button(int pos, Button button, std::string tag, int unit_type, int path_num, bool show = true);
	static entt::entity create_selected_button_image(vec2 pos, std::string tag, Unit unit);
	static entt::entity create_selected_button_progress_bar(vec2 pos, int path_num);
};

struct UI_sell_button
{
	static entt::entity createUI_sell_button(int pos, Button button, std::string tag = "default_ui_button_tag", bool show = true);
	static entt::entity create_sell_button_icon(vec2 pos);
	entt::entity sell_text;
};

struct UI_build_unit
{
	static void fill_UI_build_unit_component(UI_build_unit& ui_build_unit, Button button);
	std::string unit_name = "";
	std::vector<std::string> descriptions = {};
};

struct UI_unit_stats
{

};
