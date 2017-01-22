#ifndef RUSTYENGINE_SOUNDEFFECT_H
#define RUSTYENGINE_SOUNDEFFECT_H

#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include "Audio.h"
#include <string>

namespace RustyEngine
{
	// Handles playing of sound effects
	// Sound effects support WAVE, AIFF, RIFF, OGG, VOC
	// Supports simultaneous playback of 16 sound effects (will not queue sound effects, if all 16 channels are full, you can do that manually)
	// TODO implement sound effect queue for when game requests playback of more than 16 effects (or maybe switch to openAL for more channels?)
	class SoundEffect
	{
		friend class AudioSource;
	protected:
		bool loaded;				// Is sound file loaded?
		std::string filename;		// Audio source file
		Mix_Chunk *sound;			// Audio data
		int channel;				// On which playback channel is sound being played? (max 16 channels, not to be confused with 'static channels variable', that one controls speaker setup)
									// That means that you can only play 16 effects at the same time

	public:
		// Constructors
		SoundEffect();
		SoundEffect(std::string file_path);

		// Destructor - cleans up audio data from memory
		~SoundEffect();

		// Loader
		bool load(std::string file_path);	// Load sound effect from file

		// Playback controls
		/*
		int play(int loops = 0, int channel = -1);	// Play the sample on a given channel and loop it n times (-1 for infinite loop)
		void pause();								// Pause playback on the channel (only works when playing)
		void resume();								// Resume playback on the channel (only works when paused)
		void stop();								// Stop sample playback and free playback channel
		void setVolume(int percent);				// Set channel playback volume (0 - 100)
		*/
	};
}

#endif // RUSTYENGINE_SOUNDEFFECT_H