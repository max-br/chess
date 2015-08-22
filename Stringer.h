/*
 * ToString.h
 *
 *  Created on: Aug 22, 2015
 *      Author: max
 */

#ifndef SRC_STRINGER_H_
#define SRC_STRINGER_H_

#include <string>

#include "types.h"

class Stringer{
public:
	Stringer();
	Stringer(Board* board);
	virtual ~Stringer();

	Board* board;

	void parseBB(bitboard bb,const char figure, char* output);
	void printBB(bitboard bb);
	void printBoard();
	void printDebug();
	std::string squareNotation(square sq);
	std::string moveNotation(Move m);
	void printMove(Move m);

};

#endif /* SRC_STRINGER_H_ */
