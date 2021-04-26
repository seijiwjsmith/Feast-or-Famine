// Header
#include "world.hpp"
#include "physics.hpp"
#include "debug.hpp"
#include "render_components.hpp"
#include "monsters/spring_boss.hpp"
#include "monsters/fall_boss.hpp"
#include "monsters/summer_boss.hpp"
#include "monsters/winter_boss.hpp"
#include "monsters/burrow_boss.hpp"
#include "monsters/final_boss.hpp"
#include "monsters/fireball_boss.hpp"
#include "monsters/mob.hpp"
#include "monsters/ghost.hpp"
#include <projectile.hpp>

#include "grid_map.hpp"
#include "units/hunter.hpp"
#include "units/greenhouse.hpp"
#include "units/watchtower.hpp"
#include "units/village.hpp"
#include "units/wall.hpp"
#include "units/unit.hpp"
#include "camera.hpp"
#include "button.hpp"
#include "menu.hpp"
#include "story_card.hpp"
#include "ui.hpp"
#include "ai.hpp"
#include "particle.hpp"
#include "talky_boi.hpp"

#include "rig.hpp"
#include "monsters/dragon_rig.hpp"
#include "monsters/spider.hpp"
#include <BehaviorTree.hpp>

// stlib
#include <glm/gtc/integer.hpp>
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <units/forest.hpp>

#include "json.hpp"
#include <units/exterminator.hpp>
#include <units/robot.hpp>
#include <units/priestess.hpp>
#include <units/snowmachine.hpp>
#include <units/rangecircle.hpp>
#include <units/aura.hpp>
#include <paths.hpp>
#include <wantedboard.hpp>
#include <units/egg.hpp>

const size_t ANIMATION_FPS = 20;
const size_t MENU_ANIMATION_FPS = 16;
const size_t SPEAKER_FPS_MS = 60;
const size_t TEXT_APPEAR_SPEED = 1; // lower is faster
const int STARTING_HEALTH = 1000;

int WorldSystem::health = 1000;
int WorldSystem::world_round_number = 0;
float WorldSystem::speed_up_factor = 1.f;
float WorldSystem::reward_multiplier = 1.f;
bool WorldSystem::set_AI_paths = false;
float WorldSystem::show_path_duration_ms = 1000.f;
float WorldSystem::show_path = 0.f;
GridMap WorldSystem::current_map;
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.

std::map<int, std::vector<ivec2>> default_monster_paths;
std::vector<int> current_round_monster_types;
const std::string NEW_GAME = "new_game";
const std::string SAVE_GAME = "save_game";
const std::string LOAD_GAME = "load_game";
const std::string HELP_MENU = "help_menu";
const std::string SETTINGS_MENU = "settings_menu";
const std::string EXIT = "exit";
const std::string PATH_1_UPGRADE_BUTTON_TITLE = "path_1_upgrade_button";
const std::string PATH_2_UPGRADE_BUTTON_TITLE = "path_2_upgrade_button";
const std::string SELL_BUTTON_TITLE = "sell_button";
const std::string START_BUTTON_TITLE = "start_button";

const std::string TIPS_BUTTON_TITLE = "tips_button";
const std::string SAVE_PATH = "data/save_files/save_state.json";

WorldSystem::WorldSystem(ivec2 window_size_px, PhysicsSystem *physics) : game_state(start_menu),
    player_state(set_up_stage),
    fps_ms(0),
    next_boss_spawn(0),
    next_mob_spawn(0),
    num_mobs_spawned(0),
    next_particle_spawn(0),
    num_bosses_spawned(0),
	selected_view_change(true),
	game_tips(true)
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());

	///////////////////////////////////////
	// Initialize GLFW
	auto glfw_err_callback = [](int error, const char *desc) { std::cerr << "OpenGL:" << error << desc << std::endl; };
	glfwSetErrorCallback(glfw_err_callback);
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW");

	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization, needs to be set before glfwCreateWindow
	// Core Opengl 3.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_size_px.x, window_size_px.y, "Feast or Famine", nullptr, nullptr);
	if (window == nullptr)
		throw std::runtime_error("Failed to glfwCreateWindow");

	GLFWimage images[1];
	images[0].pixels = stbi_load("data/textures/icons/ff.png", &images[0].width, &images[0].height, 0, 4);
	glfwSetWindowIcon(this->window, 1, images);
	stbi_image_free(images[0].pixels);

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) { ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) { ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1}); };
	auto mouse_button_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2) { ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	auto scroll_redirect = [](GLFWwindow *wnd, double xoffset, double yoffset) { ((WorldSystem *)glfwGetWindowUserPointer(wnd))->scroll_callback(xoffset, yoffset); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_redirect);
	glfwSetScrollCallback(window, scroll_redirect);

	// Playing background music indefinitely
	init_audio();
	// Mix_PlayMusic(background_music, -1);
	std::cout << "Loaded music\n";

	// Attaching World Observer to Physics observerlist
	this->physics = physics;
	this->physics->attach(this);

    for (int monster_type = 0; monster_type < monster_type_count; monster_type++) {
        default_monster_paths.insert(std::pair<int, std::vector<ivec2>>(monster_type, {}));
    }
	speed_up_factor = 1.f;
}

WorldSystem::~WorldSystem()
{
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (salmon_dead_sound != nullptr)
		Mix_FreeChunk(salmon_dead_sound);
	if (salmon_eat_sound != nullptr)
		Mix_FreeChunk(salmon_eat_sound);

	// Destroy all created components
	//ECS::ContainerInterface::clear_all_components();

	registry.clear(); // this destroys all entities...
	// Close the window
	glfwDestroyWindow(window);
}

void WorldSystem::init_audio()
{
	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		throw std::runtime_error("Failed to initialize SDL Audio");

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
		throw std::runtime_error("Failed to open audio device");

	background_music = Mix_LoadMUS(audio_path("music2.wav").c_str());
	salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
	salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());
	ui_sound_bottle_pop = Mix_LoadWAV(audio_path("bottle_pop.wav").c_str());
	ui_sound_tick = Mix_LoadWAV(audio_path("tick.wav").c_str());
	ui_sound_hollow_tick = Mix_LoadWAV(audio_path("hollow_tick.wav").c_str());
	ui_sound_negative_tick = Mix_LoadWAV(audio_path("negative_tick.wav").c_str());
	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr || ui_sound_bottle_pop == nullptr)
		throw std::runtime_error("Failed to load sounds make sure the data directory is present: " +
								 audio_path("music2.wav") +
								 audio_path("salmon_dead.wav") +
								 audio_path("salmon_eat.wav") +
								 audio_path("ui_sound_bottle_pop.wav"));
}

void WorldSystem::animate_speaker(float elapsed_ms)
{
	fps_ms -= elapsed_ms;
	if (fps_ms < 0.f)
	{
		for (auto entity : registry.view<Animate>())
		{
			auto& animate = registry.get<Animate>(entity);
			animate.frame += 1;
			animate.frame = (int)animate.frame % (int)animate.frame_num;
		}
		fps_ms = SPEAKER_FPS_MS;

	}

	if ((int) fps_ms % TEXT_APPEAR_SPEED == 0)
	{
		for (auto& card : registry.view<StoryCardBase>())
		{
			auto& base = registry.get<StoryCardBase>(card);
			base.write_character();
		}
	}
}


void  WorldSystem::create_fireball() {
	//create fireball
	entt::entity fireball = FireballBoss::createFireballBossEntt();
	registry.emplace<DamageProperties>(fireball);
	auto& monster = registry.get<Monster>(fireball);
	monster.path_coords = default_monster_paths.at(monster.type);
	BTCollision->init(fireball);
}

//manages fireball spawning and dragon animations
// checks timeline + anim_state of dragon to spawn fireballs during it's attack animation
void  WorldSystem::manage_dragon_animations() {

	auto anim_view = registry.view<DragonRig>();
	for (auto entity : anim_view) {

		auto& anim = registry.get<FK_Animations>(entity);
		auto& timeline = registry.get<Timeline>(entity);
		auto& attack_bool = registry.get<Attack_bool>(entity);
		auto& timing = registry.get<Animation_timing>(entity);
		

		/*
		* an amazing state machine
		*/

		if (anim.anim_state == 0 ) {//intro 
			if (timeline.current_time > 2.1f) { // reset timeline to restart animation loop manually.
				anim.next_anim_state = 2; // after this loop, attack loop
				anim.anim_state = anim.next_anim_state; // change what animation to play next
				timeline.current_time = 0.0f;

				//stop the dragon in its tracks!
				auto& motion = registry.get<Motion>(entity);
				motion.velocity = vec2();
			}
		}

		if (anim.anim_state == 1) {//idle
			anim.next_anim_state = 2; // after this loop, attack
		}

		// 2 == attack animation  //check dragon's timeline to see if we need to spawn a fireball
		if (anim.anim_state == 2 ) {
			if (timeline.current_time > timing.loop_end + 1.5f) { // reset timeline to restart animation loop manually.
				anim.anim_state = anim.next_anim_state; // change what animation to play next
				timeline.current_time = 0.0f;
				attack_bool.value = true;
			}
			if (timeline.current_time > timing.t_fireball && attack_bool.value) {
				create_fireball();
				attack_bool.value = false; //set to false to avoid attacking again this loop of the animation
			}
		}

	}
}
// Update our game world
void WorldSystem::step(float elapsed_ms)
{
	if (game_state == in_game) {
        show_path -= elapsed_ms;
	    if (show_path < 0) {
            for (auto entity : registry.view<Path>())
                registry.destroy(entity);
	    }

        for (auto entity : registry.view<Aura>())
        {
            auto& motion = registry.get<Motion>(entity);
            motion.angle += 0.01;
        }

        for (auto entity : registry.view<Ghost>()) {
            auto& ghost = registry.get<Ghost>(entity);
            ghost.lifespan_ms -= elapsed_ms;
            if (ghost.lifespan_ms < 0) {
                registry.destroy(entity);
            }
        }

	    // for when units are destroyed (or placed in survival mode)
	    set_default_paths();

		//rig animation
		auto view_rigs = registry.view<Timeline>();
		for (auto entity : view_rigs) {
			if (registry.has<FK_Animations>(entity)) {
				RigSystem::animate_rig_fk(entity, elapsed_ms);
			}
			else {
				RigSystem::animate_rig_ik(entity, 15); // constant 15ms/frame
			}
		}
		// animation
		fps_ms -= elapsed_ms;
		if (fps_ms < 0.f)
		{
			for (auto entity : registry.view<Animate>())
			{
				auto& animate = registry.get<Animate>(entity);
				animate.next_update -= 1;
				if(animate.next_update <= 0) {
					animate.next_update = animate.update_interval;
					animate.frame += 1;
					animate.frame = (int)animate.frame % (int)animate.frame_num;
				}
			}
			fps_ms = 1000 / ANIMATION_FPS;
		}

		//Spawning new boss
		next_boss_spawn -= elapsed_ms * current_speed;
		if (num_bosses_spawned < max_boss && next_boss_spawn < 0.f)
		{
			// Reset spawn timer and spawn boss
			next_boss_spawn = (boss_delay_ms / 2) + uniform_dist(rng) * (boss_delay_ms / 2);
			entt::entity boss = create_boss(world_round_number);
			
			registry.emplace<DamageProperties>(boss);
			auto& monster = registry.get<Monster>(boss);
			monster.path_coords = default_monster_paths.at(monster.type);

			num_bosses_spawned += 1;
			BTCollision->init(boss);
		}

		// Spawning new mobs
		next_mob_spawn -= (int)(elapsed_ms * current_speed);
		if (num_mobs_spawned < max_mobs && next_mob_spawn < 0.f)
		{
			next_mob_spawn = (mob_delay_ms / 2) + uniform_dist(rng) * (mob_delay_ms / 2);
			entt::entity mob = Mob::createMobEntt(world_round_number);

			registry.emplace<DamageProperties>(mob);
			auto& monster = registry.get<Monster>(mob);
            monster.path_coords = default_monster_paths.at(monster.type);

			num_mobs_spawned += 1;
			BTCollision->init(mob);
		}

		manage_dragon_animations(); //replaces fireball code

		// update velocity for every monster
		for (auto entity : registry.view<Monster>())
		{
			auto& damage = registry.get<DamageProperties>(entity);
			for (auto& [key, value] : damage.dot_map) {
				value -= ELAPSED_MS * WorldSystem::speed_up_factor;
			}

			auto& monster = registry.get<Monster>(entity);
			float num = -1.f;
			float max_slow = 0;
			while (!damage.slow_queue.empty()) {
				std::pair<float, float> pair = damage.slow_queue.top();
				if (num == -1.f) {
					pair.second -= elapsed_ms;
					num = SLOW_DELAY - pair.second;
				}
				else {
					pair.second -= num;
				}

				if (pair.second < 0) {
					damage.slow_queue.pop();
				}
				else {
					damage.slow_queue.pop();
					damage.slow_queue.push(pair);
					max_slow = pair.first;
					break;
				}
			}
			monster.speed_multiplier *= (100 / (100 - damage.current_slow));
			monster.speed_multiplier *= ((100 - max_slow) / 100);
			damage.current_slow = max_slow;

			auto state = BTCollision->process(entity);
			if (state == BTState::Attack) {
				if (registry.valid(previous_selected)) {
					update_look_for_selected_buttons(GLFW_PRESS, false);
				}
				else {
					update_look_for_selected_buttons(GLFW_PRESS, true);
				}
			}


			if (health < 0) {
				start_lost_game_screen();
				return;
			}
		}

		// removes projectiles that are out of the screen
		for (auto projectile : registry.view<Projectile>())
		{
			auto& pos = registry.get<Motion>(projectile);
			if (pos.position.x > WINDOW_SIZE_IN_PX.x + 200 || pos.position.y > WINDOW_SIZE_IN_PX.y + 200 || pos.position.x < -200 ||
				pos.position.y < -200)
			{
				registry.destroy(projectile);
				continue;
			}

			if (registry.has<EntityDeath>(projectile)) {
				auto& death = registry.get<EntityDeath>(projectile);
				death.timer -= elapsed_ms;
				if (death.timer <= 0) {
					registry.destroy(projectile);
					continue;
				}
			}
		}

		auto particle_view = registry.view<ParticleSystem>();
		if (particle_view.size() < MAX_PARTICLES) {
			for (auto particle_entity : particle_view) {
				auto& particle = registry.get<ParticleSystem>(particle_entity);
				particle.life -= elapsed_ms;
				if (particle.life <= 0) {
					registry.destroy(particle_entity);
				}
			}
			ParticleSystem::updateParticle();

			next_particle_spawn -= elapsed_ms;

			if (weather == RAIN && next_particle_spawn < 0.f)
			{
				next_particle_spawn = 60;
				vec2 velocity = { 0.f, 450.0f };
				vec2 position = { rand() % WINDOW_SIZE_IN_PX.x + 1 , 0 };
				float life = 1500.0f;
				std::string texture = "raindrop.png";
				std::string shader = "rain";
				ParticleSystem::createParticle(velocity, position, life, texture, shader);
			}
			else if (weather == DROUGHT) {
				// change screen_sprite water fs and vs to heat fs and vs
				//screen_sprite->effect.load_from_file(shader_path("heat") + ".vs.glsl", shader_path("heat") + ".fs.glsl");
			}
			else if (weather == FOG && next_particle_spawn < 0.f) {
				next_particle_spawn = 3000;
				vec2 velocity = { -100.f, 0.f };
				vec2 position = { WINDOW_SIZE_IN_PX.x, rand() % (WINDOW_SIZE_IN_PX.y - 230) };
				float life = 13500.f;
				std::string texture = "cloud.png";
				std::string shader = "fog";
				ParticleSystem::createParticle(velocity, position, life, texture, shader);
			}
			else if (weather == SNOW && next_particle_spawn < 0.f)
			{
				next_particle_spawn = 40;
				vec2 velocity = { rand() % 400 + (-200), 300.0f };
				vec2 position = { rand() % WINDOW_SIZE_IN_PX.x + 1 , 0 };
				float life = 2300.0f;
				std::string texture = "snow.png";
				std::string shader = "snow";
				ParticleSystem::createParticle(velocity, position, life, texture, shader);
			}
		}

		// remove disapperaing text when time's up 
		auto view_disappearing_text = registry.view<DisappearingText>();
		for (auto entity : view_disappearing_text)
		{
			auto& disap_time = view_disappearing_text.get<DisappearingText>(entity);
			disap_time.on_screen_time_ms -= elapsed_ms * current_speed;
			if (disap_time.on_screen_time_ms < 0)
			{
				registry.destroy(entity);
			}
		}

		// change hit point text scale
		auto view_hitpoints_text = registry.view<HitPointsText>();
		for (auto entity : view_hitpoints_text)
		{
			auto& hitpoints_text = view_hitpoints_text.get<HitPointsText>(entity);
			auto& text = registry.get<Text>(entity);
			float scale_change = 0.005 * elapsed_ms * current_speed;
			if (text.scale - scale_change > hitpoints_text.min_scale)
			{
				text.scale -= scale_change;
			}
		}

		// change slow text y position
		auto view_slow_text = registry.view<EffectHitText>();
		for (auto entity : view_slow_text)
		{
			auto& slow_text = view_slow_text.get<EffectHitText>(entity);
			auto& text = registry.get<Text>(entity);
			float delta_y = 0.05 * elapsed_ms * current_speed;
			if (slow_text.y_distance > 0)
			{
				text.position.y += delta_y;
				slow_text.y_distance -= delta_y;
			}
		}

		auto& food_num_text = registry.get<Text>(food_text_entity);
		food_num_text.content = std::to_string(health);

		// Increment round number if all enemies are not on the map and projectiles are removed
		if (num_bosses_spawned >= max_boss && num_mobs_spawned >= max_mobs)
		{
			if (registry.view<Monster>().empty() && registry.view<Projectile>().empty() && registry.view<HealthOrb>().empty())
			{
				end_battle_phase_step(elapsed_ms);
			}
		}
	}
	
	// health orb
	for (auto entity : registry.view<HealthOrb>())
	{
		auto& health_drop = registry.get<HealthOrb>(entity);
		auto& motion = registry.get<Motion>(entity);
		if (health_drop.clicked)
		{
			float c_constant = FOOD_NUM_X_OFFSET;
			float a_constant = (health_drop.starting_point.x - c_constant) / pow(health_drop.starting_point.y, 2);
			float delta_x = 7.5f;
			float tangent_slope = 1 / (2 * sqrt(a_constant) * sqrt(motion.position.x - c_constant));
			motion.velocity.x = -1.f;
			
			
			if (motion.position.x < c_constant)
			{
				a_constant = (c_constant - health_drop.starting_point.x) / pow(health_drop.starting_point.y, 2);
				tangent_slope = 1 / (2 * sqrt(a_constant) * sqrt(c_constant - motion.position.x));
				motion.velocity.x = 1.f;
			}

			motion.velocity.y = -1.f * tangent_slope;
			float speed = 800.f;
			motion.velocity = speed * normalize(motion.velocity);
			
			if (motion.position.y < FOOD_NUM_Y_OFFSET || isnan(tangent_slope))
			{
				registry.get<SoundRef>(entity).play_sound = true;
				add_health(health_drop.food_gain_amount);
				registry.destroy(entity);
			}
			// shrink
			if (motion.scale.x > 0.f)
			{
				//health_drop.scale_change -= 0.01 * elapsed_ms;
				motion.scale -= 0.005 * elapsed_ms;
			}
		}
		else
		{
			// hover
			if (motion.position.y < health_drop.starting_point.y - health_drop.hover_distance)
			{
				if (motion.velocity.y < 0)
				{
					motion.velocity.y *= -1.f;
				}
			}
			else if (motion.position.y > health_drop.starting_point.y)
			{
				if (motion.velocity.y > 0)
				{
					motion.velocity.y *= -1.f;
				}
			}
			//shrink delay
			if (health_drop.shrink_delay_ms > 0.f)
			{
				health_drop.shrink_delay_ms -= elapsed_ms;
			}
			else
			{
				// shrink
				if (motion.scale.x > 0.f)
				{
					//health_drop.scale_change -= 0.01 * elapsed_ms;
					motion.scale -= 0.005 * elapsed_ms;
				}
				else
				{
					registry.destroy(entity);
				}
			}
		}
		
		
	}
}


