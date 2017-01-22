#include "Audio.h"
#include <iostream>
#include <math.h>


namespace RustyEngine
{
	bool Audio::initialized = false;				// Has audio subsystem been initialized?
	int Audio::frequency = MIX_DEFAULT_FREQUENCY;	// Playback frequency (default 22050 Hz) - 44100 Hz should work best on most systems
	Uint16 Audio::format = MIX_DEFAULT_FORMAT;		// Audio format (default signed 16bit samples in host byte order)
	int Audio::channels = MIX_DEFAULT_CHANNELS;		// Speaker setup (default 2ch audio - stereo)
	int Audio::chunk_size = 2048;					// Size of data chunk in bytes send to soundcard each callback (default 2048 bytes)
													// Modify this value if experience audio cuts during playback
													// Must be power of 2.


	// Returns value of 'initialized' (true if subsystem is initialized, false otherwise)
	bool Audio::isInitialized()
	{
		return initialized;
	}


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
	// Returns false if audio subsystem isn't initialized, returns true otherwise (when subsystem was stopped successfuly)
	bool Audio::quit()
	{
		if (!initialized)
			return false;

		Mix_HaltMusic();		// stop music
		Mix_HaltChannel(-1);	// stop all channels
		Mix_Quit();				// stop mixer subsystem
		return true;
	}
}