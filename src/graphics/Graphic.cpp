#include "Graphic.h"

map<int, int> Graphic::mPieceToGraphic = {
	{Piece::white | Piece::king, 0},
	{Piece::white | Piece::queen, 1},
	{Piece::white | Piece::bishop, 2},
	{Piece::white | Piece::knight, 3},
	{Piece::white | Piece::rook, 4},
	{Piece::white | Piece::pawn, 5},
	{Piece::black | Piece::king, 6},
	{Piece::black | Piece::queen, 7},
	{Piece::black | Piece::bishop, 8},
	{Piece::black | Piece::knight, 9},
	{Piece::black | Piece::rook, 10},
	{Piece::black | Piece::pawn, 11}
};

const int Graphic::maxCapturedPieces[] = {
	0, 0, 8, 2, 0, 2, 2, 1,
	0, 0, 8, 2, 0, 2, 2, 1
};

Graphic::Graphic(int x, int y)
	: mPosX(x), mPosY(y), mIsFlipped(false)
{
	mpWindow = NULL;
	mpRenderer = NULL;
	
	for (int i = 0; i < 3; i++) {
		mpFonts[i] = NULL;
	}

	mpPiecesTextures = new SDL_Texture *[12];
	mpAvatarsTextures = new SDL_Texture *[2];

	mpCapturedPiecesTextures = new SDL_Texture **[16];
	for (int i = 0; i < 16; i++)
		mpCapturedPiecesTextures[i] = new SDL_Texture * [maxCapturedPieces[i]];
}

Graphic::~Graphic()
{
	delete[] mpPiecesTextures;
	delete[] mpAvatarsTextures;

	for (int i = 0; i < 16; i++)
		delete[] mpCapturedPiecesTextures[i];

	delete[] mpCapturedPiecesTextures;
}

bool Graphic::init() 
{
	// Initialize SDL. If failed, end program.
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		cout << "Error initializing SDL: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}

	// Create our window
	mpWindow = SDL_CreateWindow(
		WINDOW_TITLE,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	// Make sure creating the window succeeded. If failed, end program.
	if (!mpWindow) {
		cout << "Error creating window: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}

	//Make sure creating renderer succeeded. If failed, end program.
	mpRenderer = SDL_CreateRenderer(mpWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!mpRenderer) {
		cout << "Error creating renderer: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}

	// Enable blen mode
	SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND);
	
	//Make sure pieces textures load succeeded. If failed, end program.
	if (!this->loadPieces()) {
		return false;
	}	

	//Make sure captured pieces textures load succeeded. If failed, end program.
	if (!this->loadCapturedPieces()) {
		return false;
	}	

	//Make sure avatars textures load succeeded. If failed, end program.
	if (!this->loadAvatars()) {
		return false;
	}

	//Initialisation de SDL_TTF. If failed, end program.
	if (TTF_Init() == -1) {
		return false;
	}

	//Make sure font load succeeded. If failed, end program.
	if (!this->loadFont()) {

		return false;
	}

	return true;
}

void Graphic::kill() {
	//Free Memory
	for (int i = 0; i < 12; i++)
	{
		SDL_DestroyTexture(mpPiecesTextures[i]);
		mpPiecesTextures[i] = NULL;
	}

	for (int i = 0; i < 2; i++)
	{
		SDL_DestroyTexture(mpAvatarsTextures[i]);
		mpAvatarsTextures[i] = NULL;
	}

	for (int i = 0; i < 16; i++) 
	{
		for (int j = 0; j < maxCapturedPieces[i]; j++)
		{
			SDL_DestroyTexture(mpCapturedPiecesTextures[i][j]);
			mpCapturedPiecesTextures[i][j] = NULL;
		}
	}

	SDL_DestroyRenderer(mpRenderer);
	SDL_DestroyWindow(mpWindow);
	mpRenderer = NULL;
	mpWindow = NULL;

	// Quit SDL
	SDL_Quit();
}

void Graphic::drawBackground() {
	// Set Background color
	SDL_SetRenderDrawColor(mpRenderer, 49, 46, 43, 255);
	
	// Apply background color
	SDL_RenderClear(mpRenderer);
}

void Graphic::drawBoard() {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			drawSquare(
				{ mPosX + x * SQUARE_SIZE, mPosY + y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE }, 
				((y + x) % 2) ? Palette::dark : Palette::light
			);
		}
	}
}