void WorldSystem::end_battle_phase_step(float elapsed_ms)
{
	if (end_of_battle_stage_dealy_ms == END_OF_BATTLE_STAGE_DELAY_MS) {
		// round cleared text
		auto closeness_outline = TextFont::load("data/fonts/Closeness/closeness.outline-regular.ttf");
		auto closeness_regular = TextFont::load("data/fonts/Closeness/closeness.regular.ttf");
		if (!round_skipped) {
            vec2 text_position = get_center_text_position(WINDOW_SIZE_IN_PX, { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2 }, 2.f, "ROUND CLEARED!");
            auto round_clear_text = DisappearingText::createDisappearingText(closeness_regular, "ROUND CLEARED!", text_position, 1000, 2.f, vec3({ 245.f / 255.f, 216.f / 255.f, 51.f / 255.f }));
            DisappearingText::createDisappearingText(closeness_outline, "ROUND CLEARED!", text_position, 1000, 2.f, vec3({ 0.f, 0.f, 0.f }));
            auto& sound = registry.emplace<SoundRef>(round_clear_text);
            sound.file_path = "ui/round_cleared_sound.wav";
		}
		// change fastforward texture to not light up
		UI_button::fastforward_light_off();
		// hide fastforward button and showi start_button
		auto view_ui_button = registry.view<Button, ShadedMeshRef>();
		for (auto [entity, button, shaded_mesh_ref] : view_ui_button.each())
		{
			if (button == Button::start_button)
			{
				RenderSystem::show_entity(entity);
			}
			else if (button == Button::fastforward_button)
			{
				RenderSystem::hide_entity(entity);
			}
		}
		// if no unit is selected, show build tower units
		if (!unit_selected)
		{
			for (auto entity : registry.view<UI_build_unit>())
			{
				RenderSystem::show_entity(entity);
			}
		}

		// if no greenhouse, shorten the end phase delay
		if (!greenhouse_food_increased && registry.view<GreenHouse>().size() == 0)
		{
			end_of_battle_stage_dealy_ms -= 500;
			greenhouse_food_increased = true;
		}
	}
	
	
	
	if (!greenhouse_food_increased && (end_of_battle_stage_dealy_ms < END_OF_BATTLE_STAGE_DELAY_MS - 500))
	{
		// greenhouse triggers at the end of battle phase once with a short delay
		int total_greenhouse_food = 0;
		for (auto entity : registry.view<GreenHouse>())
		{
			auto greenhouse = registry.get<Unit>(entity);
			total_greenhouse_food += (int)((float)greenhouse.damage * reward_multiplier);
		}
		if (total_greenhouse_food != 0)
		{
			add_health(total_greenhouse_food);
		}
		greenhouse_food_increased = true;
	}
	// count down timer
	end_of_battle_stage_dealy_ms -= elapsed_ms * current_speed;
	// end battle phase and set up next round 
	if (end_of_battle_stage_dealy_ms <= 0.f)
	{
		end_battle_phase();
		greenhouse_food_increased = false;
		end_of_battle_stage_dealy_ms = END_OF_BATTLE_STAGE_DELAY_MS;
	}

    round_skipped = false;
	update_look_for_selected_buttons(GLFW_PRESS, true);
}

void WorldSystem::title_screen_step(float elapsed_ms)
{
	// eyes blinking
	for (auto entity : registry.view<TitleEyes, ShadedMeshRef>())
	{
		auto& eyes = registry.get<TitleEyes>(entity);
		auto& shaded_mesh_ref = registry.get<ShadedMeshRef>(entity);

		eyes.blink_delay_ms -= elapsed_ms;
		// time to blink
		if (eyes.blink_delay_ms < 0.f)
		{
			shaded_mesh_ref.show = false;
			eyes.blink_time_ms -= elapsed_ms;
		}
		// time to open eyes
		if (eyes.blink_time_ms < 0.f)
		{
			if (eyes.show) {
				shaded_mesh_ref.show = true;
			}
			eyes.blink_delay_ms = rand() % 4000 + 1000; // 1 ~ 5 sec
			eyes.blink_time_ms = 200;
		}
	}

	auto particle_view = registry.view<ParticleSystem>();
	if (particle_view.size() < 200) {
		
		ParticleSystem::updateParticle();

		next_particle_spawn -= elapsed_ms;
		
		if (next_particle_spawn < 0.f)
		{
			//std::cout << "Size : " << particle_view.size() << "\n";
			next_particle_spawn = 1;
			vec2 velocity = { rand() % 100 - 50, rand() % 100 - 50 };
			vec2 position = { rand() % WINDOW_SIZE_IN_PX.x / 2 + WINDOW_SIZE_IN_PX.x + 1 , rand() % 250 + 171};
			float life = 200000.0f; // 200 sec
			std::string texture = "swarm.png";
			std::string shader = "snow";
			ParticleSystem::createParticle(velocity, position, life, texture, shader);
		}
	}
	
	ParticleSystem::updateParticle();

	for (auto particle_entity : particle_view) {
		auto& particle = registry.get<ParticleSystem>(particle_entity);
		particle.life -= elapsed_ms;
		if (particle.life <= 0) {
			registry.destroy(particle_entity);
		}
	}

	for (auto entity : particle_view) {
		auto& motion = registry.get<Motion>(entity);
		if (motion.position.x < - 100)
		{
			if (motion.velocity.x < 0)
				motion.velocity.x *= -1;
		}
		else if (motion.position.x > WINDOW_SIZE_IN_PX.x + 100)
		{
			if (motion.velocity.x > 0)
				motion.velocity.x *= -1;
		}
		if (motion.position.y < 170)
		{
			if (motion.velocity.y < 0)
				motion.velocity.y *= -1;
		}
		else if (motion.position.y > 250)
		{
			if (motion.velocity.y > 0)
				motion.velocity.y *= -1;
		}
	}
}

void WorldSystem::animation_step(float elapsed_ms)
{
	// animation
	fps_ms -= elapsed_ms;
	if (fps_ms < 0.f)
	{
		for (auto entity : registry.view<Animate>())
		{
			auto& animate = registry.get<Animate>(entity);
			animate.frame += 1;
			animate.frame = (int)animate.frame % (int)animate.frame_num;
		}
		fps_ms = 1000 / MENU_ANIMATION_FPS;
	}
}

void create_victory_stats_text(std::map<monster_type, int> player_stats)
{
	std::map<monster_type, std::string> monster_name = {
		{monster_type::MOB, "Rabbit"},
		{monster_type::SPRING_BOSS, "Hawk"},
		{monster_type::SUMMER_BOSS, "Moose"},
		{monster_type::FALL_BOSS, "Bear"},
		{monster_type::WINTER_BOSS, "Penguin"}
	};
	auto notoRegular = TextFont::load("data/fonts/Noto/NotoSans-Regular.ttf");
	auto entity = registry.create();
	registry.emplace<Text>(entity, Text("Monster Killed:", notoRegular, {50, WINDOW_SIZE_IN_PX.y / 2 + 50 }, 0.6f));
	float line_offset = 35.f;
	int line_counter = 0;
	for (auto stats : player_stats)
	{
		std::string stats_line = monster_name.at(stats.first) + ": " + std::to_string(stats.second);
		auto entity = registry.create();
		registry.emplace<Text>(entity, Text(stats_line, notoRegular, { 50, WINDOW_SIZE_IN_PX.y / 2 - line_offset * line_counter }, 0.4f));
		line_counter++;
	}
}

void create_lost_stats_text(std::map<monster_type, int> player_stats)
{
	std::map<monster_type, std::string> monster_name = {
		{monster_type::MOB, "Rabbit"},
		{monster_type::SPRING_BOSS, "Hawk"},
		{monster_type::SUMMER_BOSS, "Moose"},
		{monster_type::FALL_BOSS, "Bear"},
		{monster_type::WINTER_BOSS, "Penguin"}
	};
	auto notoRegular = TextFont::load("data/fonts/Noto/NotoSans-Regular.ttf");
	auto entity = registry.create();
	registry.emplace<Text>(entity, Text("Monster Killed:", notoRegular, { WINDOW_SIZE_IN_PX.x / 2 - 110, WINDOW_SIZE_IN_PX.y / 2 - 90.f }, 0.6f, { 1.f, 1.f, 1.f }));
	float line_offset = 35.f;
	int line_counter = 0;
	for (auto stats : player_stats)
	{
		std::string stats_line = monster_name.at(stats.first) + ": " + std::to_string(stats.second);
		auto entity = registry.create();
		registry.emplace<Text>(entity, Text(stats_line, notoRegular, { WINDOW_SIZE_IN_PX.x / 2 - 40, WINDOW_SIZE_IN_PX.y / 2 - 100.f - 40.f - line_offset * line_counter }, 0.4f, {1.f, 1.f, 1.f}));
		line_counter++;
	}
}

void WorldSystem::darken_screen_step(float elapsed_ms)
{
	if (weather == DROUGHT)
		screen_sprite->effect.load_from_file(shader_path("water") + ".vs.glsl", shader_path("water") + ".fs.glsl");
	auto& screen_state = registry.get<ScreenState>(screen_state_entity);
	if (screen_state.darken_screen_factor < 2.f)
	{
		screen_state.darken_screen_factor += elapsed_ms / 1000;
	}
	else
	{
		registry.clear();
		screen_state_entity = registry.create();
		registry.emplace<ScreenState>(screen_state_entity);
		camera = Camera::createCamera();
		// un-darken screen
		screen_state.darken_screen_factor = 0.f;

		// if victorious 
		if (victory)
		{
			game_state = GameState::victory_screen;
			Menu::createVictoryScreen();
			MenuButton::create_button(EXIT_BUTTON_X, EXIT_BUTTON_Y, MenuButtonType::exit_button, "Exit");
			create_victory_stats_text(player_stats);
		}
		// lost game
		else
		{
			game_state = GameState::lost_game_screen;
			// lost background
			Menu::createLostMenu();
			// player stats
			create_lost_stats_text(player_stats);
			// restart button
			MenuButton::create_button(RESTART_ROUND_BUTTON_X, RESTART_ROUND_BUTTON_Y, MenuButtonType::restart_round_button, "Restart round", { 1.4, 1.2 });
			// exit button
			MenuButton::create_button(LOST_EXIT_BUTTON_X, LOST_EXIT_BUTTON_Y, MenuButtonType::exit_button, "Exit");
		}
		
		
		
	}
	
}

// lost game
void WorldSystem::start_lost_game_screen()
{
	victory = false;
	music_state = MusicState::lost_screen_music;
	play_sound("ui/famine_lost_screen_sound.wav");
	auto& screen_state = registry.get<ScreenState>(screen_state_entity);
	screen_state.darken_screen_factor = 0.3;
	screen_state.all_dark = false;
	game_state = GameState::darken_screen;
}

void WorldSystem::start_victory_screen()
{
	victory = true;
	music_state = MusicState::vicotry_screen_music;
	auto& screen_state = registry.get<ScreenState>(screen_state_entity);
	screen_state.darken_screen_factor = 0.3;
	screen_state.all_dark = false;
	game_state = GameState::darken_screen;
}

// called at the end of battle pahse to set up next round
void WorldSystem::end_battle_phase()
{
    if (game_mode != SANDBOX) {
        world_round_number = world_round_number + 1;
    }
	
	if (world_round_number == MAX_ROUND_NUMBER)
	{
		start_victory_screen();
		return;
	}

	setup_round_from_round_number(world_round_number);

	//screen_sprite->effect.load_from_file(shader_path("water") + ".vs.glsl", shader_path("water") + ".fs.glsl");
		
	for (auto particle : registry.view<ParticleSystem>()) {
		registry.destroy(particle);
	}

	player_state = set_up_stage;
	num_bosses_spawned = 0;
	num_mobs_spawned = 0;
	prepare_setup_stage();
	save_game();
	//music_state = MusicState::player_prepare_music;
}

void WorldSystem::handle_game_tips()
{
	switch (tip_manager.tip_index)
	{
	case 0:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_CENTRE_Y, start_tips_0);
		game_state = paused;
		tip_manager.tip_index++;
		game_tips = false;
		break;
	case 1:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_CENTRE_Y, start_tips_1);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 2:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_BOTTOM_Y, start_tips_2);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 3:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_BOTTOM_Y, start_tips_3);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 4:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_CENTRE_Y, start_tips_4);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 5:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_CENTRE_Y, start_tips_5);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 6:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_CENTRE_Y, start_tips_6);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 7:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_TOP_Y, start_tips_7);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 8:
		TipCard::createTipCard(TIP_CARD_RIGHT_X, TIP_CARD_TOP_Y, start_tips_8);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 9:
		TipCard::createTipCard(TIP_CARD_LEFT_X - 50, TIP_CARD_TOP_Y, start_tips_9);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 10:
		TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_BOTTOM_Y, start_tips_10);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	}
}

void WorldSystem::deduct_health(int num) {
	// int total_deduction = (game_mode == SANDBOX) ? 0 : num;
	WorldSystem::health -= num;
	HealthChangeText::create_health_deduct_text(num, health);
}


void WorldSystem::add_health(int num) {
	WorldSystem::health += num;
	HealthChangeText::create_health_gain_text(num, health);
}

void un_highlight()
{
	auto view_ui = registry.view<HighlightBool>();
	for (auto [entity, highlight] : view_ui.each())
	{
		highlight.highlight = false;
	}
}

// helper for mouse_hover_ui_button
// show unit description when hover on unit button
void remove_descriptions()
{
	//std::cout << "Hover: " << build_ui.unit_name << "\n";
	for (auto entity : registry.view<UI_description_card>())
		registry.destroy(entity);
}

