#include "Game.h"

Game::Game()
{
	srand((unsigned)time(NULL));

	// Initilizing empty arrays for highlighted square tacking
	for (int i = 0; i < PALETTE_HIGHLIGHT_SIZE; i++) {
		mHighlightSquares[i] = vector<int>();
	}

	// Creating the chess data
	mpBoard = new Board();
}

Game::~Game()
{

}

bool Game::init()
{
	// No piece selected at start
	mSelectedSquare = -1;
	mDraggedPiece = -1;
	mToUnselectFlag = false;

	mHoverSquare = Coord();

	// Load start position from Fen string
	mpBoard->loadStartPosition();

	// Generate first set of legal moves
	mLegalMoves = mMoveGenerator.generateLegalMove(mpBoard);

	//Initializing graphics. If failed, end the program.
	if (!mGraphic.init())
		return false;

	if (!mAudio.init())
		return false;

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
			mGraphic.drawHints(mPossibleMoves, mpBoard->pSquares);
		}

		// Draw te pieces on the board
		mGraphic.drawPieces(mpBoard->pSquares, mDraggedPiece, &mAnimations);

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
		
		// Display additionnal information to debug
		if (mDebugMode)
			mGraphic.drawSquareIndex();

		// Update the screen
		mGraphic.update();

		// Handle IA play
		if (true || mpBoard->colourToMove == Piece::black) {
			delay += (Uint64)elapsed;
			if (delay > 1000) {
				delay = 0;
				this->iaPlay();
			}
		}

		// Handle events on queue. Return True if quit flag activated
		quit = this->handleUserEvents();
	}
}

bool Game::handleUserEvents() {
	SDL_Event e;

	// Get Mouse state
	int x, y;
	Uint32 pMouseState = SDL_GetMouseState(&x, &y);

	// Get keyboard state
	const Uint8 *pKeyboardState = SDL_GetKeyboardState(NULL);

	// Loop through events in queue
	while (SDL_PollEvent(&e) != 0) {
		switch (e.type) {
		case SDL_QUIT:
			return true;
		case SDL_MOUSEMOTION:
		{
			Coord selectedSquare = this->getBoardCoord(y, x);
			mHoverSquare = Coord(selectedSquare);

			break;
		}
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_r:
				unSelectSquare(); 
				clearHighlightSquares(0);

				mpBoard->loadStartPosition(); //TODO: create resest fonction
				mLegalMoves = mMoveGenerator.generateLegalMove(mpBoard);
				break;
			case SDLK_d:
				mDebugMode = !mDebugMode;
				break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
		{
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
				else if (mpBoard->getPiece(mHoverSquare) > 0) {								// If selecting another piece, start dragging it
					if (mSelectedSquare == mHoverSquare.getIndex()) mToUnselectFlag = true; // Flag it able to be unselected

					selectSquare(mHoverSquare.getIndex());
					mDraggedPiece = mSelectedSquare;
				}
				else {
					unSelectSquare(1);
				}

				break;
			}
			case SDL_BUTTON_RIGHT:
				unSelectSquare(1);					
				break;
			}
			break;
		}
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
					this->tryMove(move);
					unSelectSquare();
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
							this->addHighlightSquare(mHoverSquare.getIndex(), 3);
						else if (pKeyboardState[SDL_SCANCODE_LSHIFT] || pKeyboardState[SDL_SCANCODE_RSHIFT])
							this->addHighlightSquare(mHoverSquare.getIndex(), 4);
						else if (pKeyboardState[SDL_SCANCODE_LALT] || pKeyboardState[SDL_SCANCODE_RALT])
							this->addHighlightSquare(mHoverSquare.getIndex(), 5);
						else
							this->addHighlightSquare(mHoverSquare.getIndex(), 2);
					}
				}
				break;
			}
			break;
		}
	}

	return false;
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

	this->clearHighlightSquares(type);	// Clear selected square
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
			if (isAnimated)
				this->makeAnimatedMove(legal);
			else
				this->makeMove(legal);
			return true;
		}
	}

	return false;
}

bool Game::makeMove(Move move) {
	mAnimations.clear();

	mHighlightSquares[0].clear();
	mHighlightSquares[0].push_back(move.getStartSquare());
	mHighlightSquares[0].push_back(move.getTargetSquare());

	if (move.isCastle()) {
		Move rookMove = Move::getCastleRookMove(move); 
		mAnimations.push_front(Animation(&mGraphic, Move(rookMove.moveValue), mpBoard->getPiece(rookMove.getStartSquare()), Piece::none));	// Starting animation
	}

	this->playSound(move);

	mpBoard->makeMove(move);

	mLegalMoves = mMoveGenerator.generateLegalMove(mpBoard);
	this->selectSquare(mSelectedSquare);

	return true;
}

bool Game::makeAnimatedMove(Move move) {
	int animatedPiece = mpBoard->getPiece(move.getStartSquare());
	int targetPiece = mpBoard->getPiece(move.getTargetSquare());

	this->makeMove(move);

	mAnimations.push_back(Animation(&mGraphic, Move(move.moveValue), animatedPiece, targetPiece));	// Starting animation

	return true;
}

bool Game::iaPlay() {
	int chosenMove = rand() % mLegalMoves.size();
	int i = 0;

	for (Move move : mLegalMoves) {
		if (chosenMove == i) {
			this->makeAnimatedMove(move);
			return true;
		}
		
		i++;
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

void Game::playSound(Move move) {
	if ((Piece::pieceType(mpBoard->getPiece(move.getTargetSquare())) != Piece::none)
		|| move.isEnPassant()) {
		mAudio.playSound(Audio::capture);
	}
	else if (move.isCastle())
		mAudio.playSound(Audio::castle);
	else
		mAudio.playSound(Piece::isColour(mpBoard->getPiece(move.getTargetSquare()), Piece::white) ? Audio::moveSelf : Audio::moveOpponent);

}