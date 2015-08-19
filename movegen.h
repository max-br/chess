#ifndef MOVEGEN_H_INCLUDED
#define MOVEGEN_H_INCLUDED

#include <list>
#include <stdint.h>

#include "types.h"
#include "board.h"
#include "magicmoves.h"

extern int popCount(bitboard bb);
extern int bitScanForward(bitboard bb);
extern int pop_lsb(bitboard &bb);

extern "C" void initmagicmoves();


class Movegen
{
public:
	bitboard king_moves[64];
	bitboard knight_moves[64];
	bitboard between[64][64];

	bitboard pinned_pieces;

	Movegen();

	void initAttackTable();
	void initBetweenTable();
	bitboard initKingMoves(const square& sq);
	bitboard initKnightMoves(const square& sq);


	inline Move encodeMove(const square& from, const square& to,
			const Piece& piece, const Piece& capture, const Piece& promotion, const square& EP, const Movetype& type);

	void genAllMoves(const Board& board, Movelist& list, const Piece& color);

	void genCastles(const Board& board, Movelist& list, const Piece& color);
	void genEvades(const Board& board, Movelist& list, const Piece& color);
	void genPromotion(const Board& board, Movelist& list,const square& from, const square& to);


	void genWhitePawnMoves(const Board& board, Movelist& list);
	void genBlackPawnMoves(const Board& board, Movelist& list);
	void genBishopMoves(const Board& board, Movelist& list, const Piece& color);
	void genKingMoves(const Board& board, Movelist& list,const Piece& color);
	void genKnightMoves(const Board& board, Movelist& list, const Piece& color);
	void genRookMoves(const Board& board, Movelist& list, const Piece& color);
	void genQueenMoves(const Board& board, Movelist& list, const Piece& color);

	bool isAttacked(const Board& board, const Piece& color, const square& sq);
	bool isInCheck(const Board& board, const Piece& color);

	bitboard pinnedPieces(const Board& board, const Piece& color);

	bitboard getAttackers(const Board& board, const Piece& color, const square& sq);
	bitboard getInterposingSquares(square from,square to);
	bitboard getMovers(const Board& board, const Piece& color, const square& sq);
	bitboard getPawnMovers(const Board& board, const Piece& color, const square& sq);

};

#endif // #ifndef MOVEGEN_H_INCLUDED
