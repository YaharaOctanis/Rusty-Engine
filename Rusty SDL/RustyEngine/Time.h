#ifndef RUSTYENGINE_TIME_H
#define RUSTYENGINE_TIME_H

#pragma once

#include <SDL.h>
#include <cmath>

namespace RustyEngine
{
	// Holds and calculates information about time passed between frames
	// Use it for calculating movement of game objects
	// Can be used to manipulate time in-game
	class Time
	{
	private:
		static Uint64 start_t;			// Timer tick at init
		static Uint64 last_t;			// Timer tick at last timer update
		static Uint64 curr_t;			// Current tick - used as temp var, for delta calculation

		static Uint64 flast_t;			// Timer tick at last fixed timer update
		static Uint64 fcurr_t;			// Current tick - used as temp var for fixed_delta calculation
		
	public:
		static float delta_t;			// Time between rendered frames
		static float timescale;			// Multiplies delta_t (when calculating it) - avoid high values, since precision loss can occur 
										// Negative values are allowed, but not recommended (they will not reverse time, but only result in negative delta)

		static float fixed_delta_t;		// Time between fixed updates (physics)
		static float fixed_timescale;	// Multiplies fixed_delta_t (when calculating it) - avoid high values, since precision loss can occur
										// Negative values are allowed, but not recommended (they will not reverse time, but only result in negative delta)

		static void init();							// Set start_t (use after SDL init, and before level load)
		static void recalculate();					// Calculate new delta_t (use at the end of render loop)
		static void recalculateFixed();				// Calculate new fixed_delta_t (use at the end of physics loop)
		static double timeSinceStartup();			// Returns time in seconds since game launched (since init() was called)
		static Uint64 getCurrTick();				// Returns value of high-resolution timer
		static Uint64 getLastTick();				// Returns tick of last timer update
		static Uint64 getLastTickFixed();			// Returns tick of last fixed timer update
		static double diffInMs(Uint64 a, Uint64 b);	// Returns time in seconds, between given tick values 
													// b is optional (if not given, current tick will be used)

	private:
		Time();		// Do not declare objects of type Time
		~Time();	// Do not declare objects of type Time
	};
}

#endif