void WorldSystem::prepare_setup_stage()
{
	player_state = set_up_stage;
	remove_descriptions();
	auto view_ui_button = registry.view<UI_element, ShadedMeshRef>();
	for (auto button_entt : view_ui_button)
	{
		auto ui_button = view_ui_button.get<UI_element>(button_entt);

		if (ui_button.tag == START_BUTTON_TITLE)
		{
			RenderSystem::show_entity(button_entt);
		}
	}
	// remove hit point text that are still on the screen
	auto view_hit_point_text = registry.view<DisappearingText>();
	for (auto entity : view_hit_point_text)
	{
		registry.destroy(entity);
	}
}

void WorldSystem::set_up_step(float elapsed_ms)
{
	// Restart/End game after max rounds
	if (world_round_number <= 0 && game_tips) {
		handle_game_tips();
	}

	auto particle_view = registry.view<ParticleSystem>();
	if (particle_view.size() < MAX_PARTICLES) {
		for (auto particle_entity : particle_view) {
			auto& particle = registry.get<ParticleSystem>(particle_entity);
			particle.life -= elapsed_ms;
			if (particle.life <= 0) {
				registry.destroy(particle_entity);
			}
		}
	}
	ParticleSystem::updateParticle();

	next_particle_spawn -= elapsed_ms;

	if (weather == RAIN && next_particle_spawn < 0.f)
	{
		next_particle_spawn = 60;
		vec2 velocity = { 0.f, 450.0f };
		vec2 position = { rand() % WINDOW_SIZE_IN_PX.x + 1 , UI_TOP_BAR_HEIGHT };
		float life = 1500.0f;
		std::string texture = "raindrop.png";
		std::string shader = "rain";
		ParticleSystem::createParticle(velocity, position, life, texture, shader);
	}
	else if (weather == DROUGHT) {
		//screen_sprite->effect.load_from_file(shader_path("heat") + ".vs.glsl", shader_path("heat") + ".fs.glsl");
	}

	else if (weather == FOG && next_particle_spawn < 0.f) {
		next_particle_spawn = 3000;
		vec2 velocity = { -100.f, 0.f };
		vec2 position = { WINDOW_SIZE_IN_PX.x + 500, rand() % (WINDOW_SIZE_IN_PX.y - 230) };
		float life = 22000.f;
		std::string texture = "cloud.png";
		std::string shader = "fog";
		ParticleSystem::createParticle(velocity, position, life, texture, shader);
	}
	else if (weather == SNOW && next_particle_spawn < 0.f)
	{
		next_particle_spawn = 40;
		vec2 velocity = { rand() % 400 + (-200), 300.0f };
		vec2 position = { rand() % WINDOW_SIZE_IN_PX.x + 1 , UI_TOP_BAR_HEIGHT };
		float life = 2300.0f;
		std::string texture = "snow.png";
		std::string shader = "snow";
		ParticleSystem::createParticle(velocity, position, life, texture, shader);
	}
    set_default_paths();

    // remove disapperaing text when time's up
	auto view_disappearing_text = registry.view<DisappearingText>();
	for (auto entity : view_disappearing_text)
	{
		auto& disap_time = view_disappearing_text.get<DisappearingText>(entity);
		disap_time.on_screen_time_ms -= elapsed_ms * current_speed;
		if (disap_time.on_screen_time_ms < 0)
		{
			registry.destroy(entity);
		}
	}

	//registry.get<Text>(round_text_entity).content = std::to_string(round_number + 1);
	// only supports up to 2 digit rounds (99 max round)
	if (registry.get<Text>(round_text_entity).content.length() == 2)
		registry.get<Text>(round_text_entity).position.x = ROUND_NUM_X_OFFSET - 20;
	registry.get<Text>(food_text_entity).content = std::to_string(health);

	if(game_mode == SURVIVAL && world_round_number > 0) {
        start_round();
	}
}

void WorldSystem::set_default_paths() {// set default paths for monster AI for this round
    if (!set_AI_paths) {
        for (auto entity : registry.view<Path>())
            registry.destroy(entity);

        int size = current_round_monster_types.size();
        int num = 0;
        for (int monster_type : current_round_monster_types) {
            default_monster_paths.at(monster_type) = AISystem::MapAI::findPathAStar(current_map, monster_type);
            Path::createPath(default_monster_paths.at(monster_type), monster_type, size, num);
            num += 1;
        }
        show_path = show_path_duration_ms;
        set_AI_paths = true;
    }
}

void WorldSystem::start_round()
{
	game_tips = false;
	// hide towers buttons
	if (game_mode == NORMAL) {
        for (auto entity : registry.view<UI_build_unit>())
        {
            RenderSystem::hide_entity(entity);
        }
	}

	// hide start_button and shwo fastforward button
	auto view_ui_button = registry.view<Button, ShadedMeshRef>();
	for (auto [entity, button, shaded_mesh_ref] : view_ui_button.each())
	{
		if (button == Button::start_button)
		{
			RenderSystem::hide_entity(entity);
		}
		else if(button == Button::fastforward_button)
		{
			RenderSystem::show_entity(entity);
		}
	}

	WantedBoard::updateWantedBoardDisplay(wanted_board_entity, false);

	player_state = battle_stage;
	next_mob_spawn = 0;
	next_boss_spawn = 0;
	un_highlight();

	auto& stage_text = registry.get<Text>(stage_text_entity);
	if (game_mode == SURVIVAL) {
        stage_text.content = "SURVIVAL";
	} else {
        stage_text.content = "BATTLE";
	}

	stage_text.colour = { 1.0f, 0.1f, 0.1f };

    if (game_mode != SURVIVAL) {
        for (auto entity : registry.view<Path>())
            registry.destroy(entity);
    }
	set_AI_paths = false;

	//music_state = MusicState::player_battle_music;

	std::cout << world_season_str << " season! \n";
	std::cout << "weather " << weather << " \n";
}

void WorldSystem::setup_start_menu()
{

	std::cout << "In Start Menu\n";
	registry.clear();
	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);

	create_start_menu();
	camera = Camera::createCamera();
}

void destroy_entity(const entt::entity entity)
{
	registry.destroy(entity);
}

void WorldSystem::create_sandbox_ui()
{
	UI_button::create_add_monster_button(ADD_GAME_BUTTON_POS);
	UI_button::create_rem_monster_button(REM_GAME_BUTTON_POS);
	UI_button::create_inc_m_speed_button(INC_GAME_BUTTON_POS);
	UI_button::create_dec_m_speed_button(DEC_GAME_BUTTON_POS);
	UI_button::randomize_grid_map_button(RANDOM_BUTTON_POS);
	auto monster = create_ui_text(vec2(ADD_GAME_BUTTON_POS.x - 50, 30), "MONSTERS", .3f, vec3(1.f, 1.f, 1.f));
	auto speed = create_ui_text(vec2(INC_GAME_BUTTON_POS.x - 50, 30), "SPEED", .3f, vec3(1.f, 1.f, 1.f));
	registry.emplace<UI_build_unit>(monster);
	registry.emplace<UI_build_unit>(speed);

	max_mobs_text_entity = create_ui_text(vec2(ADD_GAME_BUTTON_POS.x - 25, 10), std::to_string(max_mobs), .3f, vec3(1.f, 1.f, 1.f));
	float monster_speed = 1000.f / (float)mob_delay_ms;
	mob_speed_text_entity = create_ui_text(vec2(INC_GAME_BUTTON_POS.x - 50, 10), std::to_string(monster_speed), .3f, vec3(1.f, 1.f, 1.f));
}

void WorldSystem::restart()
{
	std::cout << "Restarting\n";

	// Reset the game state
	current_speed = 1.f;
	health = game_mode == SANDBOX? STARTING_HEALTH*1000 : STARTING_HEALTH;				  //reset health
	placement_unit_selected = NONE; // no initial selection
	world_round_number = 0;
	season = SPRING;
	world_season_str = season_str.at(season);
	weather = CLEAR;
	reward_multiplier = 1;
	num_bosses_spawned = 0;
	num_mobs_spawned = 0;
	player_state = set_up_stage;
	max_mobs = 0;
	mob_delay_ms = 0;
	max_boss = 0;
	boss_delay_ms = 0;

	registry.each(destroy_entity);
	registry.clear(); // Remove all entities that we created

	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);

	//create UI	
	//UI_button::createUI_build_unit_button(0, watchtower_button, watchtower_unit.cost);
	UI_button::createUI_build_unit_button(0, hunter_button, unit_cost.at(HUNTER));
	UI_button::createUI_build_unit_button(1, exterminator_button, unit_cost.at(EXTERMINATOR));
	UI_button::createUI_build_unit_button(2, robot_button, unit_cost.at(ROBOT));
	UI_button::createUI_build_unit_button(3, priestess_button, unit_cost.at(PRIESTESS));
	UI_button::createUI_build_unit_button(4, snowmachine_button, unit_cost.at(SNOWMACHINE));
	UI_button::createUI_build_unit_button(5, green_house_button, unit_cost.at(GREENHOUSE));
	UI_button::createUI_build_unit_button(6, wall_button, unit_cost.at(WALL));

	if (game_mode == SANDBOX) {
		create_sandbox_ui();
	}

	// general buttons
	UI_button::createTips_button(TIPS_GAME_BUTTON_POS);
	wanted_board_button = UI_button::createWantedBoard_button(WANTED_BOARD_BUTTON_POS);
	UI_button::createStart_button(START_BATTLE_BUTTON_POS);
	UI_button::createMore_button(MORE_OPTIONS_BUTTON_POS);
	UI_button::createFastforward_button(FASTFORWARD_BUTTON_POS);

	// ui background
	UI_background::createUI_background();
	UI_background::createUI_top_bar();

	// season wheel
	UI_season_wheel::createUI_season_wheel();
	season_wheel_arrow_entity = UI_season_wheel::createUI_season_wheel_arrow();

	// weather icon
	weather_icon_entity = UI_weather_icon::createUI_weather_icon();

	// ui text
	season_text_entity = create_ui_text(vec2(SEASON_X_OFFSET, WINDOW_SIZE_IN_PX.y - SEASON_Y_OFFSET), "Spring", SEASON_SCALE);
	weather_text_entity = create_ui_text(vec2(WEATHER_TEXT_X_OFFSET, WINDOW_SIZE_IN_PX.y - WEATHER_TEXT_Y_OFFSET), "Clear", WEATHER_TEXT_SCALE);
	stage_text_entity = create_ui_text(vec2(5, 65), "PREPARE");

	// round label
	if (game_mode != SANDBOX) {
        create_ui_text(vec2(ROUND_LABEL_X_OFFSET, WINDOW_SIZE_IN_PX.y - ROUND_LABEL_Y_OFFSET), "Round:          / " + std::to_string(MAX_ROUND_NUMBER), ROUND_LABEL_SCALE);
    } else {
        create_ui_text(vec2(ROUND_LABEL_X_OFFSET, WINDOW_SIZE_IN_PX.y - ROUND_LABEL_Y_OFFSET), "SANDBOX", ROUND_LABEL_SCALE);
	}

	// round number text
	round_text_entity = create_ui_text(vec2(ROUND_NUM_X_OFFSET, WINDOW_SIZE_IN_PX.y - ROUND_NUM_Y_OFFSET), "1", ROUND_NUM_SCALE, { 1.f, 0.f, 0.f });

	// food label
	create_ui_text(vec2(FOOD_LABEL_X_OFFSET, WINDOW_SIZE_IN_PX.y - FOOD_LABEL_Y_OFFSET), "Food:", FOOD_LABEL_SCALE);

	// food number text
	food_text_entity = create_ui_text(vec2(FOOD_NUM_X_OFFSET, WINDOW_SIZE_IN_PX.y - FOOD_NUM_Y_OFFSET), "", FOOD_NUM_SCALE, { 0.f, 1.f, 0.f });

	// pause menu
	pause_menu_entity = Menu::createMenu((float)WINDOW_SIZE_IN_PX.x / 2, (float)WINDOW_SIZE_IN_PX.y / 2, "pause_menu", Menu_texture::pause_menu, LAYER_MENU + 1, vec2({ 220.f, 260.f }));
	registry.get<ShadedMeshRef>(pause_menu_entity).show = false;
	
	// help menu
	help_menu_entity = Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, "help_menu", Menu_texture::help_menu, 98, { 0.8, 0.8 });
	RenderSystem::hide_entity(help_menu_entity);
	
	// wanted board entity
	wanted_board_entity = WantedBoard::createWantedBoard();

	// create grid map
	current_map = registry.get<GridMap>(GridMap::createGridMap());
	village = Village::createVillage(current_map);
	Forest::createForest(current_map);
	egg = Egg::createEgg(current_map);

	// set up tip manager
	tip_manager = TipManager();

    BTCollision = AISystem::MonstersAI::createBehaviorTree();

	camera = Camera::createCamera();

	//reset kill count
	player_stats = {
		{monster_type::MOB, 0},
		{monster_type::SPRING_BOSS, 0},
		{monster_type::SUMMER_BOSS, 0},
		{monster_type::FALL_BOSS, 0},
		{monster_type::WINTER_BOSS, 0}
	};

	// set up variables for first round
	setup_round_from_round_number(world_round_number);
}

void WorldSystem::setup_round_from_round_number(int round_number)
{
	auto& stage_text = registry.get<Text>(stage_text_entity);
	stage_text.content = "PREPARE";
	stage_text.colour = { 1.0f, 1.0f, 1.0f };

	remove_game_tip_and_story_card();

	if (game_mode == SANDBOX) {
		if (max_mobs == 0) {
			max_mobs = 10;
			mob_delay_ms = 1000;
			max_boss = 10;
			boss_delay_ms = 1000;
		}

//        if (season == SPRING) {
//            season = SUMMER;
//        }
//        else if (season == SUMMER) {
//            season = FALL;
//        }
//        else if (season == FALL) {
//            season = WINTER;
//        }
//        else if (season == WINTER) {
//            season = SPRING;
//        }

		auto& stage_text = registry.get<Text>(stage_text_entity);

		auto& max_mobs_text = registry.get<Text>(max_mobs_text_entity);
		max_mobs_text.content = std::to_string(max_mobs);

		auto& mob_speed_text = registry.get<Text>(mob_speed_text_entity);
		float monster_speed = 1000.f / (float)mob_delay_ms;
		mob_speed_text.content = std::to_string(monster_speed);
	}
	else {
		nlohmann::json round_json = get_json(INPUT_PATH + std::to_string(round_number) + JSON_EXTENSION);
		max_mobs = round_json["max_mobs"];
		mob_delay_ms = round_json["mob_delay_ms"];
		max_boss = round_json["max_bosses"];
		boss_delay_ms = round_json["boss_delay_ms"];
		world_season_str = round_json["season"];
		season = season_str_to_enum.at(world_season_str);
	}

	if (game_state != help_menu)
	{
	    if (game_mode == SANDBOX || game_mode == SURVIVAL) {
	        game_state = in_game;
	    } else {
            game_state = story_card;
            StoryCard curr_story_card(STORY_TEXT_PER_LEVEL[round_number + 1], round_number);
            TalkyBoi::createTalkyBoiEntt(round_number);
	    }
	}

	int prev_weather = weather;
    current_round_monster_types.clear();
	if (max_mobs > 0 && world_round_number < 16)
		current_round_monster_types.emplace_back(MOB);

    if (game_mode != SANDBOX) {
        set_random_weather();
    }

	if (prev_weather != weather || round_number <= 0) {
	    AISystem::MapAI::setRandomMapWeatherTerrain(current_map, weather);
	} else {
        // re-roll some fraction of map for weather terrains
        int max_rerolls = (int)ceil(0.7 * MAP_SIZE_IN_COORD.x * MAP_SIZE_IN_COORD.y);
        AISystem::MapAI::setRandomWeatherTerrain(current_map, max_rerolls, weather);
	}
    // reset speed up factor
    speed_up_factor = 1.f;
	// set music state
	music_state = MusicState::normal_round_music;
    set_round_monsters();
    update_weather_season_UI(round_number);
}

void WorldSystem::update_weather_season_UI(int round_number) const {//update wanted board
    WantedBoard::updateWantedEntries(wanted_board_entity, current_round_monster_types, world_round_number, reward_multiplier);
    UI_button::wantedboard_update_on(wanted_board_button);

    if (weather == DROUGHT) {
        screen_sprite->effect.load_from_file(shader_path("heat") + ".vs.glsl", shader_path("heat") + ".fs.glsl");
    } else {
        screen_sprite->effect.load_from_file(shader_path("water") + ".vs.glsl", shader_path("water") + ".fs.glsl");
    }

    // update text
    auto& round_text = registry.get<Text>(round_text_entity);
    if (game_mode == SANDBOX) {
        round_text.content = "";
    } else {
        round_text.content = std::to_string(round_number + 1);
    }

    // only supports up to 2 digit rounds (99 max round)
    if (round_text.content.length() == 2)
        round_text.position.x = ROUND_NUM_X_OFFSET - 20;

    auto& food_num_text = registry.get<Text>(food_text_entity);
    food_num_text.content = std::to_string(health);

    auto& season_text = registry.get<Text>(season_text_entity);
    season_text.content = season_str.at(season);
    season_text.colour = season_str_colour.at(season);

    auto& weather_text = registry.get<Text>(weather_text_entity);
    weather_text.content = weather_str.at(weather);
    weather_text.colour = weather_str_colour.at(weather);
    // update season wheel angle
    UI_season_wheel::set_arrow(season_wheel_arrow_entity, season, round_number);
    UI_weather_icon::change_weather_icon(weather_icon_entity, weather);

    // monster path reset
    for (auto entity : registry.view<Path>())
        registry.destroy(entity);
    set_AI_paths = false;
}

