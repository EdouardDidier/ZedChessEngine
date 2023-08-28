#include "Profiler.h"

Profiler::Profiler() {
	mTicks = 0;
}

Profiler::~Profiler() {

}

void Profiler::startMeasure() {
	mTicks = SDL_GetTicks64();
}

Uint64 Profiler::endMeasure() {
	Uint64 ticks = SDL_GetTicks64();
	Uint64 elapsed = ticks - mTicks;
	
	mListElapsed.push_back(elapsed);

	return elapsed;
}

float Profiler::getAverage() {
	Uint64 total = 0;
	int i = 0;

	for (Uint64 elapsed : mListElapsed) {
		total += elapsed;
		i++;
	}

	return (float)total / (float)i;
}