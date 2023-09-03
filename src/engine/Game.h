#pragma once
#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>

#include <future>

#include "./src/graphics/Timer.h"

#include "./src/audio/Audio.h"

#include "./src/graphics/Graphic.h"
#include "./src/graphics/Animation.h"
#include "./src/graphics/Coord.h"

#include "Board.h"
#include "Move.h"
#include "MoveGenerator.h"

#include "./src/ai/Search.h"
#include "./src/ai/old/SearchOld.h"

#include "./src/utility/DebugUtility.h"

using namespace std;

class Game
{
public:
	Game(Board *pBoard);
	~Game();

	bool init();
	void kill();

	void run();
	bool handleUserEvents();
	bool handleGeneralEvents(SDL_Event &e, int x, int y);
	void handleGameEvents(SDL_Event &e, Uint32 pMouseState, const Uint8 *pKeyboardState, int x, int y);
	void handlePromotionMenuEvents(SDL_Event &e, int x, int y);

	void selectSquare(int startSquare);
	void unSelectSquare(int typeHighlight = -1);

	bool isInPossibleMove(Move move);

	bool tryMove(Move move, bool isAnimated = false);
	bool makeMove(Move move);
	bool makeAnimatedMove(Move move);

	bool redoMove();
	bool undoMove();

	bool iaPlay();

	void addHighlightSquare(int square, int type, bool persist = false);
	void clearHighlightSquares(int typeHighlight = -1);

	void playSound(Move move, int targetPiecex, bool inCheck);

private:
	Move asyncSearch();

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
	
	SearchV5 mSearchWhite;
	SearchV5 mSearchBlack;
	std::future<Move> mSearchMoveResult;

	MoveGenerator mMoveGenerator;
	vector<Move> mLegalMoves;
	vector<Move> mPossibleMoves;

	int mSelectedSquare = -1;
	int mDraggedPiece = -1;
	bool mToUnselectFlag = false; // Flag to check if selected piece is ready to be unselected

	Coord mHoverSquare = Coord();

	list<Animation> mAnimations;

	Move mPromotionMove = Move(0);

	bool mIsGameOver = false;
	bool mIsDraw = false;
	bool mIsPaused = false;

	bool mDebugMode = false;
	int mAttackedSquareSelector = 0;
	int mDebugOccupiedSelector = 0;
	bool mCapturedOnly = false;
};