void WorldSystem::set_round_monsters() {
    int boss;
    current_round_monster_types.clear();

    if (max_mobs > 0)
        current_round_monster_types.emplace_back(MOB);

    if (world_season_str == season_str.at(SPRING))
    {
        create_boss = SpringBoss::createSpringBossEntt;
        boss = SPRING_BOSS;
    }
    else if (world_season_str == season_str.at(SUMMER)){
        create_boss = SummerBoss::createSummerBossEntt;
        boss = SUMMER_BOSS;
    }
    else if (world_season_str == season_str.at(FALL)){
        create_boss = FallBoss::createFallBossEntt;
        boss = FALL_BOSS;
    }
    else if (world_season_str == season_str.at(WINTER)){
        create_boss = WinterBoss::createWinterBossEntt;
        boss = WINTER_BOSS;
    }
    else if (world_season_str == season_str.at(FINAL)){
        create_boss = DragonRig::createDragon;
        boss = FINAL_BOSS;
		// setup music
		music_state = MusicState::final_round_music;
    }
    if (boss != FINAL_BOSS && max_boss > 0) {
        current_round_monster_types.emplace_back(boss);
    }
}


void WorldSystem::set_random_weather() {
    if (world_season_str == season_str.at(SPRING))
    {
        reward_multiplier = 2.f;
        int weather_int = rand() % 2 + 1;
        if (weather_int % 2 == 1)
        {
            weather = RAIN;
        } else {
            weather = CLEAR;
        }
    }
    else if (world_season_str == season_str.at(SUMMER))
    {
        reward_multiplier = 1.f;
        int weather_int = rand() % 5 + 1;
        if (weather_int % 2 == 1)
        {
            weather = DROUGHT;
        } else {
            weather = CLEAR;
        }
    }
    else if (world_season_str == season_str.at(FALL))
    {
        reward_multiplier = 1.f;
        int weather_int = rand() % 5 + 1;
        if (weather_int % 2 == 1)
        {
            weather = FOG;
        } else {
            weather = CLEAR;
        }
    }
    else if (world_season_str == season_str.at(WINTER))
    {
        reward_multiplier = 0.5f;
        int weather_int = rand() % 2 + 1;
        if (weather_int % 2 == 1)
        {
            weather = SNOW;
        } else {
            weather = CLEAR;
        }
    }
    else if (world_season_str == season_str.at(FINAL))// FINAL_TITLE) else ifSPRING_TITLE
    {
        //fireball_delay_ms = FIREBALL_DELAY_MS;//5100;
        //next_fireball_spawn = fireball_delay_ms;

        int weather_int = rand() % 5 + 1;
        if (weather_int % 2 == 1)
        {
            weather = DROUGHT;
        }
        else {
            weather = CLEAR;
        }
        std::cout << "SPAWNING FINAL BOSS" << std::endl;
        create_boss = DragonRig::createDragon;
		// set final round music
		music_state = MusicState::final_round_music;
    }
	if (world_round_number > 7) {
		reward_multiplier *= 0.5;
	}
}

void WorldSystem::setup_round_from_save_file(int round_number, int weather)
{
	auto& stage_text = registry.get<Text>(stage_text_entity);
	stage_text.content = "PREPARE";
	stage_text.colour = { 1.0f, 1.0f, 1.0f };

	remove_game_tip_and_story_card();

	if (game_mode == SANDBOX) {
		max_mobs = 10;
		mob_delay_ms = 1000;
		max_boss = 10;
		boss_delay_ms = 1000;

		auto& stage_text = registry.get<Text>(stage_text_entity);

		auto& max_mobs_text = registry.get<Text>(max_mobs_text_entity);
		max_mobs_text.content = std::to_string(max_mobs);

		auto& mob_speed_text = registry.get<Text>(mob_speed_text_entity);
		float monster_speed = 1000.f / (float)mob_delay_ms;
		mob_speed_text.content = std::to_string(monster_speed);
	}
	else {
		nlohmann::json round_json = get_json(INPUT_PATH + std::to_string(round_number) + JSON_EXTENSION);
		max_mobs = round_json["max_mobs"];
		mob_delay_ms = round_json["mob_delay_ms"];
		max_boss = round_json["max_bosses"];
		boss_delay_ms = round_json["boss_delay_ms"];
		world_season_str = round_json["season"];
        season = season_str_to_enum.at(world_season_str);
	}

	if (game_state != help_menu)
	{
		if (game_mode == SANDBOX || game_mode == SURVIVAL) {
			game_state = in_game;
		}
		else {
			game_state = story_card;
			StoryCard curr_story_card(STORY_TEXT_PER_LEVEL[round_number + 1], round_number);
			TalkyBoi::createTalkyBoiEntt(round_number);
		}
	}
	// weather related
	if (weather == DROUGHT)
	{
		screen_sprite->effect.load_from_file(shader_path("heat") + ".vs.glsl", shader_path("heat") + ".fs.glsl");
	}
	else
	{
		screen_sprite->effect.load_from_file(shader_path("water") + ".vs.glsl", shader_path("water") + ".fs.glsl");
	}
	// set up boss by season
	if (world_season_str == season_str.at(SPRING))
	{
		create_boss = SpringBoss::createSpringBossEntt;
		if (max_boss > 0)
			current_round_monster_types.emplace_back(SPRING_BOSS);
	}
	else if (world_season_str == season_str.at(SUMMER))
	{
		create_boss = SummerBoss::createSummerBossEntt;
		if (max_boss > 0)
			current_round_monster_types.emplace_back(SUMMER_BOSS);
	}
	else if (world_season_str == season_str.at(FALL))
	{
		create_boss = FallBoss::createFallBossEntt;
		if (max_boss > 0)
			current_round_monster_types.emplace_back(FALL_BOSS);
	}
	else if (world_season_str == season_str.at(WINTER))
	{
		create_boss = WinterBoss::createWinterBossEntt;
		if (max_boss > 0)
			current_round_monster_types.emplace_back(WINTER_BOSS);
	}
	else if (world_season_str == season_str.at(FINAL))// FINAL_TITLE) else ifSPRING_TITLE
	{
		std::cout << "SPAWNING FINAL BOSS" << std::endl;
		create_boss = DragonRig::createDragon; //FinalBoss::createFinalBossEntt; //
		// set final round music
		music_state = MusicState::final_round_music;
	}

	//update wanted board
	WantedBoard::updateWantedEntries(wanted_board_entity, current_round_monster_types, world_round_number, reward_multiplier);
	UI_button::wantedboard_update_on(wanted_board_button);

	// update text
	auto& round_text = registry.get<Text>(round_text_entity);
	if (game_mode == SANDBOX) {
		round_text.content = "";
	}
	else {
		round_text.content = std::to_string(round_number + 1);
	}
	// only supports up to 2 digit rounds (99 max round)
	if (round_text.content.length() == 2)
		round_text.position.x = ROUND_NUM_X_OFFSET - 20;

	auto& food_num_text = registry.get<Text>(food_text_entity);
	food_num_text.content = std::to_string(health);

	auto& season_text = registry.get<Text>(season_text_entity);
	season_text.content = season_str.at(season);
	season_text.colour = season_str_colour.at(season);

	auto& weather_text = registry.get<Text>(weather_text_entity);
	weather_text.content = weather_str.at(weather);
	weather_text.colour = weather_str_colour.at(weather);
	// update season wheel angle
	UI_season_wheel::set_arrow(season_wheel_arrow_entity, season, round_number);
	// monster path reset
	for (auto entity : registry.view<Path>())
		registry.destroy(entity);
	set_AI_paths = false;

	UI_weather_icon::change_weather_icon(weather_icon_entity, weather);
}

void WorldSystem::damage_monster_helper(entt::entity e_monster, entt::entity e_projectile, int damage, bool slow) {
	
	auto& monster = registry.get<Monster>(e_monster);

	if (slow) {
		auto& damage_properties = registry.get<DamageProperties>(e_monster);
		damage_properties.slow_queue.push({ damage, SLOW_DELAY });
		float max_slow = damage_properties.slow_queue.top().first;
		monster.speed_multiplier *= (100 / (100 - damage_properties.current_slow));
		monster.speed_multiplier *= ((100 - max_slow) / 100);
		damage_properties.current_slow = max_slow;
		// add hit point text
        EffectHitText::create_effect_hit_text("slow", e_monster, {0.5, 1.f, 1.f});
	}
	else {
		monster.health -= damage;
		// add hit point text
		HitPointsText::create_hit_points_text(damage, e_monster, { 1.f, 0.f, 0.f });
		// health bar
		auto& hit_reaction = registry.get<HitReaction>(e_monster);
		hit_reaction.counter_ms = hit_reaction.counter_interval; //ms duration used by health bar
	}
	// set projectile sound to true when hit monster
	if (registry.has<SoundRef>(e_projectile))
	{
		auto& sound_ref = registry.get<SoundRef>(e_projectile);
		if (sound_ref.is_continuous)
			sound_ref.play_sound = true;
		else if (!sound_ref.one_time_sound_played)
			sound_ref.play_sound = true;
	}
	monster.collided = true;
	

	auto& hit_reaction = registry.get<HitReaction>(e_monster);
	hit_reaction.counter_ms = hit_reaction.counter_interval; //ms duration used by health bar

	if (monster.health <= 0)
	{
		
		auto food_gain_amount = (int)((float)monster.reward * reward_multiplier);
		auto& motion = registry.get<Motion>(e_monster);
		// health drop
		HealthOrb::createHealthOrb(motion.position, food_gain_amount);
		// gain food
		//add_health(food_gain_amount);
		// remove monster
		update_player_stats_monster(e_monster);
		if (registry.has<Rig>(e_monster)) {
			Rig::delete_rig(e_monster); //rigs have multiple pieces to be deleted
		}
		else {
			registry.destroy(e_monster);
			
		}
		
        Ghost::createGhostEntt(motion.position);
	}
}

void WorldSystem::update_player_stats_monster(entt::entity e_monster)
{
	try
	{
		auto type = registry.get<Monster>(e_monster).type;
		player_stats.at(type)++;
	}
	catch (std::out_of_range& const e)
	{

	}
	
}

void WorldSystem::updateProjectileMonsterCollision(entt::entity e_projectile, entt::entity e_monster)
{
	auto &prj = registry.get<Projectile>(e_projectile);

	if (registry.has<Snowball>(e_projectile)) {
		if (!registry.has<EntityDeath>(e_projectile)) {
			auto& death = registry.emplace<EntityDeath>(e_projectile);
			death.timer = 1000;

			auto& rock = registry.get<Snowball>(e_projectile);
			auto& prj_motion = registry.get<Motion>(e_projectile);
			auto& monster_motion = registry.get<Motion>(e_monster);

			vec2 vector = (monster_motion.position - prj_motion.position);
			vec2 norm;
			if ((vector.x >= 0 && vector.y >= 0) || (vector.x < 0 && vector.y < 0)) norm = { vector.y, -vector.x };
			else norm = { vector.y, vector.x };

			std::vector<vec2> points;
			points.push_back(prj_motion.position);
			points.push_back(prj_motion.position + norm);
			points.push_back(prj_motion.position + norm - vector);

			rock.bezier_points = bezierVelocities(bezierCurve(points, 1000));
		}
		damage_monster_helper(e_monster, e_projectile, prj.damage, true);
	}
	else if (registry.has<Flamethrower>(e_projectile) || registry.has<LaserBeam>(e_projectile) || registry.has<Explosion>(e_projectile) || registry.has<IceField>(e_projectile)) {
		auto& dot = registry.get<DamageProperties>(e_monster);
		if (dot.dot_map.find(e_projectile) == dot.dot_map.end()) {
			dot.dot_map.insert({ e_projectile, DOT_DELAY });
			damage_monster_helper(e_monster, e_projectile, prj.damage, registry.has<IceField>(e_projectile));
		}
		else {
			if (dot.dot_map[e_projectile] <= 0) {
				dot.dot_map[e_projectile] = DOT_DELAY;
				damage_monster_helper(e_monster, e_projectile, prj.damage, registry.has<IceField>(e_projectile));
			}
		}
	}

	else if (registry.has<Missile>(e_projectile)) {
		Explosion::createExplosion(e_projectile, prj.damage);
		registry.destroy(e_projectile);
	}

	else {
		damage_monster_helper(e_monster, e_projectile, prj.damage);
		registry.destroy(e_projectile);
	}
}

bool WorldSystem::is_over() const
{
	return glfwWindowShouldClose(window) > 0;
}

void WorldSystem::on_key(int key, int, int action, int mod)
{
	// if village is alive
	if (health > 0)
	{
	}
	if (key == GLFW_KEY_T) // for testing rigs
	{
	}
	if (key == GLFW_KEY_Y) // for testing rigs
	{
		
	}
	if (key == GLFW_KEY_N) // for testing rigs
	{
		//auto& camera1 = registry.get<MouseMovement>(camera);
		//auto view_rigs = registry.view<Rig>();
		//for (entt::entity rig : view_rigs) {
		//	RigSystem::ik_solve(rig, camera1.mouse_pos, 1);
		//}

	}
	// keys used to skip rounds; used to debug and test rounds
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_G)
	{
		if (player_state == set_up_stage)
		{
			start_round();
		}
		else if (player_state == battle_stage)
		{
			num_bosses_spawned = max_boss;
			num_mobs_spawned = max_mobs;
			for (entt::entity projectile : registry.view<Projectile>())
			{
				registry.destroy(projectile);
			}
			for (entt::entity monster : registry.view<Monster>())
			{
				if (registry.has<Rig>(monster)) {
					Rig::delete_rig(monster); //rigs have multiple pieces to be deleted
				}
				else {
					registry.destroy(monster);
				}
			}
		}
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_P && game_state == in_game) {
		pause_game();
		more_options_menu();
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_P && game_state == paused) {
		resume_game();
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE)
	{
		if (game_state == GameState::start_menu)
		{
			// Possibly close the game (same as clicking exit button)
		}
		else if (game_state == GameState::in_game)
		{
			if (registry.valid(entity_selected))
			{
				registry.destroy(entity_selected);
				if (registry.valid(entity_range_circle))
					registry.destroy(entity_range_circle);
				placement_unit_selected = unit_type::NONE;
				un_highlight();
			}
			else if (unit_selected)
			{
				unit_selected = false;
				update_look_for_selected_buttons(GLFW_PRESS, false);
				un_highlight();
			}
			else
			{
				pause_game();
				more_options_menu();
			}
		}
		else if (game_state == GameState::paused)
		{
			resume_game();
		}
	}

	// hotkey for controls
	if (action == GLFW_RELEASE && key == GLFW_KEY_H)
	{
	    if (game_state == in_game) {
			RenderSystem::show_entity(help_menu_entity);
            game_state = help_menu;
	    }
	    else if (game_state == help_menu) {
			RenderSystem::hide_entity(help_menu_entity);
            game_state = in_game;
	    }
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE)
	{
		auto view = registry.view<Motion, MouseMovement>();
		auto &cam_motion = view.get<Motion>(camera);
		auto &mouse_move = view.get<MouseMovement>(camera);
		mouse_move.mouse_start = mouse_move.mouse_pos + cam_motion.position;
		mouse_move.is_pan_state = 1;
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE)
	{
		auto view = registry.view<Motion, MouseMovement>();
		auto &mouse_move = view.get<MouseMovement>(camera);
		mouse_move.is_pan_state = 0;
	}

	// Hot keys for selecting placeable units

	else if (action == GLFW_PRESS && key == GLFW_KEY_6)
	{
		placement_unit_selected = GREENHOUSE;
		create_unit_indicator = GreenHouse::createGreenHouse;
	}
    else if (action == GLFW_PRESS && key == GLFW_KEY_1)
	{
		placement_unit_selected = HUNTER;
		create_unit_indicator = Hunter::createHunter;
	}
    else if (action == GLFW_PRESS && key == GLFW_KEY_7)
    {
        placement_unit_selected = WALL;
		create_unit_indicator = Wall::createWall;
    }
	else if (action == GLFW_PRESS && key == GLFW_KEY_2)
	{
		placement_unit_selected = EXTERMINATOR;
		create_unit_indicator = Exterminator::createExterminator;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_3)
	{
		placement_unit_selected = ROBOT;
		create_unit_indicator = Robot::createRobot;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_4)
	{
		placement_unit_selected = PRIESTESS;
		create_unit_indicator = Priestess::createPriestess;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_5)
	{
		placement_unit_selected = SNOWMACHINE;
		create_unit_indicator = SnowMachine::createSnowMachine;
	}
	// Resetting game
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_R)
	{
		if (game_state == in_game) {
			int w, h;
			glfwGetWindowSize(window, &w, &h);

			restart_with_save();
		}
	}

	// Debugging
	if (action == GLFW_PRESS && key == GLFW_KEY_D)
		DebugSystem::in_debug_mode = !DebugSystem::in_debug_mode;
		//DebugSystem::in_debug_mode = (action != GLFW_RELEASE);

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
	{
		current_speed -= 0.1f;
		std::cout << "Current speed = " << current_speed << std::endl;
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
	{
		current_speed += 0.1f;
		std::cout << "Current speed = " << current_speed << std::endl;
	}
	current_speed = std::max(0.f, current_speed);
}

