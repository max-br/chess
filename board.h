#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <stack>
#include <stdint.h>
#include <string>

#include "types.h"

class Board
{

public:

	bitboard piece_BB[PIECE_BB_SIZE];
	bitboard square_mask[64];
	Piece squares[64];
	std::stack<Moverecord> move_stack;

	// 1 - en passant to the left, 2 - en passant to the right
	// we need two in the (rare) case that there is two pawns beside a double pushed pawn
	Move en_passant[2];
	Move castle_moves[4];
	bool castling_rights[4];
	int fullmove_number, halfmove_number;

	Piece move_color;

	Piece flipColor(const Piece& color) const {return color == WHITE ? BLACK : WHITE;}

	Board();
	
	bitboard getAllPieces() const {return piece_BB[WHITE] | piece_BB[BLACK];}
	bitboard getAllPieces(Piece color) const {return color == WHITE ? piece_BB[WHITE] : piece_BB[BLACK];}
	bitboard getBlackPieces() const {return piece_BB[BLACK];}
	bitboard getWhitePieces() const {return piece_BB[WHITE];}

	bitboard getPawns(Piece color) const {return piece_BB[PAWN] & piece_BB[color];}
	bitboard getKnights(Piece color) const {return piece_BB[KNIGHT] & piece_BB[color];}
	bitboard getBishops(Piece color) const {return piece_BB[BISHOP] & piece_BB[color];}
	bitboard getRooks(Piece color) const {return piece_BB[ROOK] & piece_BB[color];}
	bitboard getQueens(Piece color) const {return piece_BB[QUEEN] & piece_BB[color];}
	bitboard getKings(Piece color) const {return piece_BB[KING] & piece_BB[color];}

	bitboard getPawns() const {return piece_BB[PAWN];}
	bitboard getKnights() const {return piece_BB[KNIGHT];}
	bitboard getBishops() const {return piece_BB[BISHOP];}
	bitboard getRooks() const {return piece_BB[ROOK];}
	bitboard getQueens() const {return piece_BB[QUEEN];}
	bitboard getKings() const {return piece_BB[KING];}

	bool checkIntegrity();
	void checkEnPassant(const square& from, const square& to, const Piece& piece, const Piece& color);
	void checkLegalCastles(const Piece& color);


	void updateCastlingRights(const Move& move, const Piece& color);

	void makeCastleMove(const Move& move, const Piece& color, Moverecord& record);
	void makePromotionMove(const Move& move, const Piece& color, Moverecord& record);
	void makeEPMove(const Move& move, const Piece& color, Moverecord& record);
	void makeMove(const Move& move, const Piece& color);

	void unmakeCastleMove(const Moverecord& lastmove);
	void unmakeEPMove(const Moverecord& lastmove);
	void unmakePromotionMove(const Moverecord& lastmove);
	void unmakeMove();

	void setupFEN(const std::string fen);
	void resetBoard();


	void parseBB(bitboard bb,const char figure, char* output) const;
	void printDebug() const;
	void printBoard() const;
	void printBB(bitboard bb) const;
	void printMove(Move m) const;

	std::string squareNotation(square sq);
	std::string moveNotation(Move m);

};

#endif // #ifndef BOARD_H_INCLUDED

