#pragma once

#define PATH_SOUNDS "resources\\audio\\"
#define SOUNDS_SIZE 9

#include <SDL.h>
#include <SDL_mixer.h>

#include <iostream>
#include <string>

#include <map>
using namespace std;

class Audio
{
public:
	// Defening piece id
	static const int gameStart = 0;
	static const int gameEnd = 1;
	static const int moveSelf = 2;
	static const int moveOpponent = 3;
	static const int moveCheck = 4;
	static const int castle = 5;
	static const int promote = 6;
	static const int capture = 7;
	static const int tenSecondsAlert = 8;

	Audio();
	~Audio();

	bool init();
	void kill();

	void playSound(int soundIndex);

	bool loadSounds();

private:
	static map<int, string> mIndexToName;
	
	Mix_Chunk **mpSounds;
};
