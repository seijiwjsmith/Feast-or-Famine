#include <string>
#include <map>

// Simple utility functions to avoid mistyping directory name
inline std::string data_path() { return "data"; };
inline std::string shader_path(const std::string& name) { return data_path() + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) { return data_path() + "/textures/" + name; };
inline std::string audio_path(const std::string& name) { return data_path() + "/audio/" + name; };
inline std::string mesh_path(const std::string& name) { return data_path() + "/meshes/" + name; };

const std::map<int, std::string> terrain_str = {
        {TERRAIN_DEFAULT,  "default"},
        {TERRAIN_PAVEMENT, "pavement"},
        {TERRAIN_MUD,      "mud"},
        {TERRAIN_PUDDLE,   "puddle"},
        {TERRAIN_DRY,   "dry"},
        {TERRAIN_FIRE,   "fire"},
        {TERRAIN_ICE,   "ice"},
};

const std::map<int, std::string> unit_str = {
        //{WATCHTOWER, "watchtower"},
        {ROBOT,      "robot"},
        {GREENHOUSE, "greenhouse"},
        {EXTERMINATOR, "exterminator"},
        {PRIESTESS, "priestess"},
        {SNOWMACHINE, "snowmachine"},
        {HUNTER,     "hunter"},
        {WALL,       "wall"},
};

const std::map<int, std::string> season_str = {
        {SPRING,    "spring"},
        {SUMMER,    "summer"},
        {FALL,      "fall"},
        {WINTER,    "winter"},
        {FINAL,     "final"}
};

const std::map<std::string, int> season_str_to_enum = {
        {"spring", SPRING},
        {"summer", SUMMER},
        {"fall", FALL},
        {"winter", WINTER},
        {"final", SUMMER}
};

const std::map<int, std::string> weather_str = {
        {CLEAR, "clear"},
        {RAIN, "rain"},
        {DROUGHT, "drought"},
        {FOG, "fog"},
        {SNOW, "snow"}
};

const std::map<MenuButtonType, std::string> menu_button_ui_tag = {
    {new_game_button,   "new_game_button"},
    {restart_round_button, "restart_round_button"},
    {load_game_button,  "load_game_button"},
    {title_help_button,   "title_help_button"},
    {title_exit_button,       "title_exit_button"},
    {exit_button,       "exit_button"},
    {help_button,       "help_button"},
    {back_button,       "back_button"},
    {menu_save_button,  "save_game_button"},
    {empty_button,      "empty_button"},
    {sandbox_button,      "sandbox_button"},
    {survival_mode_button,      "survival_mode_button"}
};

const std::string INPUT_PATH = "data/monster_rounds/";
const std::string JSON_EXTENSION = ".json";

const std::string round_voice_file_name[] = {
    "please_help_us",
    "whew_thanks(extended)",                            // boss
    "get_ready_for_summer",
    "wait_is_that_a_moose",                             // boss
    "tim/were_heading_into_fall", // 5
    "tim/prepare_for_lockdown_the_bears_are_coming",    // boss
    "tim/winter_is_coming",
    "tim/beware_the_killer_penguins",                   // boss
    "tim/hunters_rifles_out_its_hunting_time",
    "wait_whats_that_in_the_sky",                       // 10 // boss
    "tim/shhh_its_wabbit_season",
    "oh(surprised)",                                    // boss
    "tim/hunters_rifles_out_its_hunting_time",
    "oh(surprised)",                                    // boss
    "tim/shhh_its_wabbit_season",           // 15
    "oh(surprised)",                                    // boss
    "tim/prepare_for_lockdown",           // 17
};

const std::string DAVE_SPRITE = "dave_animate.png";
const std::string TIM_SPRITE = "tim_animate.png";

const std::map<int, std::string> round_talky_boi_sprite = {
    {0, DAVE_SPRITE},
    {1, DAVE_SPRITE},
    {2, DAVE_SPRITE},
    {3, DAVE_SPRITE},
    {4, TIM_SPRITE},
    {5, TIM_SPRITE},
    {6, TIM_SPRITE},
    {7, TIM_SPRITE},
    {8, TIM_SPRITE},
    {9, DAVE_SPRITE},
    {10, TIM_SPRITE},
    {11, DAVE_SPRITE},
    {12, TIM_SPRITE},
    {13, DAVE_SPRITE},
    {14, TIM_SPRITE},
    {15, DAVE_SPRITE},
    {16, TIM_SPRITE}
};