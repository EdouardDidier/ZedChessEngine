#include "PlayerHuman.h"
#include "./src/engine/Game.h"

PlayerHuman::PlayerHuman(Game *pGame, const char* str, bool isWhite)
	: Player(pGame, str, isWhite) {}

PlayerHuman::PlayerHuman(Game *pGame, bool isWhite)
	: PlayerHuman(pGame, "Player", isWhite) {}

void PlayerHuman::update() {

}

void PlayerHuman::handleEvents(SDL_Event& e, const Uint8* pKeyboardState, const Uint32 pMouseState, int x, int y)
{

}