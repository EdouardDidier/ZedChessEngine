#pragma once

#include <SDL.h>

#include <iostream>
#include <list>

using namespace std;

class Profiler
{
public:
	Profiler();
	~Profiler();

	void startMeasure();
	Uint64 endMeasure();

	float getAverage();

private:
	Uint64 mTicks;
	list<Uint64> mListElapsed;
};