void Graphic::drawCoordinate() {
	string fileC = mIsFlipped ? "8" : "1";
	string rankC = mIsFlipped ? "h" : "a";

	for (int i = 0; i < 8; i++) {
		int color = i % 2 ? 2 : 1;

		drawText(fileC, mPosX + 6, mPosY + (7 - i) * SQUARE_SIZE + 6, color, 24, FontStyle::bold);
		drawText(rankC, mPosX + i * SQUARE_SIZE + 80, mPosY + 8 * SQUARE_SIZE - 30, color, 24, FontStyle::bold);

		if (!mIsFlipped) {
			fileC[0]++;
			rankC[0]++;
		}
		else {
			fileC[0]--;
			rankC[0]--;
		}
	}
}

void Graphic::drawHighlightSquares(vector<int> *highlightSquares) { //TODO: Not draw last move when square is hightlighted 
	// Loop through highlights squares arrays
	for (int i = 0; i < PALETTE_HIGHLIGHT_SIZE; i++) {
		for (int square : highlightSquares[i]) {
			Coord coord = Coord(square);
			int flipOffset = mIsFlipped ? coord.getRank() : 7 - coord.getRank();
			drawSquare({ mPosX + coord.getFile() * SQUARE_SIZE, mPosY + flipOffset * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE }, Palette::highlight[i]);
		}
	}
}

void Graphic::drawHints(vector<Move> moves, int *squares) {
	for (Move move : moves) {

		Coord coord = Coord(move.getTargetSquare());
		int flipOffset = mIsFlipped ? coord.getRank() : 7 - coord.getRank();

		if (squares[move.getTargetSquare()] == Piece::none) {
			Geometry::SDL_RenderFillCircle(
				mpRenderer, 
				mPosX + coord.getFile() * SQUARE_SIZE + 50, mPosY + flipOffset * SQUARE_SIZE + 50,
				18, 
				Palette::hint
			);
		}
		else {
			Geometry::SDL_RenderHollowCircle(
				mpRenderer, 
				mPosX + coord.getFile() * SQUARE_SIZE + 50, mPosY + flipOffset * SQUARE_SIZE + 50,
				40, 
				48,
				Palette::hint
			);
		}
	}
}

//TODO: Remplacer par texture ?
void Graphic::drawHoverSquare(Coord coord) {
	SDL_Rect dest;

	// Defininf color of hover
	SDL_SetRenderDrawColor(mpRenderer, Palette::hover.r, Palette::hover.g, Palette::hover.b, Palette::hover.a);

	// Compute square position based on index and Adding square to Window
	int flipOffset = mIsFlipped ? coord.getRank() : 7 - coord.getRank();

	// Top
	dest = { mPosX + coord.getFile() * SQUARE_SIZE, mPosY + flipOffset * SQUARE_SIZE, SQUARE_SIZE, LINE_WIDTH };
	SDL_RenderFillRect(mpRenderer, &dest);

	// Left
	dest = { mPosX + coord.getFile() * SQUARE_SIZE, mPosY + flipOffset * SQUARE_SIZE + LINE_WIDTH,  LINE_WIDTH, SQUARE_SIZE - LINE_WIDTH * 2 };
	SDL_RenderFillRect(mpRenderer, &dest);

	// Right
	dest = { mPosX + (coord.getFile() + 1) * SQUARE_SIZE - LINE_WIDTH, mPosY + flipOffset * SQUARE_SIZE + LINE_WIDTH,  LINE_WIDTH, SQUARE_SIZE - LINE_WIDTH * 2 };
	SDL_RenderFillRect(mpRenderer, &dest);

	// Bottom
	dest = { mPosX + coord.getFile() * SQUARE_SIZE, mPosY + (flipOffset + 1) * SQUARE_SIZE - LINE_WIDTH, SQUARE_SIZE, LINE_WIDTH };
	SDL_RenderFillRect(mpRenderer, &dest);
}

