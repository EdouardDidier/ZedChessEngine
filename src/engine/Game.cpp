#include "Game.h"

Game::Game(Board *pBoard) {
	// Initilizing empty arrays for highlighted square tacking
	for (int i = 0; i < PALETTE_HIGHLIGHT_SIZE; i++) {
		mHighlightSquares[i] = vector<int>();
	}

	if (pBoard == NULL) {
		// Creating the chess data
		mpBoard = new Board();

		// Load start position from Fen string
		mpBoard->loadStartPosition();
	}
	else {
		mpBoard = pBoard;
		isPrivateBoard = false;
	}

	mSearchWhite.init(mpBoard);
	mSearchBlack.init(mpBoard);
}

Game::~Game()
{
	if (isPrivateBoard)
		delete mpBoard;
}

bool Game::init()
{
	//Initializing graphics. If failed, end the program.
	if (!mGraphic.init())
		return false;

	if (!mAudio.init())
		return false;

	// No piece selected at start
	mSelectedSquare = -1;
	mDraggedPiece = -1;
	mToUnselectFlag = false;

	mHoverSquare = Coord();

	// Generate first set of legal moves
	mLegalMoves = mMoveGenerator.generateLegalMove(mpBoard, mCapturedOnly);

	return true;
}

void Game::kill()
{
	mGraphic.kill();
}

void Game::run()
{
	bool quit = false;
	Uint64 delay = 0;

	mAudio.playSound(Audio::gameStart);

	while (!quit)
	{
		float elapsed = mTimer.getElapsedTime();
		//cout << "FPS: " << to_string(1.0f / (elapsed / 1000.0f)) << endl;
		//SDL_Delay(floor(8.333f - elapsed / 1000.0f));

		// Clear ended animations
		list<Animation>::iterator it = mAnimations.begin();
		while(it != mAnimations.end()) {
			if (it->isComplete()) {
				it = mAnimations.erase(it);
				continue;
			}
			it++;
		}

		// Draw background
		mGraphic.drawBackground();

		// Draw the chess board
		mGraphic.drawBoard();

		// Draw coordinate
		mGraphic.drawCoordinate();

		// Draw highligted squares
		mGraphic.drawHighlightSquares(mHighlightSquares);

		// Draw hints (legal moves)
		if (mSelectedSquare > -1) {
			mGraphic.drawHints(mPossibleMoves, mpBoard->squares);
		}

		// Draw te pieces on the board
		int promotionMove = mPromotionMove.isInvalid() ? -1 : mPromotionMove.getStartSquare();
		mGraphic.drawPieces(mpBoard->squares, &mAnimations, mDraggedPiece, promotionMove);

		if (mDraggedPiece > -1) {
			if (mHoverSquare.isInBoard()) {
				mGraphic.drawHoverSquare(mHoverSquare);						// Draw hover square 
			}
			mGraphic.drawPieceAtMouse(mpBoard->getPiece(mDraggedPiece));	// Draw dragged piece
		}

		// Animate piece when making a move
		for (list<Animation>::iterator it = mAnimations.begin(); it != mAnimations.end(); it++) {
			it->animate(elapsed);
		}
		
		if (mDebugMode) {
			if (mDebugOccupiedSelector == 1)
				mGraphic.debugDrawOccupiedSquares(mpBoard->kings);
			else if (mDebugOccupiedSelector > 1)
				mGraphic.debugDrawOccupiedSquares(DebugUtility::getPieceListFromSelector(mpBoard, mDebugOccupiedSelector));

			if (mAttackedSquareSelector == 1)
				mGraphic.debugDrawAttackedSquares(mMoveGenerator.attackedSquares);
			else if (mAttackedSquareSelector == 2) {
				mGraphic.debugDrawAttackedSquares(mMoveGenerator.checkRaySquares);
				mGraphic.debugDrawAttackedSquares(mMoveGenerator.pinRaySquares, 3);
			}

			mGraphic.debugDrawSquareIndex();
		}

		// Draw promotion menu
		if (!mPromotionMove.isInvalid()) {
			mGraphic.drawPromotionMenu(mPromotionMove.getTargetSquare());
		}

		if (mIsGameOver)
			mGraphic.drawGameOver(!mpBoard->whiteToMove, mIsDraw);

		// Update the screen
		mGraphic.update();
		
		// Handle IA play
		if ((1 || mpBoard->colourToMove == Piece::black) && !mIsGameOver && !mIsPaused) {
			delay += (Uint64)elapsed;
			if (delay > 200) {
				iaPlay();
				mTimer.getElapsedTime();
				delay = 0;
			}
		}

		// Handle events on queue. Return True if quit flag activated
		quit = handleUserEvents();
	}
}

