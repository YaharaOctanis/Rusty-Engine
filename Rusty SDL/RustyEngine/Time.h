#ifndef RUSTYENGINE_TIME_H
#define RUSTYENGINE_TIME_H

#pragma once

#include <SDL.h>

namespace RustyEngine
{
	// Holds and calculates information about time passed between frames
	// Use it for calculating movement of game objects
	// Can be used to manipulate time in-game
	class Time
	{
	private:
		static float start_t;			// Timer tick at init
		static float last_t;			// Timer tick at last timer update
		static float curr_t;			// Current tick - used as temp var, for delta calculation

		static float flast_t;			// Timer tick at last fixed timer update
		static float fcurr_t;			// Current tick - used as temp var for fixed_delta calculation

	public:
		static float delta_t;			// Time between rendered frames
		static float timescale;			// Multiplies delta_t (when calculating it)

		static float fixed_delta_t;		// Time between fixed updates (physics)
		static float fixed_timescale;	// Multiplies fixed_delta_t (when calculating it)

		static void init();					// Set start_t (use after SDL init, and before level load)
		static void recalculate();			// Calculate new delta_t (use at the end of render loop)
		static void recalculate_fixed();	// Calculate new fixed_delta_t (use at the end of physics loop)

	private:
		Time();
		~Time();
	};
}

#endif