void Graphic::drawPieces(int *squares, list<Animation> *animations, int draggedPiece, int pieceToPromote) {
	for (int y = 0; y < 8; y++)	{
		for (int x = 0; x < 8; x++) {
			int index = x + y * 8;

			if (squares[index]) {
				int piece = squares[index];
				bool toDraw = true;
				int offset = 0;

				if (index == draggedPiece || index == pieceToPromote)
					toDraw = false;

				for (Animation a : *animations) {
					if (index == a.getMove().getTargetSquare()) {
						if (Piece::pieceType(a.getTargetPiece()) != Piece::none)
							piece = a.getTargetPiece();
						else
							toDraw = false;
					}
				}

				if (toDraw)	{
					int flipOffset = mIsFlipped ? y : 7 - y;
					drawPiece(piece, mPosX + x * SQUARE_SIZE, mPosY + flipOffset * SQUARE_SIZE + offset);
				}
			}
		}
	}
}

void Graphic::drawPieceAtMouse(int draggedPiece) {
	int posX, posY;
	SDL_GetMouseState(&posX, &posY);

	drawPiece(draggedPiece, posX - SQUARE_SIZE / 2, posY - SQUARE_SIZE / 2);
}

void Graphic::drawPiece(int piece, int x, int y) {
	SDL_Rect dest = { x, y, SQUARE_SIZE, SQUARE_SIZE };
	SDL_RenderCopy(mpRenderer, mpPiecesTextures[Graphic::mPieceToGraphic[piece]], NULL, &dest);
}

void Graphic::drawPiece(int piece, Coord coord) {
	drawPiece(piece, mPosX + coord.getFile() * SQUARE_SIZE, mPosY + (7 - coord.getRank()) * SQUARE_SIZE);
}

void Graphic::drawPlayerInfos(Board *pBoard, Player &playerWhite, Player &playerBlack) {
	const int whiteX = mPosX + SQUARE_SIZE * 8 + 20;
	const int whiteY = mPosY + (mIsFlipped ? 0 : SQUARE_SIZE * 8 - 50);
	
	const int blackX = whiteX;
	const int blackY = mPosY + (!mIsFlipped ? 0 : SQUARE_SIZE * 8 - 50);

	// Draw white and black infos
	drawPlayerInfo(pBoard, playerWhite.name, true, whiteX, whiteY);
	drawPlayerInfo(pBoard, playerBlack.name, false, blackX, blackY);

	// Draw clocks
	drawClocks();
}

void Graphic::drawPlayerInfo(Board* pBoard, string str, bool isWhiteInfo, int x, int y) {
	int avatarIndex = isWhiteInfo ? Board::whiteIndex : Board::blackIndex;

	SDL_Rect dest = { x, y, 50, 50 };
	SDL_RenderCopy(mpRenderer, mpAvatarsTextures[avatarIndex], NULL, &dest);
	drawText(str, x + 60, y, 3, 20);
	drawCapturedPieces(pBoard, isWhiteInfo, x + 60, y + 28);
}

void Graphic::drawCapturedPieces(Board *pBoard, bool isWhiteInfo, int x, int y) {
	SDL_Rect dest;
	
	int startIndex = isWhiteInfo ? 8 : 0;
	int endIndex = isWhiteInfo ? 16 : 8;
	
	int colour = isWhiteInfo ? Piece::black : Piece::white;
	int opponentColour = isWhiteInfo ? Piece::white : Piece::black;
	int colourIndex = isWhiteInfo ? Board::blackIndex : Board::whiteIndex;

	// Displaying captured pieces textures
	for (int piece = startIndex; piece < endIndex; piece++)
	{
		int pieceType = Piece::pieceType(piece);
		int maxPiece = maxCapturedPieces[piece];
		int numPieceCaptured = maxPiece - pBoard->getPieceList(pieceType, colour)->count();

		if (numPieceCaptured < 1)
			continue;
		
		int w = 0, h = 0;
		SDL_QueryTexture(mpCapturedPiecesTextures[piece][numPieceCaptured - 1], NULL, NULL, &w, &h);
			
		dest = { x, y, w, h};
		SDL_RenderCopy(mpRenderer, mpCapturedPiecesTextures[piece][numPieceCaptured - 1], NULL, &dest);

		x += w + 2;
	}

	// Calculating score difference and displaying it
	int score = 0;
	for (int i = 0; i < 8; i++) {
		score += (pBoard->getPieceList(Piece::pieceType(i), opponentColour)->count() - pBoard->getPieceList(Piece::pieceType(i), colour)->count()) * Piece::simpleValue[i];
	}

	if (score > 0)
		drawText("+"s + std::to_string(score), x + 4, y + 2, 0, 14);
}

