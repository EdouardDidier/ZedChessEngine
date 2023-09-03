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

Graphic::Graphic(int x, int y)
	: mPosX(x), mPosY(y), mIsFlipped(false)
{
	mpWindow = NULL;
	mpRenderer = NULL;
	mpFont = NULL;

	mpPiecesTextures = new SDL_Texture *[12];
}

Graphic::~Graphic()
{
	delete mpPiecesTextures;
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
	

	//Make sure sprites load succeeded. If failed, end program.
	if (!this->loadPieces()) {

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

		drawText(fileC, mPosX + 6, mPosY + (7 - i) * SQUARE_SIZE + 6, color);
		drawText(rankC, mPosX + i * SQUARE_SIZE + 80, mPosY + 8 * SQUARE_SIZE - 30, color);

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

	this->drawPiece(draggedPiece, posX - SQUARE_SIZE / 2, posY - SQUARE_SIZE / 2);
}

void Graphic::drawPiece(int piece, int x, int y) {
	SDL_Rect dest = { x, y, SQUARE_SIZE, SQUARE_SIZE };
	SDL_RenderCopy(mpRenderer, mpPiecesTextures[Graphic::mPieceToGraphic[piece]], NULL, &dest);
}

void Graphic::drawPiece(int piece, Coord coord) {
	drawPiece(piece, mPosX + coord.getFile() * SQUARE_SIZE, mPosY + (7 - coord.getRank()) * SQUARE_SIZE);
}

void Graphic::drawGameOver(bool whiteWin, bool isDraw) {
	string str = isDraw ? "Draw" : (whiteWin ? "White wins !" : "Black wins !");
	drawText(str, 10, 10, 0);
}

void Graphic::drawSquare(SDL_Rect dest, SDL_Colour colour) {
	// Setting the colour
	SDL_SetRenderDrawColor(mpRenderer, colour.r, colour.g, colour.b, colour.a);

	// Adding square to window
	SDL_RenderFillRect(mpRenderer, &dest);
}

void Graphic::drawText(string str, int x, int y, int color) {
	if (color < 0 || color > PALETTE_TEXT_SIZE - 1) color = 0; // Set default color if color not in palette range
	
	SDL_Rect dest;
	int w, h;

	for (char& c : str) {
		SDL_QueryTexture(mpCharTextures[color][c], NULL, NULL, &w, &h);
		dest = { x, y, w, h };
		SDL_RenderCopy(mpRenderer, mpCharTextures[color][c], NULL, &dest);

		x += w;
	}
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
	SDL_Surface* pBuffer = new SDL_Surface ;
	for (int y = 0; y < 2; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			//Loading to temp surface
			SDL_Rect src = { x * SPRITE_SIZE, y * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE }; //Each sprite is 150x150
			pBuffer = SDL_CreateRGBSurface(0, SPRITE_SIZE, SPRITE_SIZE, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
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

bool Graphic::loadFont() {
	mpFont = TTF_OpenFont(PATH_FONT, 24);

	if (mpFont == NULL)
		return false;

	//Creating textures for characters
	SDL_Surface *pBuffer = new SDL_Surface;
	for (int i = 0; i < PALETTE_TEXT_SIZE; i++) {
		for (int j = 32; j < 127; j++) // Computing all characters from space (32) to '~' (126)
		{

			//Loading to temp surface
			pBuffer = TTF_RenderText_Blended(mpFont, (const char*)&j, Palette::text[i]);

			//Transfering surface to texture
			mpCharTextures[i].insert({ (char)j, SDL_CreateTextureFromSurface(mpRenderer, pBuffer) });

			//Freeing temp surface
			SDL_FreeSurface(pBuffer);

			//Make sure creating sprite succeeded
			if (!mpCharTextures[i][(char)j]) {
				cout << "Error creating character texture: " << SDL_GetError() << endl;
				return false;
			}
		}
	}

	return true;
}

void Graphic::debugDrawSquareIndex() {
	for (int y = 0; y < 8; y++)	{
		for (int x = 0; x < 8; x++)	{
			int flipOffset = mIsFlipped ? y : 7 - y;
			drawText(to_string(x + y * 8), mPosX + x * SQUARE_SIZE + 6, mPosY + flipOffset * SQUARE_SIZE + 70, 3);
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