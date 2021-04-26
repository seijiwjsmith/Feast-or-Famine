#pragma once

// internal
#include "common.hpp"
#include "grid_map.hpp"
#include "Observer.hpp"
#include "physics.hpp"
#include <BehaviorTree.hpp>
#include "text.hpp"
#include "units/unit.hpp"
#include "ui_description.hpp"
#include "tip_manager.hpp"
#include "story_card.hpp"
#include "health_orb.hpp"
// stlib
#include <vector>


#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <SDL_mixer.h>
#include <../ext/nlohmann/json.hpp>
#include "stb_image.h"
// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem : public Observer
{
public:
	// Creates a window
	WorldSystem(ivec2 window_size_px, PhysicsSystem* physics);

	// Releases all associated resources
	~WorldSystem();

	ShadedMesh* screen_sprite;

	//manage dragon animations
	void manage_dragon_animations();
	void create_fireball();
	// menu
	void game_setup();
	void create_start_menu();
	void setup_start_menu();
	void create_sandbox_ui();

	// restart level
	void restart();
	void restart_with_save();

	// start round
	void start_round();

	// helper to load game from save game path
	void load_game();

	void pause_game();

	void more_options_menu();

	void resume_game();

	// helper to save game to disk
	void save_game();

	void upgrade_unit(Unit& unit);

	void sell_unit(entt::entity& entity);

	// setup game setup stage
	void prepare_setup_stage();

	// helper for path to round jsons
	void setup_round_from_round_number(int round_number);

	void setup_round_from_save_file(int round_number, int weather);

	// Check for collisions
	void updateProjectileMonsterCollision(entt::entity e_projectile, entt::entity e_monster);

	// animates the portrait for the story card
	void animate_speaker(float elapsed_ms);

	// Steps the game during monster rounds ahead by ms milliseconds
	void step(float elapsed_ms);

	// Steps the game during set up rounds
	void set_up_step(float elapsed_ms);
	
	// steps the game during title screen
	void title_screen_step(float elapsed_ms);

	// lost game 
	void darken_screen_step(float elapsed_ms);
	void animation_step(float elapsed_ms);

	// End of battle phase
	void end_battle_phase();

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	static void deduct_health(int num);
	static void add_health(int num);
	// OpenGL window handle
	GLFWwindow* window;

	// game state
	int game_state;
	bool game_tips;
	bool monster_tips;

	// Particle System
//    GLuint billboard_vertex_buffer;
//    GLuint particles_position_buffer;

	// Menu
	enum GameState
	{
		start_menu,
		in_game,
		settings_menu,
		help_menu,
		story_card,
		paused,
		darken_screen,
		lost_game_screen,
		victory_screen,
	};

	// state for set_up and monster_rounds
	int player_state;
	enum PlayerState
	{
		set_up_stage,
		battle_stage,
		story_stage
	};

	// health of the village
	static int health;
	static GridMap current_map;

	// tutorial tip manager
	TipManager tip_manager;

	// decrease reward at higher levels
	static float reward_multiplier;
	static int world_round_number;

	// speed up factor for fastforwarding time
	static float speed_up_factor;

	// is victorious
	bool victory = false;

    static void set_default_paths();
	// music to play
	MusicState music_state;

// flag for AI paths
static bool set_AI_paths;
private:
	// PhysicsSystem handle
	PhysicsSystem* physics;

	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 mouse_pos);
	void on_mouse_click(int button, int action, int mod);
	void scroll_callback(double xoffset, double yoffset);

	// Loads the audio
	void init_audio();

	// remove game tip
	void remove_game_tip_and_story_card();

	void handle_game_tips();

	// village
	entt::entity village;

	entt::entity egg;
    bool round_skipped = false;

	// animation fps
	float fps_ms;

	// Game state
	float current_speed;
	float next_boss_spawn;
	int next_fireball_spawn;
	float next_mob_spawn;
	float next_particle_spawn;

    static float show_path_duration_ms;
    static float show_path;

	int mob_delay_ms;
	int max_mobs;
	int boss_delay_ms;
	int max_boss;
	int fireball_delay_ms;
	// end of battle stage delay 
	float end_of_battle_stage_dealy_ms = END_OF_BATTLE_STAGE_DELAY_MS;
	// greenhouse food already increased
	bool greenhouse_food_increased = false;

	int num_mobs_spawned;
	int num_bosses_spawned;
	entt::entity(*create_boss)(int round_number);

	std::shared_ptr<BTNode> BTCollision;

	// flag for selected view bottom ui
	bool selected_view_change;
	entt::entity previous_selected;
	entt::entity upgrade_button_1;
	entt::entity upgrade_button_2;
	entt::entity button_sell;
	entt::entity wanted_board_button;
	// true if a unit is selected on the field
	bool unit_selected = false;

	//UI
	entt::entity round_text_entity;
	entt::entity food_text_entity;
	entt::entity stage_text_entity;
	entt::entity season_text_entity;
	entt::entity weather_text_entity;
	entt::entity season_wheel_arrow_entity;
	entt::entity weather_icon_entity;
	entt::entity pause_menu_entity;
	entt::entity help_menu_entity;
	entt::entity title_button_highlight_entity;
	entt::entity wanted_board_entity;
	entt::entity max_mobs_text_entity;
	entt::entity mob_speed_text_entity;

	unit_type placement_unit_selected;
	entt::entity entity_selected;
	entt::entity entity_range_circle;
	entt::entity selected_range_circle;
	entt::entity(*create_unit_indicator)(vec2 pos);

	// remove entities from start menu
	void remove_menu_buttons();
	void create_controls_menu();
	entt::entity create_help_menu();

	//helper methods for updateProjectileMonsterCollision() method
	void damage_monster_helper(entt::entity e_monster, entt::entity e_projectile, int damage, bool slow = false);

	// helper for start menu mouse click and in_game mouse click
	void start_menu_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void in_game_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod);
	void paused_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod);
	void settings_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod);
	vec2 on_click_select_unit(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	bool click_on_unit(double mouse_pos_x, double mouse_pos_y);
	void help_menu_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void story_card_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void update_look_for_selected_buttons(int action, bool sell_clicked);
	void lost_game_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void victory_screen_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void createEntityRangeIndicator(vec2 mouse_pos);
	void on_click_ui(Button ui_button);
	void on_click_ui_when_selected(Button ui_button);
	void on_click_ui_general_buttons(Button ui_button);
	// lost game
	void start_lost_game_screen();
	// victory
	void start_victory_screen();
	// end of battle phase
	void end_battle_phase_step(float elapsed_ms);

	
	// music references
	Mix_Music* background_music;
	Mix_Chunk* salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;
	Mix_Chunk* ui_sound_bottle_pop;
	Mix_Chunk* ui_sound_tick;
	Mix_Chunk* ui_sound_negative_tick;
	Mix_Chunk* ui_sound_hollow_tick;
	Mix_Chunk* snowball_hit_sound;

    void set_random_weather();
    void set_round_monsters();
    void update_weather_season_UI(int round_number) const;

    void skip_to_final_round();
	// performance stats
	std::map<monster_type, int> player_stats = {
		{monster_type::MOB, 0},
		{monster_type::SPRING_BOSS, 0},
		{monster_type::SUMMER_BOSS, 0},
		{monster_type::FALL_BOSS, 0},
		{monster_type::WINTER_BOSS, 0}
	};

	void update_player_stats_monster(entt::entity monster);
};
