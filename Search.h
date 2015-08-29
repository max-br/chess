/*
 * Search.h
 *
 *  Created on: Aug 22, 2015
 *      Author: max
 */

#ifndef SRC_SEARCH_H_
#define SRC_SEARCH_H_

#include "Evaluate.h"
#include "Movegen.h"
#include "types.h"

class Search {
public:
	Search(Board* board,Evaluate* eval, Movegen* movegen);
	virtual ~Search();

	Board* board;
	Evaluate* eval;
	Movegen* movegen;


	int alphaBeta(int depth, int ply, int alpha, int beta, Line* line_ptr);
	Move bestMove(int depth);
};

#endif /* SRC_SEARCH_H_ */
