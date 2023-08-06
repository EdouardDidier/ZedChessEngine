#pragma once

#define ANIMATION_TIME 150.0f

#include <SDL.h>

#include "Graphic.h"
#include "Coord.h"

#include "Move.h"

class Graphic;

class Animation
{
public:
	Animation(Graphic *pGraphic, Move move, int piece, int targetPiece);

	void startAnimation(Move move, int piece);
	bool animate(float elapsed);

	Move getMove();
	int getPiece();
	int getTargetPiece();

	bool isComplete();

	void end();

private:
	Graphic *mpGraphic;

	Move mMove;
	int mPiece;
	int mTargetPiece;


	float mProgress;
	int mStartX, mStartY;
	int mTargetX, mTargetY;
};