void Graphic::drawClocks() {
	drawText("3:00.0", mPosX + 820, mPosY + 4 * SQUARE_SIZE - 42, 0, 48);
	
	drawText("3:00.0", mPosX + 820, mPosY + 4 * SQUARE_SIZE + 10, 0, 48);
}

void Graphic::drawGameOver(bool whiteWin, bool isDraw) {
	string str = isDraw ? "Draw" : (whiteWin ? "White wins !" : "Black wins !");
	drawText(str, mPosX, 10, 0);
}

void Graphic::drawSquare(SDL_Rect dest, SDL_Colour colour) {
	// Setting the colour
	SDL_SetRenderDrawColor(mpRenderer, colour.r, colour.g, colour.b, colour.a);

	// Adding square to window
	SDL_RenderFillRect(mpRenderer, &dest);
}

void Graphic::drawText(string str, int x, int y, int colour, int size, FontStyle style) {
	if (colour < 0 || colour > PALETTE_TEXT_SIZE - 1) colour = 0; // Set default color if color not in palette range
	
	SDL_Rect dest;

	//Creating textures for characters
	TTF_SetFontSize(mpFonts[(int)style], size);
	SDL_Surface *pSurfaceBuffer = TTF_RenderText_Blended(mpFonts[(int)style], str.c_str(), Palette::text[colour]);
	SDL_Texture *pSurfaceTexture = SDL_CreateTextureFromSurface(mpRenderer, pSurfaceBuffer);

	//Make sure creating sprite succeeded
	if (!pSurfaceTexture) {
		cout << "Error creating character texture: " << SDL_GetError() << endl;
		return;
	}

	dest = { x, y, pSurfaceBuffer->w, pSurfaceBuffer->h };
	SDL_RenderCopy(mpRenderer, pSurfaceTexture, NULL, &dest);

	//Freeing temp surface
	SDL_FreeSurface(pSurfaceBuffer);
	SDL_DestroyTexture(pSurfaceTexture);
}

void Graphic::drawPromotionMenu(int squareToPromote) {
	SDL_Rect dest;

	int x = squareToPromote % 8;
	int y = squareToPromote / 8;

	if (y != 0 && y != 7)
		return;

	int colour = (y == 7) ? Piece::white : Piece::black;
	int yOffset = ((y == 7) == mIsFlipped) ? -3 * SQUARE_SIZE : 0;
	int yOffsetFactor = ((y == 7) == mIsFlipped) ? 1 : -1;

	int flipOffset = mIsFlipped ? y : 7 - y;
	dest = { mPosX + x * SQUARE_SIZE, mPosY + flipOffset * SQUARE_SIZE + yOffset, SQUARE_SIZE, SQUARE_SIZE * 4 };

	SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, 255);
	SDL_RenderFillRect(mpRenderer, &dest);

	dest.x += 1;
	dest.y += 1;
	dest.w -= 2;
	dest.h -= 2;

	SDL_SetRenderDrawColor(mpRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(mpRenderer, &dest);

	if (mIsFlipped)
		y = 7 - y;

	drawPiece(Piece::queen | colour, Coord(y, x));
	drawPiece(Piece::rook | colour, Coord(y + yOffsetFactor * 1, x));
	drawPiece(Piece::bishop | colour, Coord(y + yOffsetFactor * 2, x));
	drawPiece(Piece::knight | colour, Coord(y + yOffsetFactor * 3, x));
}