void WorldSystem::skip_to_final_round() {
    if (player_state == set_up_stage)
    {
        start_round();
    }

    num_bosses_spawned = 0;
    num_mobs_spawned = 0;
    for (entt::entity projectile : registry.view<Projectile>())
    {
        registry.destroy(projectile);
    }
    for (entt::entity monster : registry.view<Monster>())
    {
        if (registry.has<Rig>(monster)) {
            Rig::delete_rig(monster); //rigs have multiple pieces to be deleted
        }
        else {
            registry.destroy(monster);
        }
    }

    world_season_str = season_str.at(FINAL);
	world_round_number = 16;
	setup_round_from_round_number(world_round_number);
	start_round();
}

void WorldSystem::pause_game()
{
	std::cout << "Paused" << std::endl;
	game_state = paused;
	Mix_Pause(-1);
}

void WorldSystem::more_options_menu()
{
	// pause menu
	registry.get<ShadedMeshRef>(pause_menu_entity).show = true;
	auto menu_ui = registry.get<UI_element>(pause_menu_entity);

	float top_button_y_offset = menu_ui.position.y - menu_ui.scale.y / 2.f - 10;
	MenuButton::create_button(menu_ui.position.x, top_button_y_offset + menu_ui.scale.y * 1.f / 4.5f, MenuButtonType::restart_round_button, "Restart round", { 1.4f, 1.2f });
	MenuButton::create_button(menu_ui.position.x, top_button_y_offset + menu_ui.scale.y * 2.f / 4.5f, MenuButtonType::menu_save_button, "Save", { 1.2f, 1.0f });
	MenuButton::create_button(menu_ui.position.x, top_button_y_offset + menu_ui.scale.y * 3.f / 4.5f, MenuButtonType::help_button, "Help", { 1.2f, 1.0f });
	MenuButton::create_button(menu_ui.position.x, top_button_y_offset + menu_ui.scale.y * 4.f / 4.5f, MenuButtonType::exit_button, "Exit", { 1.2f, 1.0f });
}

void WorldSystem::resume_game()
{
	std::cout << "Game Resumed" << std::endl;
	game_state = in_game;
	Mix_Resume(-1);
	// hide pause menu and destroy all menu buttons
	registry.get<ShadedMeshRef>(pause_menu_entity).show = false;
	auto menu_button_view = registry.view<MenuButton>();
	for (auto entity : menu_button_view)
		registry.destroy(entity);
}

bool mouse_in_game_area(vec2 mouse_pos)
{
	return (mouse_pos.x > 0 && mouse_pos.y > UI_TOP_BAR_HEIGHT && mouse_pos.x < MAP_SIZE_IN_PX.x && mouse_pos.y < MAP_SIZE_IN_PX.y + UI_TOP_BAR_HEIGHT);
}

void WorldSystem::scroll_callback(double xoffset, double yoffset)
{
	(void)xoffset; // avoid 'unreferenced formal parameter' warning message
	if (game_state != in_game) {
		return;
	}
	auto view = registry.view<Motion, MouseMovement>();
	auto &camera_motion = view.get<Motion>(camera);
	auto &camera_scale = camera_motion.scale;
	auto &camera_position = camera_motion.position;

	auto &mouse_movement = view.get<MouseMovement>(camera);

	double temp_scale = 20.0f;

	// zoom out limit
	if (camera_scale.y + (float)(yoffset / temp_scale) < 1)
	{
		camera_scale = {1.f, 1.f};
		camera_motion.position = {0.f, 0.f};
		return;
	}

	camera_scale.y += (float)yoffset / (float)temp_scale;
	camera_scale.x = camera_scale.y;

	float mouse_in_world_x = (float)abs(yoffset) * ((mouse_movement.mouse_pos.x + camera_position.x) / camera_scale.x) / (float)temp_scale;
	float mouse_in_world_y = (float)abs(yoffset) * ((mouse_movement.mouse_pos.y + camera_position.y) / camera_scale.y) / (float)temp_scale;

	if (yoffset > 0)
	{
		// zoom in
		// no need to check out of border
		camera_position.x += mouse_in_world_x;
		camera_position.y += mouse_in_world_y;
	}
	else
	{
		// zoom out
		double new_cam_pos_x = camera_position.x - mouse_in_world_x;
		double new_cam_pos_y = camera_position.y - mouse_in_world_y;
		// check out of map border
		if (new_cam_pos_x < 0)
		{
			new_cam_pos_x = 0;
		}
		if (new_cam_pos_y < 0)
		{
			new_cam_pos_y = 0;
		}
		if ((WINDOW_SIZE_IN_PX.x * camera_motion.scale.x) - new_cam_pos_x < WINDOW_SIZE_IN_PX.x)
		{
			//std::cout << new_pos_x + (WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) << " > " << WINDOW_SIZE_IN_PX.x << "\n";
			new_cam_pos_x = (WINDOW_SIZE_IN_PX.x * camera_motion.scale.x) - WINDOW_SIZE_IN_PX.x;
		}
		// to compensate the ui background blocking the map, increase the threshold in the y position
		auto view_ui = registry.view<UI_element>();
		int ui_element_background_height = 0;
		for (auto entity : view_ui)
		{
			UI_element ui_element = view_ui.get<UI_element>(entity);
			if (ui_element.tag == "in_game_ui_background")
			{
				ui_element_background_height = (int)ui_element.scale.y;
			}
		}
		int unsigned map_height = WINDOW_SIZE_IN_PX.y - ui_element_background_height;
		if ((map_height * camera_motion.scale.y) - new_cam_pos_y < map_height)
		{
			new_cam_pos_y = (map_height * camera_motion.scale.y) - map_height;
		}

		camera_position = vec2(new_cam_pos_x, new_cam_pos_y);
	}
}

//will move this eventually
//atm this is repeated code because ui uses a different position/scale than gridnode
void grid_highlight_system(vec2 mouse_pos, GridMap current_map)
{
	auto view_ui = registry.view<Motion, HighlightBool>();

	ivec2 mouse_coord = pixel_to_coord(mouse_pos);
	auto &node = current_map.getNodeAtCoord(pixel_to_coord(mouse_pos));
	for (auto [entity, grid_motion, highlight] : view_ui.each())
	{
		if (sdBox(mouse_pos, grid_motion.position, grid_motion.scale / 2.0f) < 0.0f && node.occupancy == NONE && node.terrain != TERRAIN_FIRE && node.terrain != TERRAIN_PUDDLE)
		{
			highlight.highlight = true;
		}
		else
		{
			highlight.highlight = false;
		}
	}
}

// helper for on_mouse_move
// manage camera translation control
void camera_control(vec2 mouse_pos)
{
	// camera control
	auto view = registry.view<Motion, MouseMovement>();
	auto& cam_motion = view.get<Motion>(camera);
	auto& mouse_move = view.get<MouseMovement>(camera);
	mouse_move.mouse_pos = mouse_pos;
	if (mouse_move.is_pan_state == 1)
	{
		// prevent pan off map
		double new_pos_x = mouse_move.mouse_start.x - mouse_pos.x;
		double new_pos_y = mouse_move.mouse_start.y - mouse_pos.y;
		if (new_pos_x < 0)
		{
			new_pos_x = 0;
		}
		if (new_pos_y < 0)
		{
			new_pos_y = 0;
		}
		if ((WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) - new_pos_x < WINDOW_SIZE_IN_PX.x)
		{
			new_pos_x = (WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) - WINDOW_SIZE_IN_PX.x;
		}
		// to compensate the ui background blocking the map, increase the threshold in the y position
		auto view_ui = registry.view<UI_element>();
		int ui_element_background_height = 0;
		for (auto entity : view_ui)
		{
			UI_element ui_element = view_ui.get<UI_element>(entity);
			if (ui_element.tag == "in_game_ui_background")
			{
				ui_element_background_height = (int)ui_element.scale.y;
			}
		}
		int unsigned map_height = WINDOW_SIZE_IN_PX.y - ui_element_background_height;
		if ((map_height * cam_motion.scale.y) - new_pos_y < map_height)
		{
			new_pos_y = (map_height * cam_motion.scale.y) - map_height;
		}
		cam_motion.position = vec2(new_pos_x, new_pos_y);
	}
}

// helper for on_mouse_move
// check if mouse is on top of unit buttons, and dispaly unit description if it is
void mouse_hover_ui_button()
{
	remove_descriptions();
	auto view_buttons = registry.view<UI_element, HighlightBool, ShadedMeshRef>();
	for (auto [entity, ui_element, highlight, shadedmeshref] : view_buttons.each()) {
		if (highlight.highlight && shadedmeshref.show) { // if a button is highlighted and we click -> button was pressed.
			if (registry.has<UI_build_unit>(entity))
			{
				UI_description_card::create_UI_description_card(entity);
			}
			else if (registry.has<UI_selected_unit>(entity))
			{
				auto& selected_unit = registry.get<UI_selected_unit>(entity);
				if (selected_unit.path_num < 3)
					UI_description_card::create_UI_description_card(entity);
			}
			break;
		}
		else
		{
			remove_descriptions();
		}
	}
	
}

