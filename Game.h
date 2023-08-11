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

#include "DebugUtility.h"

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
	bool handleGeneralEvents(SDL_Event e, int x, int y);
	void handleGameEvents(SDL_Event e, Uint32 pMouseState, const Uint8 *pKeyboardState, int x, int y);
	void handlePromotionMenuEvents(SDL_Event e, int x, int y);

	void selectSquare(int startSquare);
	void unSelectSquare(int typeHighlight = -1);

	bool isInPossibleMove(Move move);

	bool tryMove(Move move, bool isAnimated = false);
	bool makeMove(Move move);
	bool makeAnimatedMove(Move move);

	bool iaPlay();

	void gameOver();

	Coord getBoardCoord(int y, int x);

	void addHighlightSquare(int square, int type, bool persist = false);
	void clearHighlightSquares(int typeHighlight = -1);

	void playSound(Move move, int targetPiece, bool inCheck);

private:
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

	Move mPromotionMove = Move(0);

	bool mIsGameOver = false;

	bool mDebugMode = false;
	int mAttackedSquareSelector = 0;
	int mDebugOccupiedSelector = 0;
};

