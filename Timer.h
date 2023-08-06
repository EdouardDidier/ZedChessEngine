#pragma once

#include <SDL.h>

class Timer
{
public:
	Timer();
	~Timer();

	float getElapsedTime();

private:
	Uint64  mTime = 0;
};

