/*
 * directions.h
 *
 *  Created on: Mar 8, 2015
 *      Author: max
 */
#ifndef SRC_DIRECTIONS_H_
#define SRC_DIRECTIONS_H_

#include "types.h"

#define RANK_3 0xFF0000
#define RANK_6 0xFF0000000000

#define CLEAR_FILE_A 0xfefefefefefefefe
#define CLEAR_FILE_H 0x7f7f7f7f7f7f7f7f
#define CLEAR_FILE_G_H 0x3f3f3f3f3f3f3f3f
#define CLEAR_FILE_A_B 0xfcfcfcfcfcfcfcfc

/* Directions:
* noWe         nort         noEa
*         +7    +8    +9
*		      \  |  /
* west    -1 <-  0 -> +1    east
*             /  |  \
*         -9    -8    -7
* soWe         sout         soEa
*/
#define NOWE 7
#define NORT 8
#define NOEA 9
#define WEST 1
#define EAST 1
#define SOWE 9
#define SOUT 8
#define SOEA 7

inline bitboard soutOne(bitboard b) { return b >> 8; }
inline bitboard nortOne(bitboard b) { return b << 8; }

inline bitboard eastOne(bitboard bb) { return (bb << 1) & CLEAR_FILE_A; }
inline bitboard noEaOne(bitboard bb) { return (bb << 9) & CLEAR_FILE_A; }
inline bitboard soEaOne(bitboard bb) { return (bb >> 7) & CLEAR_FILE_A; }
inline bitboard westOne(bitboard bb) { return (bb >> 1) & CLEAR_FILE_H; }
inline bitboard soWeOne(bitboard bb) { return (bb >> 9) & CLEAR_FILE_H; }
inline bitboard noWeOne(bitboard bb) { return (bb << 7) & CLEAR_FILE_H; }

#endif /* SRC_DIRECTIONS_H_ */
