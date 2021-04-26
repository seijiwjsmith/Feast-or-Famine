#include <glm/vec2.hpp>				// vec2

using namespace glm;

static const int GRID_CELL_SIZE = 70;
static const ivec2 GRID_OFFSET =  ivec2(GRID_CELL_SIZE/2 , GRID_CELL_SIZE/2);
static const int UI_TOP_BAR_HEIGHT = 75;
static const int UI_TAB_HEIGHT = 88;

static const ivec2 MAP_SIZE_IN_COORD = {15, 9};
static const ivec2 MAP_SIZE_IN_PX = GRID_CELL_SIZE * MAP_SIZE_IN_COORD;
static const ivec2 WINDOW_SIZE_IN_PX = MAP_SIZE_IN_PX + ivec2(0, UI_TAB_HEIGHT + UI_TOP_BAR_HEIGHT);

static const ivec2 FOREST_COORD = ivec2(0, 0);
// offset by 1 to account for base 0 counting
static const ivec2 VILLAGE_COORD = MAP_SIZE_IN_COORD - ivec2(1, 1);
static const ivec2 DRAGON_COORD = ivec2(-2, 5);

static const float ELAPSED_MS = 20;

static const int MAX_PARTICLES = 1000;
static const float DOT_DELAY = 450; 
static const float SLOW_DELAY = 1500;

const int FIREBALL_DELAY_MS = 2385;
// rounds text 
static const int MAX_ROUND_NUMBER = 17;
// hit point text size
static const int MAX_POSSIBLE_DAMAGE = 100;
// rounds per season
static const int ROUND_PER_SEASON = 2;

static const float END_OF_BATTLE_STAGE_DELAY_MS = 1500.f;
// fastforward factor 
static const float FAST_SPEED = 2.f;