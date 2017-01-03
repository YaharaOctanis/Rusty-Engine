#include "Time.h"

void RustyEngine::Time::init()
{
	RustyEngine::Time::start_t = SDL_GetPerformanceCounter();
}

void RustyEngine::Time::recalculate()
{
	using RustyEngine::Time;
	curr_t = SDL_GetPerformanceCounter();
	delta_t = (curr_t - last_t) / ((float)SDL_GetPerformanceFrequency());
	last_t = curr_t;
}

void RustyEngine::Time::recalculate_fixed()
{
	using RustyEngine::Time;
	fcurr_t = SDL_GetPerformanceCounter();
	fixed_delta_t = (fcurr_t - flast_t) / ((float)SDL_GetPerformanceFrequency());
	flast_t = fcurr_t;
}

RustyEngine::Time::Time()
{
}


RustyEngine::Time::~Time()
{
}
