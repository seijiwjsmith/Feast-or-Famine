enum mode {
    NORMAL,
    SANDBOX,
    SURVIVAL
};

enum layer_value {
    LAYER_MAP = 0,
    LAYER_UNITS = 10,
    LAYER_MONSTERS = 30,
    LAYER_DEBUG = 40,
    LAYER_PROJECTILES = 50,
    LAYER_PARTICLES = 60,
    LAYER_UI = 70,
    LAYER_MENU = 80
};

// Terrains with neg value are ones that cannot be placed on
// May be refactored later if too hard to keep track
enum terrain_type
{
    TERRAIN_DEFAULT,
    TERRAIN_PAVEMENT,
    TERRAIN_MUD,
    TERRAIN_PUDDLE,
    TERRAIN_DRY,
    TERRAIN_FIRE,
    TERRAIN_ICE,
};

const int monster_type_count = 9;

enum monster_type
{
    MOB,
    SUMMER_BOSS,
    FALL_BOSS,
    WINTER_BOSS,
    BURROW_BOSS,
    SPIDER,
    SPRING_BOSS,
    FINAL_BOSS,
    FIREBALL_BOSS,
    TALKY_BOI,
    GHOST
};

enum monster_state {
    WALK,
    ATTACK,
    SPECIAL,
};

enum unit_type
{
    NONE,
    FOREST,
    VILLAGE,
    //WATCHTOWER,
    GREENHOUSE,
    EXTERMINATOR,
    PRIESTESS,
    SNOWMACHINE,
    ROBOT,
    HUNTER,
    WALL,
    EGG
};

enum neighbor_type
{
    DIRECT_NBRS,
    DIAGONAL_NBRS,
    ALL_NBRS,
};


enum season
{
    SPRING,
    SUMMER,
    FALL,
    WINTER,
    FINAL
};

// Weather
enum weather_type
{
    CLEAR,
    RAIN ,
    DROUGHT,
    FOG ,
    SNOW,
};

// UI button
enum Button
{
    no_button_pressed,
    watchtower_button,
    green_house_button,
    exterminator_button,
    robot_button,
    priestess_button,
    snowmachine_button,
    hunter_button,
    wall_button,
    upgrade_path_1_button,
    upgrade_path_2_button,
    save_button,
    sell_button,
    start_button,
    tips_button,
    wantedboard_button,
    pause_button,
    more_options_button, 
    fastforward_button,
    add_monster_button,
    rem_monster_button,
    inc_m_speed_button,
    dec_m_speed_button,
    randomize_grid_map,
    season_button,
    weather_button,
};

// Menu button
enum MenuButtonType
{ 
    no_menu_button_pressed,
    new_game_button,
    load_game_button,
    help_button,
    title_help_button,
    exit_button,
    title_exit_button,
    empty_button,
    back_button,
    restart_round_button,
    menu_save_button,
    sandbox_button,
    survival_mode_button
};

enum class MusicState
{
    //player_prepare_music,
    //player_battle_music,
    title_screen_music,
    vicotry_screen_music,
    lost_screen_music,
    normal_round_music,
    final_round_music,
};