//helper function for on_mouse_move 
void WorldSystem::createEntityRangeIndicator(vec2 mouse_pos)
{
	if (registry.valid(entity_selected)) {
		auto& unit = registry.get<Unit>(entity_selected);
		
		if (placement_unit_selected == unit.type)
		{
			auto& motion = registry.get<Motion>(entity_selected);
			motion.position = mouse_pos;
			auto& circle_motion = registry.get<Motion>(entity_range_circle);
			circle_motion.position = mouse_pos;
		}
		else 
		{
			registry.destroy(entity_selected);
			entity_selected = create_unit_indicator(mouse_pos);
			unit = registry.get<Unit>(entity_selected);
            unit.is_active = false;
            registry.destroy(entity_range_circle);
			entity_range_circle = RangeCircle::createRangeCircle(mouse_pos, unit.attack_range);
		}
	}
	else {
		entity_selected = create_unit_indicator(mouse_pos);

		auto& unit = registry.get<Unit>(entity_selected);
        unit.is_active = false;
		entity_range_circle = RangeCircle::createRangeCircle(mouse_pos, unit.attack_range);
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_pos)
{
	if (game_state == in_game) {
		//if mouse is hovering over a button, then highlight
		UI_highlight_system(mouse_pos);
		camera_control(mouse_pos);
		mouse_hover_ui_button();
		// hover over season
		for (auto entity : registry.view<Button, UI_element>())
		{
			if (registry.get<Button>(entity) == Button::season_button)
			{
				auto ui_element = registry.get<UI_element>(entity);
				if (sdBox(mouse_pos, ui_element.position, ui_element.scale / 2.0f) < 0.0f)
				{
					UI_description_card::create_UI_description_card(season_wheel_arrow_entity);
				}
			}
		}
		bool in_game_area = mouse_in_game_area(mouse_pos);
		if (in_game_area && placement_unit_selected != NONE && (player_state == set_up_stage || game_mode != NORMAL))
		{
			grid_highlight_system(mouse_pos, current_map);
			createEntityRangeIndicator(mouse_pos);
		}
		else {
			if (registry.valid(entity_selected))
			{
				registry.destroy(entity_selected);
				un_highlight();
			}
			if (registry.valid(entity_range_circle))
				registry.destroy(entity_range_circle);
		}
	}

	else if (game_state == start_menu)
	{
		// hover on title screen buttons, show eyes
		auto view_ui = registry.view<MenuButton, UI_element>();
		bool is_hovering = false;
		for (auto ui_entity : view_ui)
		{
			auto ui_element = registry.get<UI_element>(ui_entity);
			if (sdBox(mouse_pos, ui_element.position, ui_element.scale / 2.0f) < 0.0f) {
				is_hovering = true;
				
				registry.get<UI_element>(title_button_highlight_entity).angle = ui_element.angle;
				registry.get<UI_element>(title_button_highlight_entity).scale = vec2(ui_element.scale.x * 1.05, ui_element.scale.y * 1.15);
				if (!registry.get<ShadedMeshRef>(title_button_highlight_entity).show)
				{
					registry.get<ShadedMeshRef>(title_button_highlight_entity).show = true;
					registry.get<SoundRef>(title_button_highlight_entity).play_sound = true;
				}
				else if (registry.get<UI_element>(title_button_highlight_entity).position != vec2({ ui_element.position.x,  ui_element.position.y }))
				{
					registry.get<SoundRef>(title_button_highlight_entity).play_sound = true;
				}
				registry.get<UI_element>(title_button_highlight_entity).position = vec2({ ui_element.position.x,  ui_element.position.y });
			}
		}
		if (is_hovering)
		{
			for (auto entity : registry.view<TitleEyes>())
				registry.get<TitleEyes>(entity).show = true;
		}
		else
		{
			registry.get<ShadedMeshRef>(title_button_highlight_entity).show = false;
			registry.get<SoundRef>(title_button_highlight_entity).play_sound = false;
			for (auto entity : registry.view<TitleEyes>())
				registry.get<TitleEyes>(entity).show = false;
		}
	}
}

// helper for update_look_for_selected_buttons
// update unit stats
void update_unit_stats(Unit unit)
{
	int x_position = 220;
	int y_position = 65;
	int y_line_offset = 15;
	entt::entity damage_stats;

	std::string stat_1_string = "Attack Damage: ";
	std::string stat_2_string = "Attack speed: ";
	std::string stat_3_string = "Attack Range: ";
	std::string stat_4_string = "Current Health: ";
	
	float aps = 0.f;
	if (unit.attack_interval_ms != 0)
	{
		aps = 1000 / (float)unit.attack_interval_ms;
	}

	if (unit.type == PRIESTESS) {
		stat_1_string = "Damage Buff: ";
		stat_2_string = "Attack Speed Buff: ";
		stat_3_string = "Buff Range: ";
		aps = 1000 / aps;
	}
	else if (unit.type == GREENHOUSE) {
		stat_1_string = "Current Production: ";
	}
	else if (unit.type == SNOWMACHINE) {
		stat_1_string = "Slow Percentage: ";
	}

	// create stats text
	if (unit.damage_buff > 0) 
		damage_stats = create_ui_text(vec2(x_position, y_position), stat_1_string + std::to_string(unit.damage + unit.damage_buff), 0.3f, {1, 0, 0});
	else
		damage_stats = create_ui_text(vec2(x_position, y_position), stat_1_string + std::to_string(unit.damage));
	registry.emplace<UI_unit_stats>(damage_stats);

	
	entt::entity attack_speed_stats;
	if (unit.attack_speed_buff > 1) {
		// display aps to 2 decimals
		std::ostringstream aps_out;
		aps_out.precision(2);
		aps_out << std::fixed << (aps * unit.attack_speed_buff);

		attack_speed_stats = create_ui_text(vec2(x_position, y_position - y_line_offset), stat_2_string + aps_out.str() + " (atk/sec)", 0.3f, {1,0,0});
	}
	else {
		// display aps to 2 decimals
		std::ostringstream aps_out;
		aps_out.precision(2);
		aps_out << std::fixed << aps;

		attack_speed_stats = create_ui_text(vec2(x_position, y_position - y_line_offset), stat_2_string + aps_out.str() + " (atk/sec)");
	}
	
	registry.emplace<UI_unit_stats>(attack_speed_stats);

	// attack range
	auto attack_range_stats = create_ui_text(vec2(x_position, y_position - (2 * y_line_offset)), stat_3_string + std::to_string(unit.attack_range));
	registry.emplace<UI_unit_stats>(attack_range_stats);

	auto current_health_stats = create_ui_text(vec2(x_position, y_position - (3 * y_line_offset)), stat_4_string + std::to_string(unit.health));
	registry.emplace<UI_unit_stats>(current_health_stats);
}

void update_unit_portrait(Unit unit)
{
	// clear unit portrait
	for (auto entity : registry.view<UI_selected_unit_portrait>())
	{
		if (registry.get<UI_selected_unit_portrait>(entity).type != unit.type)
			registry.destroy(entity);
	}
	UI_selected_unit_portrait::createUI_selected_unit_portrait(unit.type);
}

//helper for the selected buttons
void update_selected_button(entt::entity e_button, Unit unit, bool remove_highlight, bool destroy_flag)
{
	if (destroy_flag) {
		auto& selected_components = registry.get<UI_selected_unit>(e_button);
		for (auto component : selected_components.button_components) {
			registry.destroy(component);
		}
		registry.destroy(e_button);
	}
	else {
		auto& selected_components = registry.get<UI_selected_unit>(e_button);
		for (auto component : selected_components.button_components) {
			registry.destroy(component);
		}
		selected_components.button_components = std::vector<entt::entity>();

		auto ui = registry.get<UI_element>(e_button);

		int path_num;
		if (ui.tag == PATH_1_UPGRADE_BUTTON_TITLE) {
			path_num = unit.path_1_upgrade;
		}
		else {
			path_num = unit.path_2_upgrade;
		}

		auto image = UI_selected_unit::create_selected_button_image(vec2(ui.position.x - ui.scale.x / 4, ui.position.y + 10), ui.tag, unit);
		auto progress = UI_selected_unit::create_selected_button_progress_bar(vec2(ui.position.x + ui.scale.x / 4, ui.position.y), path_num);

		// text
		float line_size = 35; // relative to the text size
		float left_margin = 3;
		// unit name text
		std::string key = ui.tag + "_" + unit_str.at(unit.type) + "_" + std::to_string(path_num);
		std::string short_description = upgrade_short_descriptions.at(key);
		auto title_text_scale = 0.4f;
		auto bubblegum = TextFont::load("data/fonts/MagicalMystery/MAGIMT__.ttf");
		// center text
		auto x_offset = (ui.scale.x - (short_description.length() * title_text_scale * 27)) / 2;
		// place title text at the top
		float top_margin = 10;
		auto y_title_offset = ui.scale.y / 2 - title_text_scale * line_size - top_margin;
		vec2 title_text_position = get_center_text_position(vec2(ui.scale.x / 2, ui.scale.y), vec2(ui.position.x - ui.scale.x / 4, ui.position.y), title_text_scale, short_description);
		auto& title = registry.emplace_or_replace<Text>(e_button, Text(short_description, bubblegum, vec2(title_text_position.x, title_text_position.y + y_title_offset)));
		title.scale = title_text_scale;
		title.colour = { 0.f, 0.f, 0.f };

		selected_components.button_components.push_back(image);
		selected_components.button_components.push_back(progress);

		if (remove_highlight) {
			registry.remove<HighlightBool>(e_button);
		}
	}
}

//helper for the selected buttons
void update_sell_button_text(entt::entity e_button, int sell_price)
{
	auto& UI_sell = registry.get<UI_sell_button>(e_button);
	if (registry.valid(UI_sell.sell_text))
	{
		registry.destroy(UI_sell.sell_text);
	}

	auto text_ent = registry.create();
	auto& ui = registry.get<UI_element>(e_button);
	// text
	float line_size = 35; // relative to the text size
	// unit name text
	std::string short_description = "$" + std::to_string(sell_price);
	auto title_text_scale = 0.4f;
	auto bubblegum = TextFont::load("data/fonts/MagicalMystery/MAGIMT__.ttf");
	// place title text at the top
	float top_margin = 45;
	auto y_title_offset = ui.scale.y / 2 - title_text_scale * line_size - top_margin;
	vec2 title_text_position = get_center_text_position(vec2(2 * ui.scale.x / 3, ui.scale.y), vec2(ui.position.x + ui.scale.x / 8, ui.position.y), title_text_scale, short_description);
	auto& title = registry.emplace_or_replace<Text>(text_ent, Text(short_description, bubblegum, vec2(title_text_position.x, title_text_position.y + y_title_offset)));
	title.scale = title_text_scale;
	title.colour = { 0.f, 0.f, 0.f };

	UI_sell.sell_text = text_ent;
}

// remove upgrade button and sell button
void remove_selected_unit_buttons()
{
	for (auto entity : registry.view<UI_selected_unit, UI_element, ShadedMeshRef>())
	{
		auto& selected_components = registry.get<UI_selected_unit>(entity);
		for (auto component : selected_components.button_components) {
			registry.destroy(component);
		}
		registry.destroy(entity);
	}

	for (auto entity : registry.view<UI_sell_button>())
	{
		auto& UI_sell = registry.get<UI_sell_button>(entity);
		if (registry.valid(UI_sell.sell_text))
		{
			registry.destroy(UI_sell.sell_text);
		}
		registry.destroy(entity);
	}
}

// update the appearance of ui depending on the given flags
void WorldSystem::update_look_for_selected_buttons(int action, bool sell_clicked)
{
	// prevent this function gets called twice with one mouse click (press & release)
	if (action != GLFW_PRESS)
	{
		return;
	}
	// clear unit stats text
	for (auto entity : registry.view<UI_unit_stats>())
	{
		registry.destroy(entity);
	}

	auto view_ui_build_buttons = registry.view<UI_build_unit>();

	if (registry.valid(selected_range_circle))
		registry.destroy(selected_range_circle);
	
	// if a unit is selected and the sell button is not clicked
	// show upgrade buttons and sell button
	if (unit_selected && !sell_clicked)
	{
		auto view_unit = registry.view<Unit>();
		auto view_selectable = registry.view<Selectable>();
		// get the selected unit
		Unit selected_unit;
		bool destroy_flag_1 = false;
		bool destroy_flag_2 = false;
		for (auto entity : view_selectable)
		{
			if (view_selectable.get<Selectable>(entity).selected)
			{
				selected_unit = view_unit.get<Unit>(entity);
				selected_view_change = previous_selected != entity || selected_view_change;
				previous_selected = entity;
				auto& motion = registry.get<Motion>(entity);
				selected_range_circle = RangeCircle::createRangeCircle(motion.position, selected_unit.attack_range);
				destroy_flag_1 = (registry.has<SnowMachine>(entity) || registry.has<Exterminator>(entity)) && selected_unit.path_2_upgrade > 0;
				destroy_flag_2 = registry.has<Wall>(entity) || registry.has<GreenHouse>(entity) ||
					((registry.has<SnowMachine>(entity) || registry.has<Exterminator>(entity)) && selected_unit.path_1_upgrade > 0);
			}
		}

		if (selected_view_change) {
			remove_selected_unit_buttons();

			upgrade_button_1 = UI_selected_unit::createUI_selected_unit_upgrade_button(2, upgrade_path_1_button, PATH_1_UPGRADE_BUTTON_TITLE, selected_unit.type, selected_unit.path_1_upgrade);
			upgrade_button_2 = UI_selected_unit::createUI_selected_unit_upgrade_button(3, upgrade_path_2_button, PATH_2_UPGRADE_BUTTON_TITLE, selected_unit.type, selected_unit.path_2_upgrade);
			button_sell = UI_sell_button::createUI_sell_button(4, sell_button, SELL_BUTTON_TITLE);
			selected_view_change = false;

			//update unit portrait
			update_unit_portrait(selected_unit);

		}

		if (registry.valid(upgrade_button_1))
		{
			bool highlight_flag_button_1 = registry.has<HighlightBool>(upgrade_button_1) && selected_unit.path_1_upgrade >= 3;
			update_selected_button(upgrade_button_1, selected_unit, highlight_flag_button_1, destroy_flag_1);
		}
		
		if (registry.valid(upgrade_button_2))
		{
			bool highlight_flag_button_2 = registry.has<HighlightBool>(upgrade_button_2) && selected_unit.path_2_upgrade >= 3;
			update_selected_button(upgrade_button_2, selected_unit, highlight_flag_button_2, destroy_flag_2);
		}

		update_sell_button_text(button_sell, selected_unit.sell_price);

		
		// update unit stats
		update_unit_stats(selected_unit);

		// hide all build buttons
		for (auto entity : view_ui_build_buttons)
		{
			RenderSystem::hide_entity(entity);
		}
		
		if (game_mode == SANDBOX) {
			RenderSystem::hide_entity(max_mobs_text_entity);
			RenderSystem::hide_entity(mob_speed_text_entity);
		}
	}
	else
	{
		selected_view_change = true;
		remove_selected_unit_buttons();
		if (player_state == PlayerState::set_up_stage || game_mode != NORMAL)
		{
			// show build unit buttons
			for (auto entity : view_ui_build_buttons)
			{
				RenderSystem::show_entity(entity);
			}

			if (game_mode == SANDBOX) {
				RenderSystem::show_entity(max_mobs_text_entity);
				RenderSystem::show_entity(mob_speed_text_entity);
			}
		}
	}
}

// mouse click callback function
void WorldSystem::on_mouse_click(int button, int action, int mod)
{
	if (!(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS))
		return;

	//getting cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (game_tips) {
		remove_game_tip_and_story_card();
	}
  
	switch (game_state)
	{
        case start_menu:
        {
            start_menu_click_handle(xpos, ypos, button, action, mod);
            break;
        }
        case settings_menu:
        {
            settings_menu_click_handle(xpos, ypos, button, action, mod);
            break;
        }
        case in_game:
        {
			vec2 selected_flags = on_click_select_unit(xpos, ypos, button, action, mod);
			in_game_click_handle(xpos, ypos, button, action, mod);
			update_look_for_selected_buttons(action, selected_flags.y);
            break;
        }
		case paused:
		{
			paused_click_handle(xpos, ypos, button, action, mod);
			break;
		}
        case help_menu:
        {
            help_menu_click_handle(xpos, ypos, button, action, mod);
            break;
        }
        case story_card:
        {
            story_card_click_handle(xpos, ypos, button, action, mod);
            break;
        }
		case lost_game_screen:
		{
			lost_game_click_handle(xpos, ypos, button, action, mod);
			break;
		}
		case victory_screen:
		{
			victory_screen_click_handle(xpos, ypos, button, action, mod);
			break;
		}
			
	}
}

void WorldSystem::help_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	if (action == GLFW_PRESS)
	{
		auto view = registry.view<Menu, ShadedMeshRef>();
		for (auto entity : view)
		{
			RenderSystem::hide_entity(entity);
		}
		
		/*if (world_round_number <= 0) {
			game_state = story_card;
			StoryCard story_card(STORY_TEXT_PER_LEVEL[world_round_number + 1], std::to_string(1));
			TalkyBoi::createTalkyBoiEntt();
		}*/

		/*if (registry.empty<StoryCardBase>()) {
			game_state = in_game;
		}
		else {
			game_state = story_card;
		}*/
		un_highlight();
		game_state = in_game;
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)mouse_pos_x;
	(void)mouse_pos_y;
	(void)button;
	(void)mod;
}

void WorldSystem::story_card_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	if (action == GLFW_PRESS)
	{
		remove_game_tip_and_story_card();
		game_state = in_game;
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)mouse_pos_x;
	(void)mouse_pos_y;
	(void)button;
	(void)mod;
}

void WorldSystem::remove_game_tip_and_story_card()
{
	auto tip_card_view = registry.view<TipCard>();
	for (auto entity : tip_card_view)
	{
		registry.destroy(entity);
	}
	for (auto entity : registry.view<StoryCardBase>())
	{
		registry.destroy(entity);
	}
	for (auto& talky_boi : registry.view<TalkyBoi>())
	{
		registry.destroy(talky_boi);
	}
	for (auto entity : registry.view<StoryCardText>())
	{
		registry.destroy(entity);
	}
}

// helper for unit_select_click_handle
bool check_unit_already_selected()
{
	auto view_selectable = registry.view<Selectable>();
	for (auto [entity, selectable] : view_selectable.each())
	{
		if (selectable.selected)
		{
			return true;
		}
	}
	return false;
}

// helper for unit_select_click_handle
bool check_click_on_sell_button(double mouse_pos_x, double mouse_pos_y)
{
	auto view_selected_buttons = registry.view<UI_sell_button, UI_element>();
	for (auto [entity, ui_selected_unit, ui_element] : view_selected_buttons.each())
	{
		if (ui_element.tag == SELL_BUTTON_TITLE)
		{
			if (sdBox({ mouse_pos_x, mouse_pos_y }, ui_element.position, ui_element.scale / 2.0f) < 0.0f)
			{
				return true;
			}
		}
	}
	return false;
}

// helper for unit_select_click_handle
// return true if click on unit modification buttons, except the sell button
bool check_click_on_unit_selected_buttons(double mouse_pos_x, double mouse_pos_y)
{
	auto view_selected_buttons = registry.view<UI_selected_unit, UI_element>();
	for (auto [entity, ui_selected_unit, ui_element] : view_selected_buttons.each())
	{
		if (sdBox({ mouse_pos_x, mouse_pos_y}, ui_element.position, ui_element.scale / 2.0f) < 0.0f)
		{
			if (ui_element.tag != SELL_BUTTON_TITLE)
				return true;
		}
	}
	return false;
}

// helper for unit_select_click_handle
// set the unit to selected
// return true if a unit is selected; otherwise, false
bool WorldSystem::click_on_unit(double mouse_pos_x, double mouse_pos_y)
{
	auto view_highlight = registry.view<HighlightBool>();
	auto view_unit = registry.view<Unit>();
	vec2 mouse_pos = mouse_in_world_coord({ mouse_pos_x, mouse_pos_y });
	
	if (mouse_in_game_area(mouse_pos)) {

		auto node = current_map.getNodeAtCoord(pixel_to_coord(mouse_pos));
		// check if clicked on egg
		if (egg == node.occupying_entity) {
			if (world_round_number < 16) {
				round_skipped = true;
				skip_to_final_round();
			}
			return false;
		}
		
		auto view_selectable = registry.view<Selectable, Motion>();
		for (auto [entity, selectable, motion] : view_selectable.each())
		{
			// remove selected status on all other units
			selectable.selected = false;
			view_highlight.get<HighlightBool>(entity).highlight = false;
		}

		if (registry.valid(node.occupying_entity)) {
			view_selectable.get<Selectable>(node.occupying_entity).selected = true;
			view_highlight.get<HighlightBool>(node.occupying_entity).highlight = true;
			return true;
		}
	}

	return false;
}

vec2 WorldSystem::on_click_select_unit(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	if (action != GLFW_PRESS)
	{
		return { false, false };
	}

	unit_selected = false;
	bool sell_clicked = false;

	if (check_unit_already_selected())
	{
		bool on_selected_buttons = check_click_on_unit_selected_buttons(mouse_pos_x, mouse_pos_y);
		bool on_sell_button = check_click_on_sell_button(mouse_pos_x, mouse_pos_y);
		unit_selected = (on_selected_buttons || on_sell_button);
		sell_clicked = on_sell_button;
	}
	
	if (!unit_selected)
	{
		unit_selected = click_on_unit(mouse_pos_x, mouse_pos_y);
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)button;
	(void)mod;
	return { unit_selected, sell_clicked };
}

void WorldSystem::start_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	MenuButtonType button_tag;
	
	if (action == GLFW_PRESS)
	{
		/*for (auto entity : registry.view<TitleEyes>())
		{
			registry.destroy(entity);
		}
		std::cout << "mouse pos :" << mouse_pos_x << ", " << mouse_pos_y << "\n";
		TitleEyes::createTitleEyes(vec2({ mouse_pos_x, mouse_pos_y }));*/
		//MenuButton::create_button(mouse_pos_x, mouse_pos_y, MenuButtonType::exit_button, "X");
		button_tag = on_click_button({mouse_pos_x, mouse_pos_y});

		switch (button_tag)
		{
        case (MenuButtonType::survival_mode_button):
            game_mode = SURVIVAL;
            show_path = show_path_duration_ms;
			remove_menu_buttons();
			restart_with_save();
			break;
		case (MenuButtonType::sandbox_button):
            game_mode = SANDBOX;
			remove_menu_buttons();
			restart_with_save();
			break;
		case (MenuButtonType::new_game_button):
//			game_state = help_menu;
//          RenderSystem::show_entity(help_menu_entity);
            game_mode = NORMAL;
			remove_menu_buttons();
			restart_with_save();
			break;
		case (MenuButtonType::load_game_button):
			remove_menu_buttons();
			restart();
			load_game();
			game_state = story_card;
			break;
		case (MenuButtonType::title_help_button):
			remove_menu_buttons();
			registry.destroy(title_button_highlight_entity);
			game_state = settings_menu;
			create_controls_menu();
			break;
		case (MenuButtonType::title_exit_button):
			// close window
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		}
	}	
	// avoid 'unreferenced formal parameter' warning message
	(void)button;
	(void)mod;
}

void WorldSystem::settings_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	MenuButtonType menu_button;
	if (action == GLFW_PRESS)
	{
		menu_button = on_click_button({mouse_pos_x, mouse_pos_y});
		switch (menu_button)
		{
		case MenuButtonType::back_button:
			// close window
			remove_menu_buttons();
			auto view = registry.view<Menu>();
			for (auto entity : view)
			{
				registry.destroy(entity);
			}
			auto menu_text_view = registry.view<MenuText>();
			for (auto entity : menu_text_view)
			{
				registry.destroy(entity);
			}
			game_state = start_menu;
			create_start_menu();
			break;
		}
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)button;
	(void)mod;
}

