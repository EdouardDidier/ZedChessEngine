#pragma once

#include <iostream>
#include <string>

#include <SDL.h>

#include "Clock.h"

#include "./src/Engine/Board.h"
#include "./src/Engine/Move.h"

using namespace std;

class Game;

class Player
{
public:
	string name;
	bool isWhite;

protected:
	Game *mpGame;

	Clock mClock;

public:
	Player(Game *pGame, const char *str, bool isWhite);
	virtual ~Player();

	virtual void update() = 0;
	virtual void handleEvents(SDL_Event& e, const Uint8* pKeyboardState, const Uint32 pMouseState, int x, int y) = 0;
};