bool Game::handleUserEvents() {
	SDL_Event e;

	// Get Mouse state
	int x = 0, y = 0;
	Uint32 pMouseState = SDL_GetMouseState(&x, &y);

	// Get keyboard state
	const Uint8 *pKeyboardState = SDL_GetKeyboardState(NULL);

	// Loop through events in queue
	while (SDL_PollEvent(&e) != 0) {
		if (handleGeneralEvents(e, x, y))
			return true;

		if (mPromotionMove.isInvalid())
			handleGameEvents(e, pMouseState, pKeyboardState, x, y);
		else
			handlePromotionMenuEvents(e, x, y);
	}

	return false;
}

bool Game::handleGeneralEvents(SDL_Event &e, int x, int y) {

	switch (e.type) {
	case SDL_QUIT:
		return true;
	case SDL_MOUSEMOTION:
		mHoverSquare = Coord(getBoardCoord(y, x));
		break;
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym) {
		case SDLK_KP_0:
			mDebugMode = !mDebugMode;
			break;
		case SDLK_KP_1:
			mDebugOccupiedSelector++;
			if (mDebugOccupiedSelector > 6) mDebugOccupiedSelector = 0;
			break;
		case SDLK_KP_2:
			mAttackedSquareSelector++;
			if (mAttackedSquareSelector > 2) mAttackedSquareSelector = 0;
			break;
		case SDLK_KP_9:
			mCapturedOnly = !mCapturedOnly;
			mLegalMoves.clear();
			mLegalMoves = mMoveGenerator.generateLegalMove(mpBoard, mCapturedOnly);
			selectSquare(mSelectedSquare);
			break;
		}
	}

	return false;
}

