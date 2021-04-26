#pragma once
// Created by Ray 

#include "entt.hpp"
#include "common.hpp"
#include "ui.hpp"

enum Menu_texture { title_screen, controls, default_menu, help_menu, pause_menu, lost_game,
	title_screen_title, title_screen_title2, title_screen_title2_or, foreground_trees, sign_post, victory_background};

struct Menu
{
	std::vector<entt::entity> buttons = {};
	static entt::entity createMenu(float x, float y, std::string menu_name, Menu_texture texture, int layer = LAYER_MENU, vec2 scale = { 1.f, 1.f });
	static entt::entity createMenu(float x, float y, std::string menu_name, std::string texture_file_name, int layer = LAYER_MENU, vec2 scale = { 1.f, 1.f });
	static entt::entity createLostMenu();
	static entt::entity createVictoryScreen();
};

inline std::string menu_texture_path(const std::string& name) { return data_path() + "/textures/menu/" + name; };

struct MenuText
{
	std::string menu_name;
};

struct TitleEyes
{
	bool show;
	float blink_delay_ms;
	float blink_time_ms;
	static entt::entity createTitleEyes(vec2 position);
};