void WorldSystem::remove_menu_buttons()
{
	auto view = registry.view<MenuButton>();
	for (auto entity : view)
	{
		registry.destroy(entity);
	}
}

void WorldSystem::game_setup()
{
	registry.clear();
	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);
	camera = Camera::createCamera();
}

void WorldSystem::create_start_menu()
{
	std::cout << "In Start Menu\n";
	// background
	Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, "start_menu", Menu_texture::title_screen, LAYER_MAP, {1.0, 0.9});
	// foreground trees
	Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, "foreground_trees", Menu_texture::foreground_trees, LAYER_MAP + 1, { 1.0, 0.9 });
	// sign post
	Menu::createMenu(WINDOW_SIZE_IN_PX.x / 4, WINDOW_SIZE_IN_PX.y / 2 + 100, "sign_post", Menu_texture::sign_post, LAYER_MENU + 2, { 1.0, 0.9 });
	// 2nd sign post
	Menu::createMenu(WINDOW_SIZE_IN_PX.x * 3 / 4 + 60, WINDOW_SIZE_IN_PX.y / 2 + 80, "sign_post", Menu_texture::sign_post, LAYER_MENU + 2, { -0.5, 0.55 });
	// title: Feast or Famine
	Menu::createMenu(300, 150, "title_screen_title2", Menu_texture::title_screen_title2, LAYER_MAP + 3, { 1.1, 1.1 });
	Menu::createMenu(470, 120, "title_screen_title_or", Menu_texture::title_screen_title2_or, LAYER_MAP + 3, { 0.7, 0.7 });
	
	//buttons
	MenuButton::create_button(NEW_GAME_BUTTON_X, NEW_GAME_BUTTON_Y, MenuButtonType::new_game_button, "", { 1.2f, 1.2f }, TITLE_HELP_BUTTON_ANGLE);
	MenuButton::create_button(LOAD_GAME_BUTTON_X, LOAD_GAME_BUTTON_Y, MenuButtonType::load_game_button, "", { 1.2f, 1.2f }, PI + TITLE_HELP_BUTTON_ANGLE);
	MenuButton::create_button(SURVIVAL_MODE_BUTTON_X, SURVIVAL_MODE_BUTTON_Y, MenuButtonType::survival_mode_button, "", { 1.2f, 1.2f }, TITLE_HELP_BUTTON_ANGLE);
	MenuButton::create_button(SANDBOX_BUTTON_X, SANDBOX_BUTTON_Y, MenuButtonType::sandbox_button, "", { 1.2f, 1.2f }, PI + TITLE_HELP_BUTTON_ANGLE);
	
	MenuButton::create_button(TITLE_HELP_BUTTON_X, TITLE_HELP_BUTTON_Y, MenuButtonType::title_help_button, "", { 0.8f, 0.8f }, PI - TITLE_HELP_BUTTON_ANGLE);
	MenuButton::create_button(TITLE_EXIT_BUTTON_X, TITLE_EXIT_BUTTON_Y, MenuButtonType::title_exit_button, "", { 0.8f, 0.8f }, -TITLE_HELP_BUTTON_ANGLE);

	// button text
	registry.get<UI_element>(Menu::createMenu(NEW_GAME_BUTTON_X, NEW_GAME_BUTTON_Y, "new_game_button_text", "menu_button/new_game_text.png", LAYER_MENU + 4, { 0.7, 0.7 })).angle = TITLE_HELP_BUTTON_ANGLE;
	registry.get<UI_element>(Menu::createMenu(LOAD_GAME_BUTTON_X, LOAD_GAME_BUTTON_Y, "load_game_button_text", "menu_button/load_game_text.png", LAYER_MENU + 4, { 0.7, 0.7 })).angle = TITLE_HELP_BUTTON_ANGLE;
	registry.get<UI_element>(Menu::createMenu(SURVIVAL_MODE_BUTTON_X, SURVIVAL_MODE_BUTTON_Y, "survival_mode_text", "menu_button/survival_mode_text.png", LAYER_MENU + 4, { 0.7, 0.7 })).angle = TITLE_HELP_BUTTON_ANGLE;
	registry.get<UI_element>(Menu::createMenu(SANDBOX_BUTTON_X, SANDBOX_BUTTON_Y, "sandbox_text", "menu_button/sandbox_text.png", LAYER_MENU + 4, { 0.7, 0.7 })).angle = TITLE_HELP_BUTTON_ANGLE;

	registry.get<UI_element>(Menu::createMenu(TITLE_HELP_BUTTON_X, TITLE_HELP_BUTTON_Y, "help_text", "menu_button/help_text.png", LAYER_MENU + 4, { 0.5, 0.5 })).angle = -TITLE_HELP_BUTTON_ANGLE;
	registry.get<UI_element>(Menu::createMenu(TITLE_EXIT_BUTTON_X, TITLE_EXIT_BUTTON_Y, "exit_text", "menu_button/exit_text.png", LAYER_MENU + 4, { 0.5, 0.5 })).angle = -TITLE_HELP_BUTTON_ANGLE;

	title_button_highlight_entity = MenuButton::create_button_arrow();
	// blinking eyes
	std::vector<vec2> locations = { vec2({984, 452}), vec2({891, 439}), vec2({851, 437}), vec2({764, 444}), vec2({719, 448}),
								   vec2({596, 445}), vec2({531, 442}), vec2({397, 431}), vec2({355, 432}), vec2({40, 430}) };
	for (vec2 position : locations)
	{
		TitleEyes::createTitleEyes(position);
	}
	screen_sprite->effect.load_from_file(shader_path("water") + ".vs.glsl", shader_path("water") + ".fs.glsl");
	// set title screen music
	music_state = MusicState::title_screen_music;
}

std::string add_quotation_marks(std::string hotkey)
{
	return "\"" + hotkey + "\"";
}

void WorldSystem::create_controls_menu()
{
	std::cout << "In Controls Menu\n";
	int menu_layer = LAYER_MENU + 5;
	std::string menu_name = "controls";
	auto menu = Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, menu_name, Menu_texture::controls, menu_layer, { WINDOW_SIZE_IN_PX.x / 10, WINDOW_SIZE_IN_PX.x / 10});
	// title text
	std::string title_text = "Hotkeys";
	auto title_text_scale = 1.2f;
	auto title_x_offset = (title_text.length() * title_text_scale * 27) / 2;
	auto notoRegular = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
	auto& title = registry.emplace<Text>(menu, Text(title_text, notoRegular, vec2(WINDOW_SIZE_IN_PX.x / 2 - title_x_offset, WINDOW_SIZE_IN_PX.y - 170)));
	title.scale = title_text_scale;
	title.colour = { 1.0f, 0.8f, 0.0f };
	// hotkey text
	std::vector<std::string> hotkey_list = {"1", "2", "3", "4", "5", "6", "7", "P", "Space + mouse", "Scroll", "H", "Esc"};
	int para_y_offset = 230;
	int hotkey_para_x_offset = 410;
	auto hotkey_text_scale = 0.6f;
	for (int i = 0; i < hotkey_list.size(); i++) {
		auto entity = registry.create();
		auto& menu_text = registry.emplace<MenuText>(entity);
		menu_text.menu_name = menu_name;
		int y_offset = para_y_offset + (int)(i * hotkey_text_scale * 60);
		std::string hotkey_text = add_quotation_marks(hotkey_list[i]);
		auto x_offset = (hotkey_text.length() * hotkey_text_scale * 27);
		auto& hotkey = registry.emplace<Text>(entity, Text(hotkey_text, notoRegular, vec2(hotkey_para_x_offset - x_offset, WINDOW_SIZE_IN_PX.y - y_offset)));
		hotkey.scale = hotkey_text_scale;
		hotkey.colour = { 1.0f, 0.8f, 0.0f };
	}
	// hotkey description text
	std::vector<std::string> hotkey_des_list = { "Select hunter", "Select exterminator", "Select robot", "Select priestess", "Select snowmachine",
												"Select greenhouse", "Select wall", "Pause game", "Move camera", "Zoom in / out", "Show hotkeys menu", "Return to title screen" };
	int des_para_x_offset = 570;
	for (int i = 0; i < hotkey_list.size(); i++) {
		auto entity = registry.create();
		auto& menu_text = registry.emplace<MenuText>(entity);
		menu_text.menu_name = menu_name;
		int y_offset = para_y_offset + (int)(i * hotkey_text_scale * 60);
		std::string hotkey_text = hotkey_des_list[i];
		auto& description = registry.emplace<Text>(entity, Text(hotkey_text, notoRegular, vec2(des_para_x_offset, WINDOW_SIZE_IN_PX.y - y_offset)));
		description.scale = hotkey_text_scale;
		description.colour = { 1.0f, 0.8f, 0.0f };
	}
	// arrows
	for (int i = 0; i < hotkey_list.size(); i++) {
		auto arrow = registry.create();
		ShadedMesh& resource = cache_resource("control_arrow");
		if (resource.effect.program.resource == 0) {
			resource = ShadedMesh();
		}
		RenderSystem::createSprite(resource, menu_texture_path("control_arrow.png"), "textured");
		auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(arrow, resource);
		shaded_mesh_ref.layer = menu_layer + 1;
		UI_element& ui_element = registry.emplace<UI_element>(arrow);
		ui_element.tag = "control_arrow";
		ui_element.scale = vec2({ 1.0f, 1.0f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
		int y_offset = para_y_offset + (int)(i * hotkey_text_scale * 60) - 10;
		ui_element.position = vec2(hotkey_para_x_offset + (des_para_x_offset - hotkey_para_x_offset) / 2, y_offset);
		auto& menu_text = registry.emplace<MenuText>(arrow);
		menu_text.menu_name = menu_name;
	}

	registry.get<ShadedMeshRef>(MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 6 / 7, MenuButtonType::back_button, "back")).layer = menu_layer + 1;
}

void WorldSystem::on_click_ui_when_selected(Button ui_button)
{
	auto view_selectable = registry.view<Selectable>();
	entt::entity entity_selected;
	Unit unit;

	for (auto entity : view_selectable)
	{
		if (view_selectable.get<Selectable>(entity).selected)
		{
			entity_selected = entity;
			unit = registry.get<Unit>(entity);
		}
	}
	
	if (ui_button == Button::sell_button)
	{
        if (unit.type == PRIESTESS) {
            for (auto e_aura : registry.view<Aura>()) {
                auto& aura = registry.get<Aura>(e_aura);
                if (aura.emitter == entity_selected) {
                    registry.destroy(e_aura);
                }
            }
        }
		health += unit.sell_price;
		sell_unit(entity_selected);
    }
	else if (ui_button == Button::upgrade_path_1_button && health >= unit.upgrade_path_1_cost)
	{
		upgrade_unit_path_1(entity_selected);
		auto& UIselection = registry.get<UI_selected_unit>(upgrade_button_1);
		UIselection.path_num += 1;
		mouse_hover_ui_button();
	}
	else if (ui_button == Button::upgrade_path_2_button && health >= unit.upgrade_path_2_cost)
	{
		upgrade_unit_path_2(entity_selected);
		auto& UIselection = registry.get<UI_selected_unit>(upgrade_button_2);
		UIselection.path_num += 1;
		mouse_hover_ui_button();
	}
}

void WorldSystem::on_click_ui_general_buttons(Button ui_button)
{
	if (ui_button == Button::start_button)
	{
		if (player_state == set_up_stage)
		{
			start_round();
		}
	}
	else if (ui_button == Button::more_options_button)
	{
		pause_game();
		more_options_menu();
	}
	else if (ui_button == Button::tips_button)
	{
		game_tips = !game_tips;
	}
	else if (ui_button == Button::wantedboard_button)
	{
		UI_button::wantedboard_update_off(wanted_board_button);
		auto board_meshref = registry.get<ShadedMeshRef>(wanted_board_entity);
		WantedBoard::updateWantedBoardDisplay(wanted_board_entity, !board_meshref.show);
		play_sound("ui/button_click/wanted_board_button_click_2.wav");
	}
	else if (ui_button == Button::add_monster_button)
	{
		std::cout << "more mobs" << std::endl;
		max_mobs++;
		max_boss++;

		auto& max_mobs_text = registry.get<Text>(max_mobs_text_entity);
		max_mobs_text.content = std::to_string(max_mobs);
	}
	else if (ui_button == Button::rem_monster_button)
	{
		if (max_mobs > 1 && max_boss > 1) {
			std::cout << "less mobs" << std::endl;
			max_mobs--;
			max_boss--;

			auto& max_mobs_text = registry.get<Text>(max_mobs_text_entity);
			max_mobs_text.content = std::to_string(max_mobs);
		}
	}
	else if (ui_button == Button::inc_m_speed_button)
	{
		if (mob_delay_ms > 100 && boss_delay_ms > 100) {
			std::cout << "increased" << std::endl;
			mob_delay_ms -= 100;
			boss_delay_ms -= 100;

			auto& mob_speed_text = registry.get<Text>(mob_speed_text_entity);
			float monster_speed = 1000.f / (float)mob_delay_ms;
			mob_speed_text.content = std::to_string(monster_speed);
		}
	}
	else if (ui_button == Button::dec_m_speed_button)
	{
		std::cout << "delayed" << std::endl;
		mob_delay_ms += 100;
		boss_delay_ms += 100;

		auto& mob_speed_text = registry.get<Text>(mob_speed_text_entity);
		float monster_speed = 1000.f / (float)mob_delay_ms;
		mob_speed_text.content = std::to_string(monster_speed);
	}
	else if (ui_button == Button::randomize_grid_map)
	{
		std::cout << "asdf" << std::endl;
        for (auto unit : registry.view<Unit>()) {
            registry.destroy(unit);
        }
        for (auto grid_node : registry.view<GridNode>()) {
            registry.destroy(grid_node);
        }
		for (auto grid_map : registry.view<GridMap>()) {
			registry.destroy(grid_map);
		}
		current_map = registry.get<GridMap>(GridMap::createGridMap());
		AISystem::MapAI::setRandomMapWeatherTerrain(current_map, weather);

		set_AI_paths = false;
		set_default_paths();
	}
    else if (game_mode == SANDBOX && ui_button == Button::season_button)
    {
        season = mod((float)season + 1, (float)4);
        world_season_str = season_str.at(season);
        update_weather_season_UI(0);
        set_round_monsters();
        std::cout << world_season_str << " season up\n";
    }
    else if (game_mode == SANDBOX && ui_button == Button::weather_button)
    {
        weather = mod((float)weather +  1, (float)5);
        AISystem::MapAI::setRandomMapWeatherTerrain(current_map, weather);
        for (auto particle : registry.view<ParticleSystem>()) {
            registry.destroy(particle);
        }
        next_particle_spawn = 0;
        update_weather_season_UI(0);
        std::cout << weather_str.at(weather) << " weather up\n";
    }
}

void WorldSystem::on_click_ui(Button ui_button)
{
	int ui_tip_offset = GRID_CELL_SIZE + BUTTON_GAP;
	if (ui_button == Button::green_house_button)
	{
		if (game_tips && tip_manager.greenhouse_tip)
		{
			game_state = paused;
			WorldSystem::tip_manager.greenhouse_tip = false;
			TipCard::createTipCard(TIP_CARD_LEFT_X + 5 * ui_tip_offset - 5, TIP_CARD_CENBOT_Y, greenhouse_tips);
		}

		placement_unit_selected = GREENHOUSE;
		create_unit_indicator = GreenHouse::createGreenHouse;
	}
	else if (ui_button == Button::hunter_button)
	{
		if (game_tips && tip_manager.hunter_tip)
		{
			game_state = paused;
			WorldSystem::tip_manager.hunter_tip = false;
			TipCard::createTipCard(TIP_CARD_LEFT_X + 0 * ui_tip_offset - 5, TIP_CARD_CENBOT_Y, hunter_tips);
		}

		placement_unit_selected = HUNTER;
		create_unit_indicator = Hunter::createHunter;
	}
	else if (ui_button == Button::exterminator_button)
	{
		if (game_tips && tip_manager.exterminator_tip)
		{
			game_state = paused;
			WorldSystem::tip_manager.exterminator_tip = false;
			TipCard::createTipCard(TIP_CARD_LEFT_X + 1 * ui_tip_offset - 5, TIP_CARD_CENBOT_Y, exterminator_tips);
		}

		placement_unit_selected = EXTERMINATOR;
		create_unit_indicator = Exterminator::createExterminator;
	}
	else if (ui_button == Button::robot_button)
	{
		if (game_tips && tip_manager.robot_tip)
		{
			game_state = paused;
			WorldSystem::tip_manager.robot_tip = false;
			TipCard::createTipCard(TIP_CARD_LEFT_X + 2 * ui_tip_offset - 5, TIP_CARD_CENBOT_Y, robot_tips);
		}

		placement_unit_selected = ROBOT;
		create_unit_indicator = Robot::createRobot;
	}
	else if (ui_button == Button::priestess_button)
	{
		if (game_tips && tip_manager.priestess_tip)
		{
			game_state = paused;
			WorldSystem::tip_manager.priestess_tip = false;
			TipCard::createTipCard(TIP_CARD_LEFT_X + 3 * ui_tip_offset - 5, TIP_CARD_CENBOT_Y, priestess_tips);
		}

		placement_unit_selected = PRIESTESS;
		create_unit_indicator = Priestess::createPriestess;
	}
	else if (ui_button == Button::snowmachine_button)
	{
		if (game_tips && tip_manager.snowmachine_tip)
		{
			game_state = paused;
			WorldSystem::tip_manager.snowmachine_tip = false;
			TipCard::createTipCard(TIP_CARD_LEFT_X + 4 * ui_tip_offset - 5, TIP_CARD_CENBOT_Y, snowmachine_tips);
		}

		placement_unit_selected = SNOWMACHINE;
		create_unit_indicator = SnowMachine::createSnowMachine;
	}
	else if (ui_button == Button::wall_button)
	{
		if (game_tips && tip_manager.wall_tip)
		{
			game_state = paused;
			WorldSystem::tip_manager.wall_tip = false;
			TipCard::createTipCard(TIP_CARD_LEFT_X + 6 * ui_tip_offset - BUTTON_GAP, TIP_CARD_CENBOT_Y, wall_tips);
		}
		create_unit_indicator = Wall::createWall;
		placement_unit_selected = WALL;
	}
	else
	{
		placement_unit_selected = NONE;
		on_click_ui_general_buttons(ui_button);
		on_click_ui_when_selected(ui_button);
	}
}

