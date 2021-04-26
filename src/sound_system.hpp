#include <iostream>
#include <SDL_mixer.h>
#include <SDL.h>
#include "entt.hpp"
#include "world.hpp"
#include "common.hpp"

class SoundSystem
{
public:
	SoundSystem(WorldSystem* world);
	~SoundSystem();

	int volume;

	void step(float elapsed_ms);

private:
	// de-allocate channels if no sound is playing 
	void deallocate_channel();
	WorldSystem* world;
	MusicState music_state;
	std::map<MusicState, std::string> music_file_names;
	std::map<MusicState, Mix_Music*> mix_music_ref;
	bool transitioning_music;
};

