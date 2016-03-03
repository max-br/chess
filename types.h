#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdint.h>

#define MAX_POSSIBLE_MOVES 256

typedef uint64_t bitboard;
typedef unsigned char square;

class Board;

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

#define INFINITE 32000
#define NULLMOVE 0
#define EMPTYBOARD 0

#define encodeFrom(X) ((Move)X)
#define encodeTo(X) (((Move)X) << 6)
#define encodePiece(X) (((Move)X) << 12)
#define encodePromotion(X) (((Move)X) << 15)
#define encodeEP(X) (((Move)X) << 18)
#define encodeMovetype(X) (((Move)X) << 24)
#define encodeCapture(X) (((Move)X) << 27)

#define extractFrom(m) ((m)&0x3F)
#define extractTo(m) (((m) >> 6) & 0x3F)
#define extractPiece(m) (((m) >> 12) & 7)
#define extractPromotion(m) (((m) >> 15) & 7)
#define extractEP(m) (((m) >> 18) & 0x3F)
#define extractMovetype(m) (((m) >> 24) & 7)
#define extractCapture(m) (((m) >> 27) & 7)

enum Movetype {
  NONE,
  CASTLE,
};

enum Piece {
  EMPTY,
  PAWN,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING
  // WHITE,
  // BLACK
};

enum Color { WHITE = 7, BLACK = 8 };

enum Castlingrights { WKINGSIDE, WQUEENSIDE, BKINGSIDE, BQUEENSIDE };

struct Movelist {
  int count = 0;
  Move moves[MAX_POSSIBLE_MOVES];
};

struct Moverecord {
  Move move;
  Move en_passant[2];
  bool castling_rights[4];
  Piece capture;
  Color us, them;
};

#endif // #ifndef TYPES_H_INCLUDED
