#include "Time.h"

// Private static member definitions
Uint64 RustyEngine::Time::start_t;
Uint64 RustyEngine::Time::last_t;
Uint64 RustyEngine::Time::curr_t;

Uint64 RustyEngine::Time::flast_t;
Uint64 RustyEngine::Time::fcurr_t;


// Public static member definitions
float RustyEngine::Time::delta_t;
float RustyEngine::Time::timescale;

float RustyEngine::Time::fixed_delta_t;
float RustyEngine::Time::fixed_timescale;



// Public static method definitions

// Set static values (use after SDL init, and before level load)
void RustyEngine::Time::init()
{
	RustyEngine::Time::start_t = SDL_GetPerformanceCounter();
	RustyEngine::Time::curr_t = RustyEngine::Time::start_t;
	RustyEngine::Time::last_t = RustyEngine::Time::start_t;
	RustyEngine::Time::fcurr_t = RustyEngine::Time::start_t;
	RustyEngine::Time::flast_t = RustyEngine::Time::start_t;
	RustyEngine::Time::delta_t = 0;
	RustyEngine::Time::fixed_delta_t = 0;
	RustyEngine::Time::timescale = 1;
	RustyEngine::Time::fixed_timescale = 1;
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


// Returns time in seconds since game launched (since init() was called)
double RustyEngine::Time::timeSinceStartup()
{
	// Calculate time with doubles, then return float
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


// Hidden private constructor (class is currently static only.. could also make it a namespace)
RustyEngine::Time::Time()
{
}

// Hidden private destructor
RustyEngine::Time::~Time()
{
}