void Game::handleGameEvents(SDL_Event &e, Uint32 pMouseState, const Uint8 *pKeyboardState, int x, int y) {
	switch (e.type) {
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_LEFT:
			mIsPaused = true;
			undoMove();
			break;
		case SDLK_RIGHT:
			mIsPaused = true;
			redoMove();
			break;
		case SDLK_r:
			mPromotionMove = Move(0);

			unSelectSquare();
			clearHighlightSquares(0);

			mIsGameOver = false;
			mIsDraw = false;

			mpBoard->reset();
			mLegalMoves = mMoveGenerator.generateLegalMove(mpBoard, mCapturedOnly);
			break;
		case SDLK_SPACE:
			mIsPaused = !mIsPaused;
			break;
		}
	case SDL_MOUSEBUTTONDOWN:
		mHoverSquare = Coord(getBoardCoord(y, x));

		switch (e.button.button) {
		case SDL_BUTTON_LEFT:
		{
			// Clear all hightlighted squares
			if (mHoverSquare.isInBoard())
				clearHighlightSquares();

			Move move = Move(mSelectedSquare, mHoverSquare.getIndex());

			if (tryMove(move, true)) {
				unSelectSquare();
			}
			else if (mPromotionMove.isInvalid()) {
				if (mpBoard->getPiece(mHoverSquare) > 0) {									// If selecting another piece, start dragging it
					if (mSelectedSquare == mHoverSquare.getIndex()) mToUnselectFlag = true; // Flag it able to be unselected

					selectSquare(mHoverSquare.getIndex());
					mDraggedPiece = mSelectedSquare;
				}
				else {
					unSelectSquare(1);
				}
			}

			break;
		}
		case SDL_BUTTON_RIGHT:
			unSelectSquare(1);
			break;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		switch (e.button.button) {
		case SDL_BUTTON_LEFT:
		{
			Move move = Move(mSelectedSquare, mHoverSquare.getIndex());

			// If a piece is being dragged, attempt to move
			if (mHoverSquare.getIndex() == mSelectedSquare && mToUnselectFlag) {
				unSelectSquare();
			}
			else if (isInPossibleMove(move)) {
				if (tryMove(move)) {
					unSelectSquare();
				}
			}

			mDraggedPiece = -1; // Reset dragged index

			break;
		}
		case SDL_BUTTON_RIGHT:
			if (!(pMouseState & SDL_BUTTON(1))) {	// Selected square only if left mouse button isn't pressed
				if (mHoverSquare.isInBoard())
				{
					// Adding highlight squares to correct vector depending on pressed keys
					if (pKeyboardState[SDL_SCANCODE_LCTRL] || pKeyboardState[SDL_SCANCODE_RCTRL])
						addHighlightSquare(mHoverSquare.getIndex(), 3);
					else if (pKeyboardState[SDL_SCANCODE_LSHIFT] || pKeyboardState[SDL_SCANCODE_RSHIFT])
						addHighlightSquare(mHoverSquare.getIndex(), 4);
					else if (pKeyboardState[SDL_SCANCODE_LALT] || pKeyboardState[SDL_SCANCODE_RALT])
						addHighlightSquare(mHoverSquare.getIndex(), 5);
					else
						addHighlightSquare(mHoverSquare.getIndex(), 2);
				}
			}
			break;
		}
		break;
	}
}

void Game::handlePromotionMenuEvents(SDL_Event &e, int x, int y) {
	switch (e.type) {
	case SDL_MOUSEBUTTONDOWN:
		switch (e.button.button) {
		case SDL_BUTTON_LEFT:

			Coord hoverSquare = getBoardCoord(y, x);

			if (hoverSquare.isInBoard()) {
				Coord squareToPromote = Coord(mPromotionMove.getTargetSquare());

				if (hoverSquare.getFile() == squareToPromote.getFile()) {
					int selection = squareToPromote.getRank() - hoverSquare.getRank();
					int coulour = Piece::white;
					int flag = Move::Flag::promoteToQueen;

					if (squareToPromote.getRank() == 0) {
						coulour = Piece::black;
						selection = -selection;
					}

					switch(selection) {
					case 1:
						flag = Move::Flag::promoteToRook;
						break;
					case 2:
						flag = Move::Flag::promoteToBishop;
						break;
					case 3:
						flag = Move::Flag::promoteToKnight;
						break;
					}

					makeMove(Move(mPromotionMove.moveValue | (flag << 12)));
					unSelectSquare();
				}
			}

			mPromotionMove = Move(0);

			break;
		}
	}
}

void Game::selectSquare(int square) {
	vector<int> alreadyPushedMove;

	if (square == -1)
		return;

	mPossibleMoves.clear();

	for (Move move : mLegalMoves) {
		if (move.getStartSquare() == square) {
			// In case of promotion, just add one of the legalmove 
			// to avoid to display multiple moves one the same square (one for each kind of promotion)
			if (move.isPromotion()) {
				bool toAdd = true;
				for (int i : alreadyPushedMove) {
					if (move.getTargetSquare() == i) {
						toAdd = false;
					}
				}
				if (toAdd) {
					alreadyPushedMove.push_back(move.getTargetSquare());
					mPossibleMoves.push_back(move);
				}
			}
			else {
				mPossibleMoves.push_back(move);
			}
		}
	}

	mSelectedSquare = square;
	addHighlightSquare(square, 1, true); // Select new piece
}

