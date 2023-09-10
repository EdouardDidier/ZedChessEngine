#include "Player.h"

Player::Player(Game *pGame, const char *str, bool isWhite)
	: mpGame(pGame), name(str), isWhite(isWhite) {}

Player::~Player() {};