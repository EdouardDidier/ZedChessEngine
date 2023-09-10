#pragma once

#include "Player.h"

class PlayerHuman : public Player
{
public:
	PlayerHuman(Game *pGame, const char *str, bool isWhite);
	PlayerHuman(Game *pGame, bool isWhite);

	void update() override;
	void handleEvents(SDL_Event& e, const Uint8* pKeyboardState, const Uint32 pMouseState, int x, int y) override;
};
