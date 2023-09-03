#include "Fen.h"

const string Fen::fenStartPosition = /**/ "rnb1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; /*/ "r2qk2r / ppp2ppp / 2n5 / 3n4 / PbPpp1bP / 1P6 / 3PPPP1 / RNBQKBNR b KQkq c3 0 1"; // */
// Mate in 10 ply with forced knight promotion "8/pRP4p/6pk/3r4/1P3p2/4p2P/P4PP1/6K1 b - - 0 1"
// Standard starting position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
// Mate in 2: "r2qkb1r/pp2nppp/3p4/2pNN1B1/2BnP3/3P4/PPP2PPP/R2bK2R w KQkq - 1 0"
// En passant capture that leves a check "8/6bb/8/8/R1pP2k1/4P3/P7/K7 b - d3" "8/6bb/8/8/R1p3k1/4P3/P2P/K7 w - -"
// 218 possible moves "R6R / 3Q4 / 1Q4Q1 / 4Q3 / 2Q4Q / Q4Q2 / pp1Q4 / kBNNK1B1 w - -0 1"

map<char, int> Fen::symbolToPiece = {
	{'k', Piece::king},
	{'q', Piece::queen},
	{'b', Piece::bishop},
	{'n', Piece::knight},
	{'r', Piece::rook},
	{'p', Piece::pawn}
};

Fen::Fen() {

}

Fen::~Fen() {

}

Fen::Position Fen::positionFromFen(string fen) {
	Position position = Position();

	// Spliting the string into 6 parts
	// 1: Position of pieces on the board
	// 2: Color to play
	// 3: Castle flags
	// 4: "en passant" possiblity
	// 5: Counter of half moves since last capture (for the fifty-mover rule)
	// 6: Number of full move
	stringstream ss(fen);
	string section[6];
	for (int i = 0; i < 6 && getline(ss, section[i], ' '); i++);

	int file = 0;
	int rank = 7;

	// Looping through pieces positions
	for (char symbol : section[0]) {
		if (symbol == '/') {
			file = 0;
			rank--;
		} else {
			if (symbol >= '0' && symbol <= '9') {
				file += int(symbol - '0');
			} else { 
				position.squares[rank * 8 + file] = isupper(symbol) ? (Piece::white | symbolToPiece[tolower(symbol)]) : (Piece::black | symbolToPiece[tolower(symbol)]);
				file++;
			}
		}
	}

	// Defining turn to play
	position.whiteToMove = (section[1] == "w");

	// Defining castle rights
	position.whiteCastleKingside = (section[2].find("K") != string::npos);
	position.whiteCastleQueenside = (section[2].find("Q") != string::npos);
	position.blackCastleKingside = (section[2].find("k") != string::npos);
	position.blackCastleQueenside = (section[2].find("q") != string::npos);

	// Defining en passant rights
	string enPassantFileName = section[3];
	if (enPassantFileName.find("-") == string::npos) {
		position.epFile = BoardRepresentation::getSquareFromString(enPassantFileName) + 1;
	}

	// Defining half-move clock
	position.fiftyMoveCounter = atoi(section[4].c_str());
	
	// Defining fullmove clock
	position.moveCount = atoi(section[5].c_str());

	return position;
}