#include "AudioSource.h"
#include "Game.h"
#include "RustyConstants.h"


namespace RustyEngine
{
	AudioSource* AudioSource::current[16];		// List of audio sources currently playing on each channel

	// Callback function called when playback has finished
	void AudioSource::playbackFinished(int c)
	{
		if (AudioSource::current[c] == nullptr)
			return;
		AudioSource::current[c]->isPlaying = false;
		AudioSource::current[c]->isPaused = false;
		AudioSource::current[c]->playback_channel = -1;
		AudioSource::current[c] = nullptr;
	}

	void AudioSource::stopAll()
	{
		for (int i = 0; i < 16; i++)
		{
			if(AudioSource::current[i] != nullptr)
				AudioSource::current[i]->stop();
		}
	}

	// Constructors
	AudioSource::AudioSource()
	{
		volume = 100;
		isPlaying = false;
		isPaused = false;
		sound_effect = nullptr;
		playback_channel = -1;
		isPositional = true;
		max_distance = 100;
	}

	AudioSource::AudioSource(SoundEffect * effect)
	{
		volume = 100;
		isPlaying = false;
		isPaused = false;
		sound_effect = effect;
		playback_channel = -1;
		isPositional = true;
		max_distance = 100;
	}

	// Destructor
	AudioSource::~AudioSource()
	{
		if (isPlaying || isPaused)
		{
			Mix_HaltChannel(playback_channel);
			if(playback_channel != -1)
				current[playback_channel] = nullptr;
			Mix_ChannelFinished(nullptr);
		}

		sound_effect = nullptr;
	}

	void AudioSource::setSoundEffect(SoundEffect * effect)
	{
		sound_effect = effect;
	}

	// Play the sample and loop it n times (-1 for infinite loop)
	void AudioSource::play(int loops, int channel)
	{
		// If sound is playing, stop it first
		if (isPlaying && playback_channel != -1)
			Mix_HaltChannel(playback_channel);

		// Play sound on a given channel
		playback_channel = Mix_PlayChannel(channel, sound_effect->sound, loops);
		current[playback_channel] = this;

		if (playback_channel != -1)
		{
			Mix_Volume(playback_channel, volume);
			Mix_ChannelFinished(playbackFinished);
			isPlaying = true;
			isPaused = false;
		}
	}


	// Pause playback of the sample (only works when playing)
	void AudioSource::pause()
	{
		// TODO add callback to reset channel to -1 after finishing playback
		if (isPlaying && !isPaused)
		{
			Mix_Pause(playback_channel);
			isPlaying = false;
			isPaused = true;
		}
	}


	// Resume playback of the sample (only works when paused)
	void AudioSource::resume()
	{
		if (!isPlaying && isPaused)
		{
			Mix_Resume(playback_channel);
			isPlaying = true;
			isPaused = false;
		}
	}


	// Stop sample playback and free playback channel
	void AudioSource::stop()
	{
		if(isPlaying || isPaused)
			Mix_HaltChannel(playback_channel);
		isPlaying = false;
		isPaused = false;
		current[playback_channel] = nullptr;
		playback_channel = -1;
	}


	// Set playback volume (0 - 100)
	void AudioSource::setVolume(int percent)
	{
		volume = percent;
		if (isPlaying || isPaused)
		{
			float vol = ((float)MIX_MAX_VOLUME) / 100.0f;  // Convert percent to mixer known value (MIX_MAX_VOLUME is not 100)
			Mix_Volume(playback_channel, roundf(vol * volume)); // Apply volume change
		}
	}

	// Update, called once per frame at the end of render loop
	void AudioSource::update()
	{
		if (!isPlaying)
			return;

		// Set panning based on x-axis distance
		float left = 1, right = 1;
		Uint8 power = 254;
		float distance = game_object->transform.position.x - Game::world.audio_listener->transform.position.x;

		// Calculate panning
		if (distance > 0.01) // if distance is positive, then source is on the right
		{
			if (distance >= max_distance)
				left = 0;
			else
				left = 1 - (distance / max_distance);
		}
		else if (distance < -0.01) // if negative, then it's on the left
		{
			distance = -distance;
			if (distance >= max_distance)
				right = 0;
			else
				right = 1 - (distance / max_distance);
		} // otherwise it's on the center
		
		distance = game_object->transform.position.distanceTo(Game::world.audio_listener->transform.position);

		if (distance >= max_distance)
			power = 0;
		else
			power = (Uint8)roundf(254 * (1 - (distance / max_distance))); // Calculate attenuation

		Mix_SetPanning(playback_channel, power * left, power * right);
	}
}
