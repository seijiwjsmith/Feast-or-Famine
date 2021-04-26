#include "sound_system.hpp"


void adjust_volume(int channel)
{
    int channel_playing = Mix_Playing(-1);
    
    // multiple projectiles sounds stacked together may sound loud
    // decrease the volume for all channels
    //std::cout << "Channel playing: " << Mix_Playing(-1) << "\n";
    for (int i = 0; i < Mix_AllocateChannels(-1); i++)
    {
        int volume = 70 - 4 * (channel_playing);
        volume = (volume < 30) ? 30 : volume;
        Mix_Volume(i, volume);
        //std::cout << "Volume: " << volume << "\n";
    }
    
}

void play_sound(SoundRef& sound_ref)
{
    Mix_Chunk* chunk = cache_chunk(sound_ref.file_path);
    int channel_played = Mix_PlayChannel(-1, chunk, 0);
    // if no channels are free
    if (channel_played == -1) {
        // allocate new channels
        Mix_AllocateChannels(Mix_AllocateChannels(-1) + 1);
        channel_played = Mix_PlayChannel(-1, chunk, 0);
    }
    adjust_volume(channel_played);
    sound_ref.play_delay_counter_ms = sound_ref.play_delay_ms;
    sound_ref.channel_num = channel_played;
    sound_ref.play_sound = false;
    sound_ref.one_time_sound_played = true;
}

void sound_on_destroy(entt::basic_registry<entt::entity>& registry, entt::entity entity) {
    //std::cout << "SoundRef destoryed\n";
    auto& sound_ref = registry.get<SoundRef>(entity);
    if (!sound_ref.on_impact_destory)
    {
        if (Mix_GetChunk(sound_ref.channel_num) == cache_chunk(sound_ref.file_path))
        {
            Mix_HaltChannel(sound_ref.channel_num);
        }
        
        /*if (sound_ref.one_time_sound_played && !sound_ref.is_continuous)
            if (cache_chunk(sound_ref.file_path) != nullptr)
                Mix_FreeChunk(cache_chunk(sound_ref.file_path));*/
    }
    else
    {
        if (sound_ref.play_sound)
        {
            play_sound(sound_ref);
        }
    }

}

void sound_on_construct(entt::basic_registry<entt::entity>& registry, entt::entity entity) {
    std::cout << "SoundRef constructed\n";
}

// constructor for SoundSystem
SoundSystem::SoundSystem(WorldSystem* world)
{
    volume = 110;
    // reserve 10 channels for world
    Mix_AllocateChannels(10);
    auto destroy_sink = registry.on_destroy<SoundRef>();
    destroy_sink.connect<&sound_on_destroy>();
    this->world = world;
    // load all music
    std::map<MusicState, std::string> music_file_names =
    {
        {MusicState::title_screen_music, "Two Steps from Hell - Heart of Courage.wav"},
        //{MusicState::player_prepare_music, "Skydrome (epic synthwave metal).wav"},
        //{MusicState::player_battle_music, "Overkill - Antti Martikainen.wav"},
        {MusicState::vicotry_screen_music, "FFI_Victory.wav"},
        {MusicState::lost_screen_music, ""}, // no music during lost screen
        {MusicState::normal_round_music, "Skydrome (epic synthwave metal).wav"},
        {MusicState::final_round_music, "The Game Changer (epic symphonic metal).wav"}
    };

    for (auto music_file : music_file_names)
    {
        Mix_Music * music_ref = Mix_LoadMUS(audio_path("music/" + music_file.second).c_str());
        mix_music_ref.emplace(music_file.first, music_ref);
    }
}


SoundSystem::~SoundSystem()
{
    registry.clear<SoundRef>();
    Mix_CloseAudio();
}

void SoundSystem::step(float elasped_ms)
{
    // check music state change in world
    if (this->music_state != this->world->music_state)
    {
        // play music
        Mix_FadeOutMusic(1000);
        if (Mix_PlayingMusic())
        {
            transitioning_music = true;
        }
        this->music_state = this->world->music_state;
    }
    if (transitioning_music && !Mix_PlayingMusic())
    {
        Mix_FadeInMusic(mix_music_ref.at(this->music_state), -1, 1000);
        Mix_VolumeMusic(60);
    }
    if (world->game_state == WorldSystem::GameState::story_card)
    {
        Mix_VolumeMusic(40);
    }
    else if (world->player_state == WorldSystem::PlayerState::battle_stage)
    {
        Mix_VolumeMusic(60);
    }
    auto sound_ref_view = registry.view<SoundRef>();
    // play all sounds continuously if the entity exists
	for (auto entity : sound_ref_view)
	{
        auto& sound_ref = registry.get<SoundRef>(entity);
        if (sound_ref.is_continuous)
        {
            if (sound_ref.play_delay_counter_ms > 0)
            {
                sound_ref.play_delay_counter_ms -= elasped_ms;
            }
            else
            {
                play_sound(sound_ref);
            }
        }
        else
        {
            if (sound_ref.play_sound)
                play_sound(sound_ref);
        }
	}
    deallocate_channel();
}

// de-allocate channels if no sound is playing 
void SoundSystem::deallocate_channel()
{
    if (Mix_Playing(-1) == 0 )
    {
        // reserve 10 channels for world
        Mix_AllocateChannels(10);
    }
}




