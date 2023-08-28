#pragma once

#define WINDOW_TITLE "ZedChess"
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

#define SQUARE_SIZE 100

#define LINE_WIDTH 5

#define PATH_PIECES "resources\\graphics\\sprites\\pieces.png"
#define SPRITE_SIZE 100

#define PATH_FONT "resources\\fonts\\G_ari_bd.TTF"


#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include "Animation.h"

#include "Geometry.h"
#include "Palette.h"
#include "Coord.h"

#include "Piece.h"
#include "PieceList.h"
#include "Move.h"

using namespace std;

class Animation;

class Graphic
{
public:
	Graphic();
	~Graphic();

	bool init();
	void kill();

	void drawBackground();
	void drawBoard();
	void drawCoordinate();
	void drawHighlightSquares(vector<int>* highlightSquares);
	void drawHints(vector<Move> moves, int* squares);
	void drawHoverSquare(Coord coord);
	void drawPieces(int* squares, list<Animation> *animations, int draggedPiece, int pieceToPromote);

	void drawPieceAtMouse(int draggedPiece);
	void drawPiece(int piece, int x, int y);
	void drawPiece(int piece, Coord coord);

	void drawGameOver(bool whiteWin, bool isDraw);

	void drawSquare(SDL_Rect dest, SDL_Colour colour);
	void drawText(string str, int x, int y, int color = 0);

	void drawPromotionMenu(int squareToPromote);

	void update();

	bool loadPieces();
	bool loadFont();

	// Info / Debug functions
	void debugDrawSquareIndex();
	void debugDrawOccupiedSquares(PieceList **pieceList);
	void debugDrawOccupiedSquares(int *occupiedList);
	void debugDrawAttackedSquares(bool* attackedSquares, int colour = 2);

private:
	static map<int, int> mPieceToGraphic;

	// Pointers to our window and surface
	SDL_Window *mpWindow;
	SDL_Renderer *mpRenderer;
	SDL_Texture **mpPiecesTextures;

	map<char, SDL_Texture *> mpCharTextures[PALETTE_TEXT_SIZE];

	TTF_Font* mpFont;
};