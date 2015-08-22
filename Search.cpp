/*
 * Search.cpp
 *
 *  Created on: Aug 22, 2015
 *      Author: max
 */

#include <assert.h>
#include <string.h>

#include "Search.h"

#include "Board.h"
#include "Evaluate.h"
#include "Movegen.h"
#include "types.h"

Search::Search(Board* board,Evaluate* eval, Movegen* movegen){
	this->board = board;
	this->eval = eval;
	this->movegen = movegen;
}

Search::~Search() {
	// TODO Auto-generated destructor stub
}

int Search::alphaBeta(int depth, int ply, int alpha, int beta, Line* line_ptr) {
    Line line;
    Movelist list;

    assert(-INFINITE <= alpha && alpha < beta && beta <= INFINITE);

    if(board->inCheck && depth == 0){
    	depth++;
    }

    if (depth == 0) {
    	line_ptr->movecount = 0;
        return eval->evaluatePos(board);
    }

    movegen->genAllMoves(list);

    if(list.count == 0){
    	if(board->inCheck){
    		return -32000 + ply; //mate
    	}else {
    		return 0; //stalemate
    	}
    }

    for (int i = 0; i < list.count;++i)  {
    	// only consider legal moves
    	if(board->makeMove(list.moves[i])){
            int val = -alphaBeta(depth - 1, ply + 1, -beta, -alpha, &line);
            board->unmakeMove();
            if (val >= beta) return beta;
            if (val > alpha) {
                alpha = val;
                line_ptr->moves[0] = list.moves[i];
                memcpy(line_ptr->moves + 1, line.moves, line.movecount * sizeof(Move));
                line_ptr->movecount = line.movecount + 1;
            }
    	}else{
    		board->unmakeMove();
    	}
    }
    return alpha;
}


Move Search::bestMove(int depth)
{
	Line line;
	alphaBeta(depth,0,-INFINITE,INFINITE,&line);
	return line.moves[0];
}
