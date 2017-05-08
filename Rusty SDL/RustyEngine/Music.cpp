#include "Music.h"
#include <iostream>
#include "Game.h"


namespace RustyEngine
{
	Music* Music::current;
	bool Music::disabled = false;

	// Private callback function called when playback has finished
	void Music::playbackFinished()
	{
		if (current == nullptr)
			return;
		current->isPlaying = false;
		current->isPaused = false;
		current = nullptr;
	}

	void Music::stopCurrent()
	{
		if(current != nullptr)
			current->stop();
	}

	void Music::pauseCurrent()
	{
		if (current != nullptr)
			current->pause();
	}

	void Music::resumeCurrent()
	{
		if (current != nullptr)
			current->resume();
	}

	void Music::toggleMusic()
	{
		disabled = !disabled;

		if (disabled)
			pauseCurrent();
		else
			resumeCurrent();
	}

	void Music::disableMusic()
	{
		disabled = true;
		pauseCurrent();
	}

	void Music::enableMusic()
	{
		disabled = false;
		resumeCurrent();
	}

	void Music::killMusic()
	{
		disabled = true;
		stopCurrent();
	}

	bool Music::isDisabled()
	{
		return disabled;
	}

	// Constructors
	Music::Music()
	{
		music = nullptr;
		filename = "";
		loaded = false;
		isPlaying = false;
		isPaused = false;
	}

	Music::Music(std::string file_path)
	{
		music = nullptr;
		filename = "";
		loaded = false;
		isPlaying = false;
		isPaused = false;
		load(filename);
	}


	// Destructor - cleans up audio data from memory
	Music::~Music()
	{
		if (isPlaying || isPaused)
		{
			Mix_HaltMusic();
			current = nullptr;
		}
		Mix_FreeMusic(music);	// Remove audio data from memory
		music = nullptr;
		loaded = false;
		isPlaying = false;
	}


	// Load sound effect from file
	bool Music::load(std::string file_path)
	{
		if (music != nullptr) // Is something already loaded?
			return false; // unload here

		filename = file_path;
		music = Mix_LoadMUS(filename.c_str()); // Load music file

		// If load failed
		if (music == NULL)
		{
			std::cout << "MUSIC | Failed to load: " << filename << " " << Mix_GetError() << std::endl;
			return false;
		}

		return true;
	}


	// Play the music and loop it n times (-1 for infinite loop)
	void Music::play(int loops)
	{
		// if music is playing, first halt music, then start new music file, and change hook to new object
		if (isPlaying)
			Mix_HaltMusic();

		if (Mix_PlayMusic(music, loops) == -1)	// if playback fails, return
			return;

		current = this;
		
		Mix_HookMusicFinished(playbackFinished);
		isPlaying = true;
		isPaused = false;

		if (Music::disabled)
			pause();
	}


	// Pause music playback (only works when playing)
	void Music::pause()
	{
		if (isPlaying && !isPaused)
		{
			Mix_PauseMusic();
			isPlaying = false;
			isPaused = true;
		}
	}


	// Resume music playback(only works when paused)
	void Music::resume()
	{
		if (Music::disabled)
			return;

		if (!isPlaying && isPaused)
		{
			Mix_ResumeMusic();
			isPlaying = true;
			isPaused = false;
		}
	}


	// Stop music playback
	void Music::stop()
	{
		if (isPlaying)
		{
			Mix_HaltMusic();
			isPlaying = false;
			isPaused = false;
			current = nullptr;
		}
	}


	// Set music playback volume (0 - 100)
	void Music::setVolume(int percent)
	{
		float vol = ((float)MIX_MAX_VOLUME) / 100.0f;  // Convert percent to mixer known value (MIX_MAX_VOLUME is not 100)
		Mix_VolumeMusic(roundf(vol * percent)); // Apply volume change
	}
}