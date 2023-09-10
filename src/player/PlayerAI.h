#pragma once

#include <future>

#include "Player.h"

#include "./src/AI/Search.h"

class PlayerAI : public Player
{
private:
	Search *mpSearch;

	std::future<Move> mSearchMoveResult;

public:
	PlayerAI(Game *pGame, const char *str, bool isWhite);
	PlayerAI(Game *pGame, bool isWhite);

	~PlayerAI();

	void update() override;
	void handleEvents(SDL_Event& e, const Uint8* pKeyboardState, const Uint32 pMouseState, int x, int y) override;

private:
	void startSearch();
	Move asyncSearch();
};

