#pragma once
#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <vector>
#include <list>

#include "Timer.h"

#include "Audio.h"

#include "Graphic.h"
#include "Animation.h"
#include "Coord.h"

#include "Board.h"
#include "Move.h"
#include "MoveGenerator.h"

using namespace std;

class Game
{
public:
	Game();
	~Game();

	bool init();
	void kill();

	void run();
	bool handleUserEvents();

	void selectSquare(int startSquare);
	void unSelectSquare(int typeHighlight = -1);

	bool isInPossibleMove(Move move);

	bool tryMove(Move move, bool isAnimated = false);
	bool makeMove(Move move);
	bool makeAnimatedMove(Move move);

	bool iaPlay();

	Coord getBoardCoord(int y, int x);

	void addHighlightSquare(int square, int type, bool persist = false);
	void clearHighlightSquares(int typeHighlight = -1);

	void playSound(Move move);

private:
	bool mDebugMode = false;

	Timer mTimer;
	Graphic mGraphic;
	Audio mAudio;
		
	// Highlight Arrays: 
	// - 0: Last move
	// - 1: Selected squares
	// - 2: Color no key squares
	// - 3: Color CTRL squares
	// - 4: Color SHIFT squares
	// - 5: Color ALT squares
	vector<int> mHighlightSquares[PALETTE_HIGHLIGHT_SIZE];

	Board* mpBoard;

	MoveGenerator mMoveGenerator;
	list<Move> mLegalMoves;
	list<Move> mPossibleMoves;

	int mSelectedSquare = -1;
	int mDraggedPiece = -1;
	bool mToUnselectFlag = false; // Flag to check if selected piece is ready to be unselected

	Coord mHoverSquare = Coord();

	list<Animation> mAnimations;
	int mAnimationTargetSquare = -1;
	int mAnimationTargetPiece = -1;
};

