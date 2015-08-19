#ifndef MOVEGEN_H_INCLUDED
#define MOVEGEN_H_INCLUDED

#include <list>
#include <stdint.h>

#include "types.h"
#include "magicmoves.h"

extern "C" void initmagicmoves();

class Movegen
{
public:
	Board* board;
	bitboard between[64][64];

	bitboard pinned_pieces;

	Movegen(Board* board);

	void initAttackTable();
	void initBetweenTable();
	bitboard initKingMoves(const square& sq);
	bitboard initKnightMoves(const square& sq);


	inline Move encodeMove(const square& from, const square& to,
			const Piece& piece, const Piece& capture, const Piece& promotion, const square& EP, const Movetype& type);

	void genAllMoves(Movelist& list);

	void genCastles(Movelist& list);
	void genEvades(Movelist& list, const Color& color);
	void genPromotion(Movelist& list,const square& from, const square& to);


	void genWhitePawnMoves(Movelist& list);
	void genBlackPawnMoves(Movelist& list);
	void genBishopMoves(Movelist& list);
	void genKingMoves(Movelist& list);
	void genKnightMoves(Movelist& list);
	void genRookMoves(Movelist& list);
	void genQueenMoves(Movelist& list);

	bitboard pinnedPieces(const Color& color);

	bitboard getAttackers(const Color& color, const square& sq);
	bitboard getInterposingSquares(square from,square to);
	bitboard getMovers(const Color& color, const square& sq);
	bitboard getPawnMovers(const Color& color, const square& sq);

};

#endif // #ifndef MOVEGEN_H_INCLUDED
