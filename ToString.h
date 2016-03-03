/*
 * ToString.h
 *
 *  Created on: Aug 22, 2015
 *      Author: max
 */

#ifndef SRC_TOSTRING_H_
#define SRC_TOSTRING_H_

#include <string>

#include "types.h"

class ToString {
public:
  ToString();
  virtual ~ToString();

  void parseBB(bitboard bb, const char figure, char *output);
  void printBB(bitboard bb);
  void print(Board &board);
  void print(Board *board);
  void printDebug(Board &board);
  std::string squareNotation(square sq);
  std::string moveNotation(Move m);
  void printMove(Move m);
};

#endif /* SRC_TOSTRING_H_ */
