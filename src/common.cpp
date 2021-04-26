#include "common.hpp"
#include "text.hpp"
#include "rig.hpp"
#include <iostream>
#include <world.hpp>

// Note, we could also use the functions from GLM but we write the transformations here to show the uderlying math
void Transform::scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * S;
}

void Transform::rotate(float radians)
{
	float c = std::cos(radians);
	float s = std::sin(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * R;
}

void Transform::translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	mat = mat * T;
}



//detects if mouse is within the a rectangle of size scale at position entity_pos
//not easy to use right now, look at how it's used in the ui procedures.
float sdBox(vec2 mouse_pos_grid, vec2 entity_pos, vec2 scale) {
	vec2 d = abs(mouse_pos_grid - entity_pos) - scale;
	return length(max(d, vec2(0.0, 0.0))) + min(max(d.x, d.y), 0.0f);
}


entt::registry registry;

entt::entity screen_state_entity;
// for camera view; zoom & pan
entt::entity camera;

vec2 mouse_in_world_coord(vec2 mouse_pos)
{
	auto view = registry.view<Motion>();
	auto& camera_motion = view.get<Motion>(camera);
	vec2 mouse_world_pos = vec2({ (mouse_pos.x + camera_motion.position.x) / camera_motion.scale.x,
								  (mouse_pos.y + camera_motion.position.y) / camera_motion.scale.y });
	return mouse_world_pos;
}

vec2 coord_to_pixel(ivec2 grid_coord) {
    return grid_coord * GRID_CELL_SIZE + GRID_OFFSET + ivec2(0, UI_TOP_BAR_HEIGHT);
}

ivec2 pixel_to_coord(vec2 pixel_position) {
    return ((ivec2)pixel_position - ivec2(0, UI_TOP_BAR_HEIGHT)) / GRID_CELL_SIZE;
}

// convert original scale to some set multiple of cell units
vec2 scale_to_grid_units(vec2 original_scale, float cell_units, int frames) {
    vec2 scale =  abs(vec2(original_scale.x / (float)frames, original_scale.y));
    vec2 unit_scale = original_scale / max(scale.x, scale.y);
    return unit_scale * cell_units * (float) GRID_CELL_SIZE;
}

// unit_velocity: velocity as a multiple of grid cell units
// eg. unit_velocity = vec2(1,1) means moving 1 grid right and 1 grid down every second
vec2 grid_to_pixel_velocity(vec2 unit_velocity) {
    return unit_velocity * (vec2) GRID_CELL_SIZE;
}

bool is_inbounds(ivec2 grid_coord) {
    return grid_coord.x >= 0 && grid_coord.y >= 0 && grid_coord.x < MAP_SIZE_IN_COORD.x && grid_coord.y < MAP_SIZE_IN_COORD.y;
}

std::vector<vec2> bezierVelocities(std::vector<vec2> points) {
	std::vector<vec2> velocities;
	for (int i = 0; i < points.size() - 1; i++) {
		velocities.push_back(points[i + 1] - points[i]);
	}
	return velocities;
}

// total time in milliseconds
std::vector<vec2> bezierCurve(std::vector<vec2> points, float total_time) {
	size_t num_points = points.size();
	float num_frames = round(total_time / (ELAPSED_MS * WorldSystem::speed_up_factor)); 
	float step = 1 / num_frames;
	std::vector<float> coefficients = pascalNRow((int)num_points);
	
	std::vector<vec2> bezier_points; 
	for (int i = 0; i <= num_frames; i++) {
		vec2 point = { 0, 0 };
		for (int j = 0; j < num_points; j++) {
			point += coefficients[j] * (float) pow(1 - step * i, num_points - j - 1) * (float) pow(step * i, j) * points[j];
		}
		bezier_points.push_back(point);
	}
	return bezier_points;
}

std::vector<float> pascalNRow(int n) {
	std::vector<float> row;
	row.push_back(1.f);

	for (int i = 1; i < n; i++) {
		std::vector<float> next_row;
		next_row.push_back(1.f);
		for (int j = 0; j < row.size() - 1; j++) {
			next_row.push_back(row[j] + row[j + 1]);
		}
		next_row.push_back(1.f);
		row = next_row;
	}

	return row;
}

nlohmann::json get_json(std::string json_path)
{
	std::ifstream input_stream(json_path);

	if (input_stream.fail())
	{
		std::cout << "Not reading json file for path \"" + json_path + "\" \n";
		return NULL;
	}

	try {
		auto json = nlohmann::json::parse(input_stream);
		return json;
	}
	catch (std::exception) {
		return NULL;
	}
}

void Monster::setSprite(entt::entity entity){
    if (registry.has<Rig>(entity)) {
        return;
    }
    // Create the rendering components
    std::string key = sprite;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(sprite), "monster");
    }
    auto& shaded_mesh_ref = registry.get<ShadedMeshRef>(entity);
    shaded_mesh_ref.reference_to_cache = &resource;
}

Mix_Chunk* cache_chunk(std::string file_path)
{
	static std::unordered_map<std::string, Mix_Chunk*> chunk_cache;

	const auto it = chunk_cache.find(file_path);
	if (it == chunk_cache.end())
	{
		Mix_Chunk* chunk = Mix_LoadWAV(audio_path(file_path).c_str());
		const auto it_succeeded = chunk_cache.emplace(file_path, chunk);
		return it_succeeded.first->second;
	}
	return it->second;
}

void play_sound(std::string file_path)
{
	Mix_Chunk* chunk = cache_chunk(file_path);
	int channel_played = Mix_PlayChannel(-1, chunk, 0);
	// if no channels are free
	if (channel_played == -1) {
		// allocate new channels
		Mix_AllocateChannels(Mix_AllocateChannels(-1) + 1);
		channel_played = Mix_PlayChannel(-1, chunk, 0);
	}
}