void Game::unSelectSquare(int type) {
	mPossibleMoves.clear();

	mSelectedSquare = -1;		// Reset selected index
	mDraggedPiece = -1;			// Reset dragged index
	mToUnselectFlag = false;	// Reset Flag

	clearHighlightSquares(type);	// Clear selected square
}

bool Game::isInPossibleMove(Move move) {
	for (Move legal : mPossibleMoves)
		if (Move::isSameMoveNoFlag(move, legal))
			return true;

	return false;
}

bool Game::tryMove(Move move, bool isAnimated) {
	for (Move legal : mPossibleMoves) {
		if (Move::isSameMoveNoFlag(move, legal)) {
			// Check if is promotion, then open promotion menu
			if (legal.isPromotion()) {
				if (isAnimated)
					mAnimations.push_back(Animation(&mGraphic, Move(move.moveValue), mpBoard->getPiece(move.getStartSquare()), 0));	// Starting animation

				mPromotionMove = Move(move.moveValue);
				selectSquare(move.getStartSquare());

				return false;
			}
			else if (isAnimated)
				makeAnimatedMove(legal);
			else
				makeMove(legal);
			return true;
		}
	}

	return false;
}

bool Game::makeMove(Move move) {
	int targetPiece = Piece::pieceType(mpBoard->getPiece(move.getTargetSquare()));

	mAnimations.clear();

	mHighlightSquares[0].clear();
	mHighlightSquares[0].push_back(move.getStartSquare());
	mHighlightSquares[0].push_back(move.getTargetSquare());

	if (move.isCastle()) {
		Move rookMove = Move::getCastleRookMove(move); 
		mAnimations.push_front(Animation(&mGraphic, Move(rookMove.moveValue), mpBoard->getPiece(rookMove.getStartSquare()), Piece::none));	// Starting animation
	}

	mpBoard->makeMove(move);

	mLegalMoves = mMoveGenerator.generateLegalMove(mpBoard, mCapturedOnly);

	// Checking if game is over
	if (mpBoard->isRepetition()) {
		mIsGameOver = true;
		mIsDraw = true;
		mLegalMoves.clear();
	}
	else if (mLegalMoves.empty()) {
		mIsGameOver = true;
		
		if (!mMoveGenerator.inCheck)
			mIsDraw = true;
	}

	playSound(move, targetPiece, mMoveGenerator.inCheck);

	selectSquare(mSelectedSquare);

	return true;
}

bool Game::makeAnimatedMove(Move move) {
	int animatedPiece = mpBoard->getPiece(move.getStartSquare());
	int targetPiece = mpBoard->getPiece(move.getTargetSquare());

	makeMove(move);

	mAnimations.push_back(Animation(&mGraphic, Move(move.moveValue), animatedPiece, targetPiece));	// Starting animation

	return true;
}

bool Game::redoMove() {
	Move move = mpBoard->getFirstRedoMove();

	if (move.isInvalid())
		return false;

	int animatedPiece = mpBoard->getPiece(move.getStartSquare());
	int targetPiece = mpBoard->getPiece(move.getTargetSquare());

	mAnimations.clear();

	mHighlightSquares[0].clear();
	mHighlightSquares[0].push_back(move.getStartSquare());
	mHighlightSquares[0].push_back(move.getTargetSquare());

	if (move.isCastle()) {
		Move rookMove = Move::getCastleRookMove(move);
		mAnimations.push_front(Animation(&mGraphic, Move(rookMove.moveValue), mpBoard->getPiece(rookMove.getStartSquare()), Piece::none));	// Starting animation
	}

	mpBoard->redoMove();

	mAnimations.push_back(Animation(&mGraphic, Move(move.moveValue), animatedPiece, targetPiece));	// Starting animation

	mLegalMoves = mMoveGenerator.generateLegalMove(mpBoard, mCapturedOnly);

	playSound(move, targetPiece, mMoveGenerator.inCheck);

	return true;
}

