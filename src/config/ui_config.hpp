#include <map>
#include <glm/vec3.hpp>

// food label
static const float FOOD_LABEL_X_OFFSET = 30.f;
static const float FOOD_LABEL_Y_OFFSET = 60.f;
static const float FOOD_LABEL_SCALE = 0.6f;
// food num
static const float FOOD_NUM_X_OFFSET = FOOD_LABEL_X_OFFSET + 80.f;
static const float FOOD_NUM_Y_OFFSET = FOOD_LABEL_Y_OFFSET;
static const float FOOD_NUM_SCALE = 1.0f;
// round label
static const float ROUND_LABEL_X_OFFSET = (float)WINDOW_SIZE_IN_PX.x / 2.f - 100;
static const float ROUND_LABEL_Y_OFFSET = FOOD_LABEL_Y_OFFSET;
static const float ROUND_LABEL_SCALE = 0.6f;
// round number
static const float ROUND_NUM_X_OFFSET = (float)WINDOW_SIZE_IN_PX.x / 2 + 20;
static const float ROUND_NUM_Y_OFFSET = FOOD_LABEL_Y_OFFSET;
static const float ROUND_NUM_SCALE = 1.2f;


// season text colour
const std::map<int, vec3> season_str_colour = {
        {SPRING,    {0.f, 196.f / 255.f, 26.f / 255.f}},
        {SUMMER,    {1.f, 0.f, 0.f}},
        {FALL,      {1.f, 123.f / 255.f, 0.f}},
        {WINTER,    {0.f, 221.f / 255.f, 1.f}},
};
// weather text colour
const std::map<int, vec3> weather_str_colour = {
        {CLEAR,     {0.f, 221.f / 255.f, 1.f}},
        {RAIN,      {0.f, 110.f / 255.f, 1.f}},
        {DROUGHT,   {1.f, 190.f / 255.f, 92.f / 255.f}},
        {FOG,       {0.7f, 0.7f, 0.7f}},
        {SNOW,      {1.f, 1.f, 1.f}}
};
// season wheel
static const float SEASON_WHEEL_X_OFFSET = (float)WINDOW_SIZE_IN_PX.x - 300;
static const float SEASON_WHEEL_Y_OFFSET = FOOD_LABEL_Y_OFFSET - 23.f;
// weather icon
static const float WEATHER_ICON_X_OFFSET = (float)WINDOW_SIZE_IN_PX.x - 130;
static const float WEATHER_ICON_Y_OFFSET = FOOD_LABEL_Y_OFFSET - 23.f;

// season text
static const float SEASON_X_OFFSET = SEASON_WHEEL_X_OFFSET + 50.f;
static const float SEASON_Y_OFFSET = SEASON_WHEEL_Y_OFFSET + 10.f;
static const float SEASON_SCALE = 0.45f;
// weather text
static const float WEATHER_TEXT_X_OFFSET = WEATHER_ICON_X_OFFSET + 37.f;
static const float WEATHER_TEXT_Y_OFFSET = WEATHER_ICON_Y_OFFSET + 10.f;
static const float WEATHER_TEXT_SCALE = 0.45f;

// season wheel
//static const float SEASON_WHEEL_X_OFFSET = SEASON_X_OFFSET + 105.f;
//static const float SEASON_WHEEL_Y_OFFSET = SEASON_Y_OFFSET - 13.f;
//// weather icon
//static const float WEATHER_ICON_X_OFFSET = WEATHER_TEXT_X_OFFSET + 105.f;
//static const float WEATHER_ICON_Y_OFFSET = WEATHER_TEXT_Y_OFFSET - 13.f;

// Lost Screen Buttons
static const float RESTART_ROUND_BUTTON_X = 200;
static const float RESTART_ROUND_BUTTON_Y = WINDOW_SIZE_IN_PX.y * 7 / 8;
static const float EXIT_BUTTON_X = WINDOW_SIZE_IN_PX.x / 2;
static const float EXIT_BUTTON_Y = WINDOW_SIZE_IN_PX.y * 7 / 8;
static const float LOST_EXIT_BUTTON_X = WINDOW_SIZE_IN_PX.x - 200;
static const float LOST_EXIT_BUTTON_Y = WINDOW_SIZE_IN_PX.y * 7 / 8;

// Title Screen Buttons
static const float NEW_GAME_BUTTON_X = 255;
static const float NEW_GAME_BUTTON_Y = 340;
static const float NEW_GAME_BUTTON_ANGLE = -3.1415926 / 16;
static const float LOAD_GAME_BUTTON_X = 230;
static const float LOAD_GAME_BUTTON_Y = 427;
static const float SURVIVAL_MODE_BUTTON_X = 285;
static const float SURVIVAL_MODE_BUTTON_Y = 505;
static const float SANDBOX_BUTTON_X = 233;
static const float SANDBOX_BUTTON_Y = 595;

static const float TITLE_HELP_BUTTON_X = 815;
static const float TITLE_HELP_BUTTON_Y = 395;
static const float TITLE_HELP_BUTTON_ANGLE = -3.1415926 / 32;
static const float TITLE_EXIT_BUTTON_X = 850;
static const float TITLE_EXIT_BUTTON_Y = TITLE_HELP_BUTTON_Y + 70;

// tips button
static const vec2 TIPS_GAME_BUTTON_POS = { WINDOW_SIZE_IN_PX.x - 25, WINDOW_SIZE_IN_PX.y - 40 };
// add monster button
static const vec2 ADD_GAME_BUTTON_POS = { WINDOW_SIZE_IN_PX.x - 250, WINDOW_SIZE_IN_PX.y - 65 };
// rem monster button
static const vec2 REM_GAME_BUTTON_POS = { WINDOW_SIZE_IN_PX.x - 275, WINDOW_SIZE_IN_PX.y - 65 };
// increase monster speed button
static const vec2 INC_GAME_BUTTON_POS = { WINDOW_SIZE_IN_PX.x - 150, WINDOW_SIZE_IN_PX.y - 65 };
// decrease monster speed button
static const vec2 DEC_GAME_BUTTON_POS = { WINDOW_SIZE_IN_PX.x - 175, WINDOW_SIZE_IN_PX.y - 65 };
// randomize grid map button
static const vec2 RANDOM_BUTTON_POS = { WINDOW_SIZE_IN_PX.x - 350, WINDOW_SIZE_IN_PX.y - 40 };
// wantedboard button
static const vec2 WANTED_BOARD_BUTTON_POS = { WINDOW_SIZE_IN_PX.x - 100, WINDOW_SIZE_IN_PX.y - 40 };
// start battle button
static const vec2 START_BATTLE_BUTTON_POS = { 40,  WINDOW_SIZE_IN_PX.y - 40 };
// puase game button
static const vec2 PAUSE_BUTTON_POS = { 90,  WINDOW_SIZE_IN_PX.y - 40 };
// more options button
static const vec2 MORE_OPTIONS_BUTTON_POS = { 90,  WINDOW_SIZE_IN_PX.y - 40 };
// fastforward button
static const vec2 FASTFORWARD_BUTTON_POS = { 40,  WINDOW_SIZE_IN_PX.y - 40 };

// portrait of units
static const vec2 PORTRAIT_POS = { 170, WINDOW_SIZE_IN_PX.y - (UI_TAB_HEIGHT / 2) };