void Graphic::update() {
	SDL_RenderPresent(mpRenderer);
}

Coord Graphic::getBoardCoord(int y, int x) {
	x -= mPosX;
	y -= mPosY;

	if (x >= 0 && x <= SQUARE_SIZE * 8
		&& y >= 0 && y <= SQUARE_SIZE * 8)
	{
		x /= SQUARE_SIZE;
		y /= SQUARE_SIZE;

		if (!mIsFlipped)
			y = 7 - y;
	}
	else
	{
		x = -1;
		y = -1;
	}

	return Coord(y, x);
}

int Graphic::getPosX() {
	return mPosX;
}

int Graphic::getPosY() {
	return mPosY;
}

bool Graphic::isFlipped() {
	return mIsFlipped;
}

void Graphic::moveTo(int x, int y) {
	mPosX = x;
	mPosY = y;
}

void Graphic::flip() {
	mIsFlipped = !mIsFlipped;
}

bool Graphic::loadPieces()
{
	//Loading image with pieces
	SDL_Surface *pImageLoaded = IMG_Load(PATH_PIECES);
	if (!pImageLoaded) {
		cout << "Error loading image: " << SDL_GetError() << endl;
		return false;
	}

	//Creating textures for the 12 pieces
	SDL_Surface* pBuffer = NULL;
	for (int y = 0; y < 2; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			//Loading to temp surface
			SDL_Rect src = { x * PIECE_SPRITE_SIZE, y * PIECE_SPRITE_SIZE, PIECE_SPRITE_SIZE, PIECE_SPRITE_SIZE }; //Each sprite is 150x150
			pBuffer = SDL_CreateRGBSurface(0, PIECE_SPRITE_SIZE, PIECE_SPRITE_SIZE, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
			SDL_BlitSurface(pImageLoaded, &src, pBuffer, NULL);

			//Transfering surface to texture
			mpPiecesTextures[x + y * 6] = SDL_CreateTextureFromSurface(mpRenderer, pBuffer);

			//Freeing temp surface
			SDL_FreeSurface(pBuffer);

			//Make sure creating sprite succeeded
			if (!mpPiecesTextures[x + y * 6]) {
				cout << "Error creating texture: " << SDL_GetError() << endl;
				return false;
			}
		}
	}

	//Freeing main image
	SDL_FreeSurface(pImageLoaded);
	pImageLoaded = NULL;

	return true;
}

