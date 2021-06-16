#include "Time.h"

// Private static member definitions
Uint64 RustyEngine::Time::start_t;	// Timer tick at init
Uint64 RustyEngine::Time::last_t;	// Timer tick at last timer update
Uint64 RustyEngine::Time::curr_t;	// Current tick - used as temp var, for delta calculation

Uint64 RustyEngine::Time::flast_t;	// Timer tick at last fixed timer update
Uint64 RustyEngine::Time::fcurr_t;	// Current tick - used as temp var for fixed_delta calculation


// Public static member definitions
float RustyEngine::Time::delta_t;	// Time between rendered frames
float RustyEngine::Time::timescale;	// Multiplies delta_t (when calculating it) - avoid high values, since precision loss can occur 
									// Negative values are allowed, but not recommended (they will not reverse time, but only result in negative delta)

float RustyEngine::Time::fixed_delta_t;		// Time between fixed updates (physics)
float RustyEngine::Time::fixed_timescale;	// Multiplies fixed_delta_t (when calculating it) - avoid high values, since precision loss can occur
											// Negative values are allowed, but not recommended (they will not reverse time, but only result in negative delta)



// Public static method definitions

// Set static values (use after SDL init, and before level load)
void RustyEngine::Time::init()
{
	RustyEngine::Time::start_t = SDL_GetPerformanceCounter();	// Remember starting tick
	RustyEngine::Time::curr_t = RustyEngine::Time::start_t;		// Set everything to starting tick
	RustyEngine::Time::last_t = RustyEngine::Time::start_t;
	RustyEngine::Time::fcurr_t = RustyEngine::Time::start_t;
	RustyEngine::Time::flast_t = RustyEngine::Time::start_t;
	RustyEngine::Time::delta_t = 0;			// Delta is 0 when starting of course
	RustyEngine::Time::fixed_delta_t = 0;	// Same goes for fixed delta
	RustyEngine::Time::timescale = 1;		// Default timescale is 1
	RustyEngine::Time::fixed_timescale = 1;	// Default fixed timescale is also 1
}


// Calculate new delta_t (use at the end of render loop)
void RustyEngine::Time::recalculate()
{
	using RustyEngine::Time;
	curr_t = SDL_GetPerformanceCounter();	// Get current high-resolution timer tick
	delta_t = ((double)(curr_t - last_t) / (double)SDL_GetPerformanceFrequency()) * timescale; // Don't forget to multiply by timescale and cast results to float
	last_t = curr_t;
}


// Calculate new fixed_delta_t (use at the end of physics loop)
void RustyEngine::Time::recalculateFixed()
{
	using RustyEngine::Time;
	fcurr_t = SDL_GetPerformanceCounter();	// Get current high-resolution timer tick
	fixed_delta_t = ((double)(fcurr_t - flast_t) / ((double)SDL_GetPerformanceFrequency())) * fixed_timescale; // Don't forget to multiply by timescale and cast results to float
	flast_t = fcurr_t;
}


// TODO - FIX
// Returns time in seconds since game launched (since init() was called)
double RustyEngine::Time::timeSinceStartup()
{
	// Calculate time with doubles, then return float ( (start - curr) / freq )
	return (double)(RustyEngine::Time::start_t - SDL_GetPerformanceCounter()) / (double)SDL_GetPerformanceFrequency();
}


// Returns value of high-resolution timer
Uint64 RustyEngine::Time::getCurrTick()
{
	return SDL_GetPerformanceCounter();
}


// Returns tick of last timer update
Uint64 RustyEngine::Time::getLastTick()
{
	return RustyEngine::Time::last_t;
}


// Returns tick of last fixed timer update
Uint64 RustyEngine::Time::getLastTickFixed()
{
	return RustyEngine::Time::flast_t;
}


// Returns time in seconds, between given tick values 
// b is optional (if not given, current tick will be used)
double RustyEngine::Time::diffInMs(Uint64 a, Uint64 b) 
{
	// make sure we left operand is bigger
	if(b > a)
		return (double)(b - a) / (double)SDL_GetPerformanceFrequency(); // performance frequency tells how many ticks are in 1 second
	else
		return (double)(a - b) / (double)SDL_GetPerformanceFrequency();
}


// Hidden private constructor (class is currently static only)
RustyEngine::Time::Time()
{
}

// Hidden private destructor
RustyEngine::Time::~Time()
{
}
