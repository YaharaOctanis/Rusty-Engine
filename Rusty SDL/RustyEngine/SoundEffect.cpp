#include "SoundEffect.h"
#include <iostream>

namespace RustyEngine
{
	// Constructors
	SoundEffect::SoundEffect()
	{
		sound = nullptr;
		filename = "";
		loaded = false;
	}

	SoundEffect::SoundEffect(std::string file_path)
	{
		sound = nullptr;
		filename = "";
		load(file_path);
	}


	// Destructor - cleans up audio data from memory
	SoundEffect::~SoundEffect()
	{
		Mix_FreeChunk(sound); // Remove audio data from memory
		sound = nullptr;
		loaded = false;
	}


	// Load sound effect from file
	bool SoundEffect::load(std::string file_path)
	{
		if (sound != nullptr) // Is something already loaded?
			return false; // unload here

		filename = file_path;
		sound = Mix_LoadWAV(filename.c_str()); // Load sound effect

											   // If load failed
		if (sound == NULL)
		{
			std::cout << "SOUND FX | Failed to load: " << filename << " " << Mix_GetError() << std::endl;
			return false;
		}

		return true;
	}
	/*

	// Play the sample and loop it n times (-1 for infinite loop)
	int SoundEffect::play(int loops, int channel)
	{
		channel = Mix_PlayChannel(channel, sound, loops);
		return channel;
	}


	// Pause playback of the sample (only works when playing)
	void SoundEffect::pause()
	{
		// TODO add callback to reset channel to -1 after finishing playback
		Mix_Pause(channel);
	}


	// Resume playback of the sample (only works when paused)
	void SoundEffect::resume()
	{
		Mix_Resume(channel);
	}


	// Stop sample playback and free playback channel
	void SoundEffect::stop()
	{
		Mix_HaltChannel(channel);
	}


	// Set playback volume (0 - 100)
	void SoundEffect::setVolume(int percent)
	{
		float vol = ((float)MIX_MAX_VOLUME) / 100.0f;  // Convert percent to mixer known value (MIX_MAX_VOLUME is not 100)
		Mix_VolumeChunk(sound, roundf(vol * percent)); // Apply volume change
	}*/
}