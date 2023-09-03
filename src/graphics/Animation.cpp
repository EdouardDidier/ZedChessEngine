#include "Animation.h"

Animation::Animation(Graphic *pGraphic, Move move, int piece, int targetPiece) {
	mpGraphic = pGraphic;

	mMove = move;
	mPiece = piece;
	mTargetPiece = targetPiece;

	Coord mStartPos = Coord(move.getStartSquare());
	Coord mTargetPos = Coord(move.getTargetSquare());

	int flipOffset = pGraphic->isFlipped() ? mStartPos.getRank() : 7 - mStartPos.getRank();
	mStartX = pGraphic->getPosX() + mStartPos.getFile() * SQUARE_SIZE;
	mStartY = pGraphic->getPosY() + flipOffset * SQUARE_SIZE;


	flipOffset = pGraphic->isFlipped() ? mTargetPos.getRank() : 7 - mTargetPos.getRank();
	mTargetX = pGraphic->getPosX() + mTargetPos.getFile() * SQUARE_SIZE;
	mTargetY = pGraphic->getPosY() + flipOffset * SQUARE_SIZE;

	mProgress = 0;
}

bool Animation::animate(float elapsed) {
	if (mPiece == -1) return true;

	mProgress += elapsed;

	float progress = min(mProgress / ANIMATION_TIME, 1.0f);

	int posX = mStartX + (int)((mTargetX - mStartX) * progress);
	int posY = mStartY + (int)((mTargetY - mStartY) * progress);

	mpGraphic->drawPiece(mPiece, posX, posY);

	if (mProgress > ANIMATION_TIME) 
		mPiece = -1;

	return false;
}

Move Animation::getMove() {
	return mMove;
}

int Animation::getPiece() {
	return mPiece;
}

int Animation::getTargetPiece() {
	return mTargetPiece;
}

bool Animation::isComplete() {
	return mPiece == -1;
}

void Animation::end() {

}