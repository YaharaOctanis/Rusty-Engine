#ifndef RUSTYENGINE_TIME_H
#define RUSTYENGINE_TIME_H

#pragma once

namespace RustyEngine
{
	// Holds information about time passed between frames
	// Use it for calculating movement of game objects
	// Can be used to manipulate time in-game
	class Time
	{
	public:
		static float delta_t;			// Time between rendered frames
		static float timescale;			// Multiplies delta_t (when calculating it)

		static float fixed_delta_t;		// Time between fixed updates (physics)
		static float fixed_timescale;	// Multiplies fixed_delta_t (when calculating it)

		Time();
		~Time();
	};
}

#endif