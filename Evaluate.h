/*
 * Evaluate.h
 *
 *  Created on: Mar 9, 2015
 *      Author: max
 */

#include "Board.h"

#ifndef EVALUATE_H_
#define EVALUATE_H_

class Evaluate {
public:
  Evaluate();
  virtual ~Evaluate();

  // color * Piecetype * squares
  int square_value[2][PIECE_BB_SIZE][64];
  std::string square_value_init[PIECE_BB_SIZE];

  void initValueTables();

  int evaluatePos(const Board *board);
  int piecePosition(bitboard &pieces, const Color &color, const Piece &type);

  int material(const Board *board, const Color &color);
  int position(const Board *board, const Color &color);
};

#endif /* EVALUATE_H_ */