bool Graphic::loadCapturedPieces() {
	//Loading image with pieces
	SDL_Surface* pImageLoaded = IMG_Load(PATH_CAPTURED_PIECES);
	if (!pImageLoaded) {
		cout << "Error loading image: " << SDL_GetError() << endl;
		return false;
	}

	//Creating textures for the 12 pieces
	SDL_Surface* pBuffer = NULL;
	int xOffset = 0;
	for (int i = 0; i < 16; i++)
	{
		int pieceType = Piece::pieceType(i);
		int pieceWidth = 0;

		switch (pieceType) {
		case Piece::pawn:
			pieceWidth = CAPTURED_PIECES_SPRITE_PAWN_WIDTH;
			break;
		case Piece::queen:
			pieceWidth = CAPTURED_PIECES_SPRITE_QUEEN_WIDTH;
			break;
		default:
			pieceWidth = CAPTURED_PIECES_SPRITE_MINOR_WIDTH;
		}

		int maxPiece = maxCapturedPieces[i];
		for (int j = 0; j < maxPiece; j++)
		{
			//Loading to temp surface
			SDL_Rect src = { xOffset, (maxPiece - j - 1) * CAPTURED_PIECES_SPRITE_HEIGHT, pieceWidth, CAPTURED_PIECES_SPRITE_HEIGHT };
			pBuffer = SDL_CreateRGBSurface(0, pieceWidth, CAPTURED_PIECES_SPRITE_HEIGHT, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
			SDL_BlitSurface(pImageLoaded, &src, pBuffer, NULL);

			//Transfering surface to texture
			mpCapturedPiecesTextures[i][j] = SDL_CreateTextureFromSurface(mpRenderer, pBuffer);

			//Freeing temp surface
			SDL_FreeSurface(pBuffer);

			//Make sure creating sprite succeeded
			if (!mpCapturedPiecesTextures[i][j]) {
				cout << "Error creating texture: " << SDL_GetError() << endl;
				return false;
			}

			if (j < maxPiece - 1)
				pieceWidth += CAPTURED_PIECES_SPRITE_MULTIPLE_WIDTH;
		}

		if (maxPiece > 0)
			xOffset += pieceWidth;
	}

	//Freeing main image
	SDL_FreeSurface(pImageLoaded);
	pImageLoaded = NULL;

	return true;
}

bool Graphic::loadAvatars() {
	SDL_Surface* pImageLoaded = IMG_Load(PATH_AVATARS);
	if (!pImageLoaded) {
		cout << "Error loading image: " << SDL_GetError() << endl;
		return false;
	}
	
	// Creating texture for avatars
	SDL_Surface* pBuffer = NULL;
	for (int i = 0; i < 2; i++)	{
		//Loading to temp surface
		SDL_Rect src = { i * AVATAR_SPRITE_SIZE, 0, AVATAR_SPRITE_SIZE, AVATAR_SPRITE_SIZE };
		pBuffer = SDL_CreateRGBSurface(0, AVATAR_SPRITE_SIZE, AVATAR_SPRITE_SIZE, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
		SDL_BlitSurface(pImageLoaded, &src, pBuffer, NULL);

		//Transfering surface to texture
		mpAvatarsTextures[i] = SDL_CreateTextureFromSurface(mpRenderer, pBuffer);

		//Freeing temp surface
		SDL_FreeSurface(pBuffer);

		//Make sure creating sprite succeeded
		if (!mpAvatarsTextures[i]) {
			cout << "Error creating texture: " << SDL_GetError() << endl;
			return false;
		}
	}

	return true;
}

bool Graphic::loadFont() {
	const int defaultSize = 24;
	
	mpFonts[(int)FontStyle::normal] = TTF_OpenFont(PATH_FONT, defaultSize);
	mpFonts[(int)FontStyle::bold] = TTF_OpenFont(PATH_FONT_BOLD, defaultSize);
	mpFonts[(int)FontStyle::italic] = TTF_OpenFont(PATH_FONT_ITALIC, defaultSize);
	
	if (mpFonts == NULL)
		return false;

	return true;
}

void Graphic::debugDrawSquareIndex() {
	for (int y = 0; y < 8; y++)	{
		for (int x = 0; x < 8; x++)	{
			int flipOffset = mIsFlipped ? y : 7 - y;
			drawText(to_string(x + y * 8), mPosX + x * SQUARE_SIZE + 6, mPosY + flipOffset * SQUARE_SIZE + 70, 5, 24, FontStyle::bold);
		}
	}
}

void Graphic::debugDrawOccupiedSquares(PieceList** pieceList) {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < pieceList[i]->count(); j++) {
			Coord coord = Coord((*pieceList[i])[j]);
			int flipOffset = mIsFlipped ? coord.getRank() : 7 - coord.getRank();
			drawSquare({ mPosX + coord.getFile() * SQUARE_SIZE, mPosY + flipOffset * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE }, Palette::debug[i]);
		}
	}
}

void Graphic::debugDrawOccupiedSquares(int *occupiedList) {
	for (int i = 0; i < 2; i++) {
		Coord coord = Coord(occupiedList[i]);
		int flipOffset = mIsFlipped ? coord.getRank() : 7 - coord.getRank();
		drawSquare({ mPosX + coord.getFile() * SQUARE_SIZE, mPosY + flipOffset * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE }, Palette::debug[i]);
	}
}

void Graphic::debugDrawAttackedSquares(bool *attackedSquares, int colour) {
	for (int i = 0; i < 64; i++) {
		if (!attackedSquares[i])
			continue;

		Coord coord = Coord(i);
		int flipOffset = mIsFlipped ? coord.getRank() : 7 - coord.getRank();
		drawSquare({ mPosX + coord.getFile() * SQUARE_SIZE, mPosY + flipOffset * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE }, Palette::debug[colour]);
	}
}