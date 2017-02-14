#ifndef RUSTYENGINE_MUSIC_H
#define RUSTYENGINE_MUSIC_H

#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include "Audio.h"

namespace RustyEngine
{
	// Handles playback of music
	// Music supports WAVE, MOD, S3m, IT, XM, MIDI, OGG, MP3, FLAC
	// Supports playback of only one music track at the time.
	class Music
	{

	//  STATIC CONTEXT
	// ---------------------------------------------------
	private:
		static Music *current;	// Currently played music file
		static bool disabled;

	public:
		static void playbackFinished();	// Called when music playback has finished
		static void stopCurrent();
		static void pauseCurrent();
		static void resumeCurrent();
		static void toggleMusic();
		static void disableMusic();
		static void enableMusic();
		static void killMusic();
		static bool isDisabled();
	// ---------------------------------------------------

	private:
		bool loaded;			// Is sound file loaded?
		std::string filename;	// Audio source file
		Mix_Music *music;		// Audio data
		bool isPlaying;
		bool isPaused;

	public:
		// Constructors
		Music();
		Music(std::string file_path);

		// Destructor
		~Music();

		// Loader
		bool load(std::string file_path);	// Load sound effect from file

		// Playback controls
		void play(int loops = 0);		// Play the music and loop it n times (-1 for infinite loop)
		void pause();					// Pause music playback (only works when playing)
		void resume();					// Resume music playback(only works when paused)
		void stop();					// Stop music playback
		void setVolume(int percent);	// Set music playback volume (0 - 100)
	};
}

#endif // RUSTYENGINE_MUSIC_H