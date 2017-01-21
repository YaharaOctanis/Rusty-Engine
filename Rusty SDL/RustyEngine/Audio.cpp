#include "Audio.h"
#include <iostream>
#include <math.h>


namespace RustyEngine
{
//  STATIC CONTEXT
// -----------------------------------------------------------------------------------
	bool Audio::initialized = false;				// Has audio subsystem been initialized?
	int Audio::frequency = MIX_DEFAULT_FREQUENCY;	// Playback frequency (default 22050 Hz) - 44100 Hz should work best on most systems
	Uint16 Audio::format = MIX_DEFAULT_FORMAT;		// Audio format (default signed 16bit samples in host byte order)
	int Audio::channels = MIX_DEFAULT_CHANNELS;		// Speaker setup (default 2ch audio - stereo)
	int Audio::chunk_size = 2048;					// Size of data chunk in bytes send to soundcard each callback (default 2048 bytes)
													// Modify this value if experience audio cuts during playback
													// Must be power of 2.


	// Initialize audio subsystem
	bool Audio::init()
	{
		if (initialized) // Do not initialize twice
			return true;

		// Open audio device channels
		if (Mix_OpenAudio(frequency, format, channels, chunk_size) < 0)
		{
			std::cout << "AUDIO | Cannot initialize audio! SDL_mixer error: " << Mix_GetError() << std::endl;
			return false;
		}
		initialized = true;
		return true;
	}

	// Stop music and all audio channels, then close mixer subsystem
	// Does not handle clean-up of loaded audio chunks, you should do that before calling quit
	bool Audio::quit()
	{
		Mix_HaltMusic();		// stop music
		Mix_HaltChannel(-1);	// stop all channels
		Mix_Quit();				// stop mixer subsystem
		return false;
	}
// -----------------------------------------------------------------------------------

	// Constructors
	Audio::Audio()
	{
		sound = nullptr;
		filename = "";
		loaded = false;
	}

	Audio::Audio(std::string file_path)
	{
		sound = nullptr;
		filename = "";
		load(file_path);
	}


	// Destructor - cleans up audio data from memory
	Audio::~Audio()
	{
		Mix_FreeChunk(sound); // Remove audio data from memory
		sound = nullptr;
		loaded = false;
	}


	// Load sound effect from file
	bool Audio::load(std::string file_path)
	{
		if (sound != nullptr) // Is something already loaded?
			return false; // unload here

		filename = file_path;
		sound = Mix_LoadWAV(filename.c_str()); // Load sound effect

		// If load failed
		if (sound == NULL)
		{
			std::cout << "AUDIO | Failed to load: " << filename << " " << Mix_GetError() << std::endl;
			return false;
		}

		return true;
	}


	// Play the sample and loop it n times (-1 for infinite loop)
	void Audio::play(int loops)
	{
		channel = Mix_PlayChannel(-1, sound, loops);
	}


	// Pause playback of the sample (only works when playing)
	void Audio::pause()
	{
		// TODO add callback to reset channel to -1 after finishing playback
		Mix_Pause(channel);
	}


	// Resume playback of the sample (only works when paused)
	void Audio::resume()
	{
		Mix_Resume(channel);
	}


	// Stop sample playback and free playback channel
	void Audio::stop()
	{
		Mix_HaltChannel(channel);
	}


	// Set playback volume (0 - 100)
	void Audio::setVolume(int percent)
	{
		float vol = ((float)MIX_MAX_VOLUME) / 100.0f;  // Convert percent to mixer known value (MIX_MAX_VOLUME is not 100)
		Mix_VolumeChunk(sound, roundf(vol * percent)); // Apply volume change
	}
}