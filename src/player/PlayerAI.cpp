#include "PlayerAI.h"
#include "./src/engine/Game.h"

PlayerAI::PlayerAI(Game * pGame, const char *str, bool isWhite)
	: Player(pGame, str, isWhite), mpSearch(new SearchV5()) {}

PlayerAI::PlayerAI(Game *pGame, bool isWhite)
	: PlayerAI(pGame, "ZedChess", isWhite) {}

PlayerAI::~PlayerAI() {
	delete mpSearch;
}

void PlayerAI::update() 
{
	if (!mpGame->isGameOver() && !mpGame->isPaused())
	{
		if (mpGame->getBoard()->whiteToMove == isWhite)
		{
			if (mSearchMoveResult.valid() == false)
			{
				startSearch();
				return;
			}

			if (mSearchMoveResult.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				Move move = mSearchMoveResult.get();
				mpGame->makeAnimatedMove(move); //TODO: Consider check if legal move ?
				cout << " ---> Move played: " << BoardRepresentation::getMoveString(move) << endl << endl;
			}
		}
	}
}

void PlayerAI::handleEvents(SDL_Event& e, const Uint8* pKeyboardState, const Uint32 pMouseState, int x, int y)
{

}

void PlayerAI::startSearch() {
	mSearchMoveResult = std::async(std::launch::async, &PlayerAI::asyncSearch, this);
}

Move PlayerAI::asyncSearch() {
	mpSearch->searchMove(mpGame->getBoard(), 2000);
	return mpSearch->getBestMove();
}