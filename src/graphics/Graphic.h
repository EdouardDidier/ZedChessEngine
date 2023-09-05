#pragma once

#define WINDOW_TITLE "ZedChess"
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

#define SQUARE_SIZE 100

#define LINE_WIDTH 5

#define PATH_PIECES "resources\\graphics\\pieces\\pieces.png"
#define PIECE_SPRITE_SIZE 100

#define PATH_CAPTURED_PIECES "resources\\graphics\\pieces\\capturedPieces.png"
#define CAPTURED_PIECES_SPRITE_HEIGHT 18
#define CAPTURED_PIECES_SPRITE_PAWN_WIDTH 12
#define CAPTURED_PIECES_SPRITE_MINOR_WIDTH 15
#define CAPTURED_PIECES_SPRITE_QUEEN_WIDTH 18
#define CAPTURED_PIECES_SPRITE_MULTIPLE_WIDTH 7

#define PATH_AVATARS "resources\\graphics\\avatars\\avatars.png"
#define AVATAR_SPRITE_SIZE 400

#define PATH_FONT "resources\\fonts\\arial.ttf"
#define PATH_FONT_BOLD "resources\\fonts\\G_ari_bd.TTF"
#define PATH_FONT_ITALIC "resources\\fonts\\G_ari_i.TTF"


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

#include "./src/engine/Board.h"
#include "./src/engine/Piece.h"
#include "./src/engine/PieceList.h"
#include "./src/engine/Move.h"

using namespace std;

class Animation;

class Graphic
{
public:
	enum class FontStyle {
		normal, bold, italic
	};
public:
	Graphic(int x, int y);
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

	//TODO: Redo highlights square systems
	//		Add arraws draw
	//		Add player infos	Done
	//		Add captured pieces	Done
	//		Add a font manager
	//		Add capcuted pieces score
	//		Add clocks
	void drawPlayerInfo(Board *pBoard);
	void drawCapturedPieces(Board* pBoard, int x, int y, bool isWhiteInfo);
	void drawClocks();

	void drawGameOver(bool whiteWin, bool isDraw);

	void drawSquare(SDL_Rect dest, SDL_Colour colour);
	void drawText(string str, int x, int y, int colour = 0, int size = 24, FontStyle style = FontStyle::normal);

	void drawPromotionMenu(int squareToPromote);

	void update();

	Coord getBoardCoord(int x, int y);

	int getPosX();
	int getPosY();
	bool isFlipped();

	void moveTo(int x, int y);
	void flip();

	bool loadPieces();
	bool loadCapturedPieces();
	bool loadAvatars();
	bool loadFont();

	// Info / Debug functions
	void debugDrawSquareIndex();
	void debugDrawOccupiedSquares(PieceList **pieceList);
	void debugDrawOccupiedSquares(int *occupiedList);
	void debugDrawAttackedSquares(bool* attackedSquares, int colour = 2);
private:
	static map<int, int> mPieceToGraphic;
	static const int maxCapturedPieces[];

	// Pointers to our window and surface
	SDL_Window *mpWindow;
	SDL_Renderer *mpRenderer;
	SDL_Texture **mpPiecesTextures;
	SDL_Texture ***mpCapturedPiecesTextures;
	SDL_Texture **mpAvatarsTextures;

	map<char, SDL_Texture *> mpCharTextures[PALETTE_TEXT_SIZE];

	TTF_Font *mpFonts[3];

	int mPosX;
	int mPosY;

	bool mIsFlipped;
};