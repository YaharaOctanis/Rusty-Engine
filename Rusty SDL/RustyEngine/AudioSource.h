#ifndef RUSTYENGINE_AUDIOSOURCE_H
#define RUSTYENGINE_AUDIOSOURCE_H

#pragma once

#include "Component.h"
#include "SoundEffect.h"

namespace RustyEngine
{
	class AudioSource : public Component
	{
	private:
		static AudioSource *current[16];		// List of audio sources currently playing on each channel
		static bool disabled;
	public:
		static void playbackFinished(int c);	// Called when playback has finished
		static void stopAll();
		static void pauseAll();
		static void resumeAll();
		static void toggleAudio();
		static void disableAudio();
		static void enableAudio();
		static void killAudio();
		static bool isDisabled();

	private:
		SoundEffect *sound_effect;
		int volume;
		int playback_channel;
		bool isPlaying;
		bool isPaused;

		
	public:
		bool isPositional;
		float max_distance;	// Used for sound attenuation. 
							// When listener is at max distance from source, effect will be silent
							// Closer it is, louder it will get, all the way to maximum volume, at distance 0. Fall-off is linear.
							// Fall-off is set via panning.

		// Constructors
		AudioSource();
		AudioSource(SoundEffect *effect);

		// Destructor
		~AudioSource();

		// Set effect
		void setSoundEffect(SoundEffect *effect);
		
		// Playback controls
		void play(int loops = 0, int channel = -1);	// Play the sample on a given channel and loop it n times (-1 for infinite loop)
		void pause();								// Pause playback on the channel (only works when playing)
		void resume();								// Resume playback on the channel (only works when paused)
		void stop();								// Stop sample playback and free playback channel
		void setVolume(int percent);				// Set channel playback volume (0 - 100)

		// Update
		void update();	// called once per frame at the end of render loop
	};
}

#endif // RUSTYENGINE_AUDIOSOURCE_H