#include "Timer.h"

Timer::Timer() {
	mTime = SDL_GetTicks();
}

Timer::~Timer() {

}

float Timer::getElapsedTime() {
	Uint64 newTime = SDL_GetTicks64();
	
	float  elapsed = float(newTime - mTime);
	mTime = newTime;

	return elapsed;
}