void WorldSystem::in_game_click_handle(double xpos, double ypos, int button, int action, int mod)
{
	Motion camera_motion = registry.get<Motion>(camera);

	// cursor position in world pos
	vec2 mouse_world_pos = mouse_in_world_coord(vec2({xpos, ypos}));

	Button ui_button = UI_click_system(); // returns enum of button pressed or no_button_pressed enum

	bool in_game_area = mouse_in_game_area(vec2(xpos, ypos));

	if (player_state == set_up_stage || game_mode != NORMAL)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && in_game_area)
			WantedBoard::updateWantedBoardDisplay(wanted_board_entity, false);
		
		// Mouse click for placing units
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && placement_unit_selected != NONE && in_game_area)
		{
			auto &node = current_map.getNodeAtCoord(pixel_to_coord(mouse_world_pos));
			vec2 unit_position = coord_to_pixel(node.coord);
            bool can_place_unit = true;
            entt::entity entity;
			if (node.occupancy == NONE && node.terrain != TERRAIN_PUDDLE && node.terrain != TERRAIN_FIRE)
			{
				if (placement_unit_selected == HUNTER && health >= hunter_unit.cost)
				{
                    entity = Hunter::createHunter(unit_position);
					auto& sound = registry.emplace<SoundRef>(entity);
					sound.file_path = "ui/tower_built_sound/hunter_built_sound.wav";
					deduct_health(hunter_unit.cost);
				}
				else if (placement_unit_selected == GREENHOUSE && health >= greenhouse_unit.cost)
				{
					entity = GreenHouse::createGreenHouse(unit_position);
					auto& sound = registry.emplace<SoundRef>(entity);
					sound.file_path = "ui/tower_built_sound/greenhouse_built_sound.wav";
					deduct_health(greenhouse_unit.cost);
				}
				else if (placement_unit_selected == EXTERMINATOR && health >= exterminator_unit.cost)
				{
					entity = Exterminator::createExterminator(unit_position);
					auto& sound = registry.emplace<SoundRef>(entity);
					sound.file_path = "ui/tower_built_sound/exterminator_built_sound.wav";
					deduct_health(exterminator_unit.cost);
				}
				else if (placement_unit_selected == ROBOT && health >= robot_unit.cost)
				{
					entity = Robot::createRobot(unit_position);
					auto& sound = registry.emplace<SoundRef>(entity);
					sound.file_path = "ui/tower_built_sound/robot_built_sound.wav";
					deduct_health(robot_unit.cost);
				}
				else if (placement_unit_selected == PRIESTESS && health >= priestess_unit.cost)
				{
					entity = Priestess::createPriestess(unit_position);
					auto& unit = registry.get<Unit>(entity);
                    Aura::createAura(unit_position, unit.attack_range, entity);
					auto& sound = registry.emplace<SoundRef>(entity);
					sound.file_path = "ui/tower_built_sound/priestess_built_sound.wav";
					deduct_health(priestess_unit.cost);
				}
				else if (placement_unit_selected == SNOWMACHINE && health >= snowmachine_unit.cost)
				{
					entity = SnowMachine::createSnowMachine(unit_position);
					auto& sound = registry.emplace<SoundRef>(entity);
					sound.file_path = "ui/tower_built_sound/snowmachine_built_sound.wav";
					deduct_health(snowmachine_unit.cost);
				}
				else if (placement_unit_selected == WALL && health >= wall_unit.cost)
				{
					entity = Wall::createWall(unit_position/*, false*/);
					auto& sound = registry.emplace<SoundRef>(entity);
					sound.file_path = "ui/tower_built_sound/wall_built_sound.wav";
					deduct_health(wall_unit.cost);
				}
				else
				{
					//insufficent funds -- should feedback be given here, or when the button is pressed?
					Mix_PlayChannel(-1, ui_sound_negative_tick, 0);
                    can_place_unit = false;
				}
				if (can_place_unit) {
                    Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				    auto& motion = registry.get<Motion>(entity);
                    current_map.setGridOccupancy(node.coord, placement_unit_selected, entity);
                    show_path = show_path_duration_ms;
					set_AI_paths = false;

					auto& unit = registry.get<Unit>(entity);
				}
				placement_unit_selected = NONE;

				if (registry.valid(entity_selected))
				{
					registry.destroy(entity_selected);
					registry.destroy(entity_range_circle);
				}

				un_highlight();
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !in_game_area)
		{
			on_click_ui(ui_button);
		}

		Priestess::updateBuffs();
	}
	else if (player_state == battle_stage)
	{
		if (ui_button == Button::fastforward_button)
		{
			if (speed_up_factor != 1.f)
			{
				UI_button::fastforward_light_off();
				// set speed up factor
				speed_up_factor = 1.f;
			}
			else
			{
				UI_button::fastforward_light_up();
				// set speed up factor
				speed_up_factor = FAST_SPEED;
			}
		}
		else if (ui_button == Button::more_options_button)
		{
			pause_game();
			more_options_menu();
		}
		else if (ui_button == Button::wantedboard_button)
		{
			UI_button::wantedboard_update_off(wanted_board_button);
			auto board_meshref = registry.get<ShadedMeshRef>(wanted_board_entity);
			WantedBoard::updateWantedBoardDisplay(wanted_board_entity, !board_meshref.show);
			play_sound("ui/button_click/wanted_board_button_click_2.wav");
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !in_game_area)
		{
			on_click_ui_when_selected(ui_button);
		}
		// click on health drop
		for (auto e_health_drop : registry.view<HealthOrb>())
		{
			auto health_drop_motion = registry.get<Motion>(e_health_drop);
			if (sdBox(mouse_world_pos, health_drop_motion.position, health_drop_motion.scale / 2.0f) < 0.0f) {
				auto& health_drop = registry.get<HealthOrb>(e_health_drop);
				health_drop.clicked = true;
				health_drop.starting_point = health_drop_motion.position;
			}
		}
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)mod;
}

void WorldSystem::victory_screen_click_handle(double xpos, double ypos, int button, int action, int mod)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		vec2 mouse_pos = { (float)xpos, (float)ypos };
		MenuButtonType button_clicked = on_click_button(mouse_pos);
		switch (button_clicked)
		{
		case MenuButtonType::restart_round_button:
			remove_menu_buttons();
			restart();
			load_game();
			game_state = story_card;
			break;
		case MenuButtonType::exit_button:
			game_setup();
			create_start_menu();
			player_state = set_up_stage;
			game_state = start_menu;
			break;
		}
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)mod;
}

void WorldSystem::lost_game_click_handle(double xpos, double ypos, int button, int action, int mod)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		vec2 mouse_pos = { (float)xpos, (float)ypos };
		MenuButtonType button_clicked = on_click_button(mouse_pos);
		switch (button_clicked)
		{
		case MenuButtonType::restart_round_button:
			remove_menu_buttons();
			restart();
			load_game();
			game_state = story_card;
			break;
		case MenuButtonType::exit_button:
			game_setup();
			create_start_menu();
			player_state = set_up_stage;
			game_state = start_menu;
			break;
		}
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)mod;
}

// unpause if paused
void WorldSystem::paused_click_handle(double xpos, double ypos, int button, int action, int mod)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		vec2 mouse_pos = { (float)xpos, (float)ypos };
		auto menu_ui = registry.get<UI_element>(pause_menu_entity);
		// check menu is visible and
		//click within menu
		if (registry.get<ShadedMeshRef>(pause_menu_entity).show && sdBox(mouse_pos, menu_ui.position, menu_ui.scale / 2.0f) < 0.0f)
		{
			MenuButtonType button_clicked = on_click_button(mouse_pos);
			// world_round_number get reset to 0 in restart();
			int temp_world_round_number = world_round_number;
			switch (button_clicked)
			{
			case MenuButtonType::menu_save_button:
				save_game();
				break;
			case MenuButtonType::load_game_button:
				remove_menu_buttons();
				restart();
				load_game();
				game_state = story_card;
				break;
			case MenuButtonType::restart_round_button:
				remove_menu_buttons();
				RenderSystem::hide_entity(pause_menu_entity);
				restart();
				load_game();
				game_state = story_card;
				break;
			case MenuButtonType::help_button:
				for (auto entity : registry.view<MenuButton>())
					registry.destroy(entity);
				RenderSystem::show_entity(help_menu_entity);
				game_state = GameState::help_menu;
				break;
			case MenuButtonType::exit_button:
				game_setup();
				create_start_menu();
				player_state = set_up_stage;
				game_state = start_menu;
				break;
			}
		}
		else
		{
			// remove game tips if exist
			remove_game_tip_and_story_card();
			resume_game();
			un_highlight();
		}
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)mod;
}

void WorldSystem::sell_unit(entt::entity &entity)
{
    auto& motion = registry.get<Motion>(entity);
    current_map.setGridOccupancy(pixel_to_coord(motion.position), NONE, entity);
	// ai path find reset
	set_AI_paths = false;
	// destroy the laser beam when removing robot
	if (registry.has<Robot>(entity))
	{
		for (auto e_laser : registry.view<LaserBeam>())
		{
			if (registry.get<LaserBeam>(e_laser).unit_pos == registry.get<Motion>(entity).position)
			{
				registry.destroy(e_laser);
			}
		}
	}
	// destroy the flame when removing exterminator 
	else if (registry.has<Exterminator>(entity))
	{
		for (auto e_projectile : registry.view<Flamethrower>())
		{
			auto e_unit = registry.get<Flamethrower>(e_projectile).e_unit;
			if (registry.get<Motion>(e_unit).position == registry.get<Motion>(entity).position)
			{
				registry.destroy(e_projectile);
			}
		}
	}
	// destroy the flame when removing exterminator 
	else if (registry.has<SnowMachine>(entity))
	{
		for (auto e_projectile : registry.view<IceField>())
		{
			auto e_unit = registry.get<IceField>(e_projectile).e_unit;
			if (registry.get<Motion>(e_unit).position == registry.get<Motion>(entity).position)
			{
				registry.destroy(e_projectile);
			}
		}
	}
	// destroy unit
	registry.destroy(entity);
}

void WorldSystem::save_game()
{
	nlohmann::json save_json;
	save_json["round_number"] = world_round_number;
	save_json["health"] = health;
	save_json["game_mode"] = game_mode;
	save_json["weather"] = weather;
	// save kill count
	nlohmann::json kill_count_json;
	for (auto kill_count : player_stats)
	{
		nlohmann::json one_monster;
		one_monster["monster_type"] = kill_count.first;
		one_monster["monster_killed"] = kill_count.second;
		kill_count_json.push_back(one_monster);
	}
	
	save_json["kill_count"] = kill_count_json;

	// TODO finish implementing, may need to edit unit struct
	auto view_unit = registry.view<Unit>();
	auto view_motion = registry.view<Motion>();
	auto view_selectable = registry.view<Selectable>();
	std::vector<nlohmann::json> unit_list(view_selectable.size());

	int i = 0;
	for (auto &entity : view_selectable)
	{
		nlohmann::json curr_unit;
		auto unit = view_unit.get<Unit>(entity);
		auto motion = view_motion.get<Motion>(entity);

        curr_unit["type_str"] = unit_str.at(unit.type);
		curr_unit["type"] = unit.type;
		curr_unit["x_coord"] = motion.position.x;
		curr_unit["y_coord"] = motion.position.y;
		curr_unit["path_1_upgrades"] = unit.path_1_upgrade;
		curr_unit["path_2_upgrades"] = unit.path_2_upgrade;
		curr_unit["rotate"] = unit.rotate;

		unit_list[i++] = curr_unit;
	}

	save_json["units"] = unit_list;

	std::vector<std::vector<nlohmann::json>> map;
	for (int x = 0; x < MAP_SIZE_IN_COORD.x; x++) {
		std::vector<nlohmann::json> map_row;
		for (int y = 0; y < MAP_SIZE_IN_COORD.y; y++) {
			auto& node = current_map.getNodeAtCoord(ivec2(x, y));
			nlohmann::json node_json; 
			node_json["terrain"] = node.terrain;
			node_json["occupancy"] = node.occupancy;
			map_row.push_back(node_json);
		}
		map.push_back(map_row);
	}

	save_json["map_data"] = map;

	std::ofstream file(SAVE_PATH);
	file << save_json.dump(4);
	file.close();

	// references file from build folder but requires the folder /data/save_files to be present
	// TODO: not sure how it works
	if (!file.fail())
	{
		std::cout << "Game saved!" << std::endl;
	}
	else
	{
		std::cout << "Game failed to save" << std::endl;
	}
}

void WorldSystem::load_game()
{
	// hardcoded for now
	nlohmann::json save_json = get_json(SAVE_PATH);
	if (save_json == NULL) {
		return;
	}

	health = save_json["health"];
	world_round_number = save_json["round_number"];
    game_mode = save_json["game_mode"];
	weather = save_json["weather"];
	// load monster kill count
	auto list_of_json = save_json["kill_count"];
	for (auto kill_json : list_of_json)
	{
		player_stats.at(kill_json["monster_type"]) = kill_json["monster_killed"];
	}
	setup_round_from_save_file(world_round_number, weather);

	for (nlohmann::json unit : save_json["units"])
	{
		int x = unit["x_coord"];
		int y = unit["y_coord"];
        unit_type type = unit["type"];
		entt::entity entity;
		if (type == GREENHOUSE)
		{
			entity = GreenHouse::createGreenHouse({x, y});
		}
		else if (type == WALL)
		{
			entity = Wall::createWall({x, y}/*, unit["rotate"]*/);
		}
		else if (type == HUNTER)
		{
			entity = Hunter::createHunter({x, y});
		}
		else if (type == EXTERMINATOR)
		{
			entity = Exterminator::createExterminator({ x, y });
		}
		else if (type == PRIESTESS)
		{
			entity = Priestess::createPriestess({ x, y });
		}
		else if (type == SNOWMACHINE)
		{
			entity = SnowMachine::createSnowMachine({ x, y });
		}
		else if (type == ROBOT)
		{
			entity = Robot::createRobot({ x, y });
		}
		
		auto& motion = registry.get<Motion>(entity);
        current_map.setGridOccupancy(pixel_to_coord(vec2(x,y)), type, entity);
		auto view_unit = registry.view<Unit>();
		auto &curr_unit = view_unit.get<Unit>(entity);
		
		for (int i = 0; i < unit["path_1_upgrades"]; i++) {
			auto& unit = registry.get<Unit>(entity);
			health += unit.upgrade_path_1_cost;
			upgrade_unit_path_1(entity);
		}

		for (int j = 0; j < unit["path_2_upgrades"]; j++) {
			auto& unit = registry.get<Unit>(entity);
			health += unit.upgrade_path_2_cost;
			upgrade_unit_path_2(entity);
		}
	}

	std::vector<std::vector<nlohmann::json>> map = save_json["map_data"];
	for (int x = 0; x < MAP_SIZE_IN_COORD.x; x++) {
		for (int y = 0; y < MAP_SIZE_IN_COORD.y; y++) {
			terrain_type terrain = map[x][y]["terrain"];
			current_map.setGridTerrain(ivec2(x, y), terrain);
		}
	}
}

void WorldSystem::restart_with_save() {
	restart();
	save_game();
}