bool Game::undoMove() {
	Move move = mpBoard->getLastMove();

	if (move.isInvalid())
		return false;

	int animatedPiece = mpBoard->getPiece(move.getTargetSquare());
	int targetPiece = mpBoard->getPiece(move.getStartSquare());

	mAnimations.clear();

	if (move.isCastle()) {
		Move rookMove = Move::getCastleRookMove(move);
		mAnimations.push_front(Animation(&mGraphic, Move(rookMove.getTargetSquare(), rookMove.getStartSquare()), mpBoard->getPiece(rookMove.getTargetSquare()), Piece::none));	// Starting animation
	}

	mpBoard->undoMove();

	mAnimations.push_back(Animation(&mGraphic, Move(move.getTargetSquare(), move.getStartSquare()), animatedPiece, targetPiece));	// Starting animation

	mLegalMoves = mMoveGenerator.generateLegalMove(mpBoard, mCapturedOnly);

	playSound(move, targetPiece, mMoveGenerator.inCheck);

	move = mpBoard->getLastMove();
	mHighlightSquares[0].clear();

	if (!move.isInvalid()) {
		mHighlightSquares[0].push_back(move.getStartSquare());
		mHighlightSquares[0].push_back(move.getTargetSquare());
	}

	return true;
}

bool Game::iaPlay() {
	Move move;

	if (mpBoard->colourToMove == Piece::white) {
		mSearchWhite.searchMove(6);
		move = mSearchWhite.getBestMove();
	}
	else {
		mSearchBlack.searchMove(6);
		move = mSearchBlack.getBestMove();
	}

	if (!move.isInvalid()) {
		makeAnimatedMove(move);
		return true;
	}

	return false;
}

Coord Game::getBoardCoord(int y, int x)
{
	x -= WINDOW_WIDTH / 2 - SQUARE_SIZE * 4;
	y -= WINDOW_HEIGHT / 2 - SQUARE_SIZE * 4;

	if (x >= 0 && x <= SQUARE_SIZE * 8
		&& y >= 0 && y <= SQUARE_SIZE * 8)
	{
		x /= SQUARE_SIZE;
		y /= SQUARE_SIZE;

		y = 7 - y; // Reverse Y coordinate to read board from bottom to top
	}
	else
	{
		x = -1;
		y = -1;
	}

	return Coord(y, x);
}

void Game::addHighlightSquare(int square, int type, bool persist) {
	bool toUnselect = false;

	// Looping through hightlight vector to check if square il already highlighted
	for (int i = 1; i < PALETTE_HIGHLIGHT_SIZE; i++) {

		// Finding and deleting duplicated instances
		auto it = mHighlightSquares[i].begin();
		while (it != mHighlightSquares[i].end()) {

			// Remove squares that are already highlighted
			if (square == *it) {
				if (type == i) toUnselect = true;
				it = mHighlightSquares[i].erase(it);
			}
			else {
				it++;
			}
		}
	}

	if (!toUnselect || persist) mHighlightSquares[type].push_back(square);
}

void Game::clearHighlightSquares(int type) {
	// Looping through hightlight vector an freeing memory
	if (type > -1 && type < PALETTE_HIGHLIGHT_SIZE) {
		mHighlightSquares[type].clear();
	} else {
		for (int i = 1; i < PALETTE_HIGHLIGHT_SIZE; i++) { // No clear of last move
			mHighlightSquares[i].clear();
		}
	}
}

void Game::playSound(Move move, int targetPiece, bool inCheck) {
	if (inCheck) {
		if (mIsGameOver)
			mAudio.playSound(Audio::gameEnd);
		else
			mAudio.playSound(Audio::moveCheck);
	}
	else if (move.isPromotion())
		mAudio.playSound(Audio::promote);
	else if ((targetPiece != Piece::none)
		|| move.isEnPassant())
		mAudio.playSound(Audio::capture);
	else if (move.isCastle())
		mAudio.playSound(Audio::castle);
	else
		mAudio.playSound(Piece::isColour(mpBoard->getPiece(move.getTargetSquare()), Piece::white) ? Audio::moveSelf : Audio::moveOpponent);

}