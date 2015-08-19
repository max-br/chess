#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdint.h>

#define MAX_POSSIBLE_MOVES 256

typedef uint64_t bitboard;
typedef unsigned char square;


#define PIECE_BB_SIZE 9

/*	BITS		USAGE
 *	0-5			from Square
 * 	6-11		to Square
 *	12-14		piece to move
 * 	15-17		piece to capture
 *	18-20		piece to promote to
 *	21-26		enpassant pawn capture
 *	27-29		movetype (for castling)
 *	30-31		none
*/
typedef unsigned int Move;
#define NULLMOVE 0
#define EMPTYBOARD 0

#define encodeFrom(X)       ((Move)X)
#define encodeTo(X)         (((Move)X)<<6)
#define encodePiece(X)      (((Move)X)<<12)
#define encodeCapture(X)    (((Move)X)<<15)
#define encodePromotion(X)  (((Move)X)<<18)
#define encodeEP(X)         (((Move)X)<<21)
#define encodeMovetype(X)   (((Move)X)<<27)

#define extractFrom(m)		((m)&0x3F)
#define extractTo(m)		(((m)>>6)&0x3F)
#define extractPiece(m)     (((m)>>12)&7)
#define extractCapture(m)	(((m)>>15)&7)
#define extractPromotion(m) (((m)>>18)&7)
#define extractEP(m)	    (((m)>>21)&0x3F)
#define extractMovetype(m)	(((m)>>27)&7)


enum Movetype{
	NONE,
	CASTLE,
};

enum Piece{
	EMPTY,
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING
	//WHITE,
	//BLACK
};

enum Color{
	WHITE = 7,
	BLACK = 8
};

enum Castlingrights{
	WKINGSIDE,
	WQUEENSIDE,
	BKINGSIDE,
	BQUEENSIDE
};

struct Line {
    int movecount;              // Number of moves in the line.
    Move moves[32];  			// The line.
};

struct Movelist{
	int 	count = 0;
	Move	moves[MAX_POSSIBLE_MOVES];
};

struct Moverecord{
	Move	move;
	Move 	en_passant[2];
	bool	castling_rights[4];
	Piece	capture;
	Color	us,them;
};

enum enumSquare {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8
};

#endif // #ifndef TYPES_H_INCLUDED
