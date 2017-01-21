#ifndef RUSTYENGINE_AUDIO_H
#define RUSTYENGINE_AUDIO_H

#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>

namespace RustyEngine
{
	// Handles audio subsystem and playing of sound effects and music
	// Sound effects support WAVE, AIFF, RIFF, OGG, VOC
	// Music supports WAVE, MOD, S3m, IT, XM, MIDI, OGG, MP3, FLAC
	// Supports simultaneous playback of 16 sound effects and 1 music track.
	class Audio
	{
	//  STATIC CONTEXT - audio subsystem (TODO transfer to audio subsystem class?)
	// -----------------------------------------------------------------------------------
	private:
		static bool initialized;	// Has audio subsystem been initialized?

		static int frequency;		// Playback frequency (default 22050 Hz) - 44100 Hz should work best on most systems
		static Uint16 format;		// Audio format (default signed 16bit samples in host byte order)
		static int channels;		// Speaker setup (default 2ch audio - stereo)
		static int chunk_size;		// Size of data chunk in bytes send to soundcard each callback (default 2048 bytes)
									// Modify this value if experience audio cuts during playback
									// Must be power of 2.

	public:
		static bool init();			// Initialize audio subsystem
		static bool quit();			// Stop music and all audio channels, then close mixer subsystem
									// Does not handle clean-up of loaded audio chunks, you should do that before calling quit
	// -----------------------------------------------------------------------------------


	private:
		bool loaded;				// Is sound file loaded?
		std::string filename;		// Audio source file
		Mix_Chunk *sound;			// Audio data
		int channel;				// On which playback channel is sound being played? (max 16 channels, not to be confused with 'static channels variable', that one controls speaker setup)
									// That means that you can only play 16 effects at the same time

	public:
		// Constructors
		Audio();
		Audio(std::string file_path);

		// Destructor - cleans up audio data from memory
		~Audio();

		// Loader
		bool load(std::string file_path);	// Load sound effect from file

		// Playback controls
		void play(int loops=0);				// Play the sample and loop it n times (-1 for infinite loop)
		void pause();						// Pause playback of the sample (only works when playing)
		void resume();						// Resume playback of the sample (only works when paused)
		void stop();						// Stop sample playback and free playback channel
		void setVolume(int percent);		// Set playback volume (0 - 100)
	};
}

#endif // RUSTYENGINE_AUDIO_H