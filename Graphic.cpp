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

Graphic::Graphic()
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

// Apply background color
void Graphic::drawBackground() {
	// Set Background
	SDL_SetRenderDrawColor(mpRenderer, 49, 46, 43, 255);
	SDL_RenderClear(mpRenderer);
}

void Graphic::drawBoard() {
	SDL_Rect dest;

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {

			// Defining color of square
			if ((y + x) % 2) {
				SDL_SetRenderDrawColor(mpRenderer, Palette::dark.r, Palette::dark.g, Palette::dark.b, Palette::dark.a);
			}
			else {
				SDL_SetRenderDrawColor(mpRenderer, Palette::light.r, Palette::light.g, Palette::light.b, Palette::light.a);
			}

			// Compute square position based on coordinates and screen size
			dest = { (x - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2, (y - 4) * SQUARE_SIZE + WINDOW_HEIGHT / 2, SQUARE_SIZE, SQUARE_SIZE };

			// Adding square to window
			SDL_RenderFillRect(mpRenderer, &dest);
		}
	}
}

void Graphic::drawCoordinate() {
	string fileC = "1";
	string rankC = "a";

	for (int i = 0; i < 8; i++) {
		int color = i % 2 ? 2 : 1;
		drawText(fileC, -4 * SQUARE_SIZE + WINDOW_WIDTH / 2 + 6, (7 - i - 4) * SQUARE_SIZE + WINDOW_HEIGHT / 2 + 6, color);
		drawText(rankC, (i - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2 + 80, 4 * SQUARE_SIZE + WINDOW_HEIGHT / 2 - 30, color);
		fileC[0]++;
		rankC[0]++;
	}
}

void Graphic::drawHighlightSquares(vector<int> *highlightSquares) { //TODO: Not draw last move when square is hightlighted 
	SDL_Rect dest;

	// Loop through highlights squares arrays
	for (int i = 0; i < PALETTE_HIGHLIGHT_SIZE; i++) {
		for (int square : highlightSquares[i]) {
			Coord coord = Coord(square);

			// Defininf color of highlight
			SDL_SetRenderDrawColor(mpRenderer, Palette::highlight[i].r, Palette::highlight[i].g, Palette::highlight[i].b, Palette::highlight[i].a);
			
			// Compute square position based on index
			dest = { (coord.getFile() - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2, (7 - coord.getRank() - 4) * SQUARE_SIZE + WINDOW_HEIGHT / 2, SQUARE_SIZE, SQUARE_SIZE };

			// Adding square to window
			SDL_RenderFillRect(mpRenderer, &dest);
		}
	}
}

void Graphic::drawHints(list<Move> moves, int *squares) {
	for (Move move : moves) {

		Coord coord = Coord(move.getTargetSquare());
		if (squares[move.getTargetSquare()] == Piece::none) {
			Geometry::SDL_RenderFillCircle(
				mpRenderer, 
				(coord.getFile() - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2 + 50, 
				(7 - coord.getRank() - 4) * SQUARE_SIZE + WINDOW_HEIGHT / 2 + 50, 
				18, 
				Palette::hint
			);
		}
		else {
			Geometry::SDL_RenderHollowCircle(
				mpRenderer, 
				(coord.getFile() - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2 + 50, (7 - coord.getRank() - 4) * SQUARE_SIZE + WINDOW_HEIGHT / 2 + 50, 
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
	// Top
	dest = { (coord.getFile() - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2, (7 - coord.getRank() - 4) * SQUARE_SIZE + WINDOW_HEIGHT / 2, SQUARE_SIZE, LINE_WIDTH };
	SDL_RenderFillRect(mpRenderer, &dest);

	// Left
	dest = { (coord.getFile() - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2, (7 - coord.getRank() - 4) * SQUARE_SIZE + WINDOW_HEIGHT / 2 + LINE_WIDTH,  LINE_WIDTH, SQUARE_SIZE - LINE_WIDTH * 2 };
	SDL_RenderFillRect(mpRenderer, &dest);

	// Right
	dest = { (coord.getFile() - 3) * SQUARE_SIZE + WINDOW_WIDTH / 2 - LINE_WIDTH, (7 - coord.getRank() - 4) * SQUARE_SIZE + WINDOW_HEIGHT / 2 + LINE_WIDTH,  LINE_WIDTH, SQUARE_SIZE - LINE_WIDTH * 2 };
	SDL_RenderFillRect(mpRenderer, &dest);

	// Bottom
	dest = { (coord.getFile() - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2, (7 - coord.getRank() - 3) * SQUARE_SIZE + WINDOW_HEIGHT / 2 - LINE_WIDTH, SQUARE_SIZE, LINE_WIDTH };
	SDL_RenderFillRect(mpRenderer, &dest);
}

void Graphic::drawPieces(int *squares, int draggedPiece, list<Animation> *animations) {
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			int index = x + y * 8;

			if (squares[index]) {
				int piece = squares[index];
				bool toDraw = true;
				int offset = 0;

				if (index == draggedPiece)
					toDraw = false;

				for (Animation a : *animations) {
					if (index == a.getMove().getTargetSquare()) {
						if (Piece::pieceType(a.getTargetPiece()) != Piece::none)
							piece = a.getTargetPiece();
						else
							toDraw = false;
					}
				}

				if (toDraw)	this->drawPiece(piece, (x - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2, (7 - y - 4) * SQUARE_SIZE + WINDOW_HEIGHT / 2 + offset);
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
	SDL_Rect dest;

	dest = { x, y, SQUARE_SIZE, SQUARE_SIZE };
	SDL_RenderCopy(mpRenderer, mpPiecesTextures[Graphic::mPieceToGraphic[piece]], NULL, &dest);
}

void Graphic::drawPiece(int piece, Coord coord) {
	this->drawPiece(piece, (coord.getFile() - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2, (7 - coord.getRank() - 4)* SQUARE_SIZE + WINDOW_HEIGHT / 2);
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

void Graphic::update() {
	SDL_RenderPresent(mpRenderer);
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


void Graphic::drawSquareIndex() {
	int i = 0;

	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			this->drawText(to_string(i), (x - 4) * SQUARE_SIZE + WINDOW_WIDTH / 2 + 6, (7 - y - 4) * SQUARE_SIZE + WINDOW_HEIGHT / 2 + 70, 3);
			i++;
		}
	}
}