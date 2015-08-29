/*
 * Search.cpp
 *
 *  Created on: Aug 22, 2015
 *      Author: max
 */

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <functional>
#include <string.h>

#include "Search.h"

#include "Board.h"
#include "Evaluate.h"
#include "Movegen.h"
#include "ToString.h"
#include "types.h"

Search::Search(Board* board,Evaluate* eval, Movegen* movegen){
	this->board = board;
	this->eval = eval;
	this->movegen = movegen;
}

Search::~Search() {
	// TODO Auto-generated destructor stub
}

int Search::alphaBeta(int depth, int ply, int alpha, int beta) {
    Movelist list;

    assert(-INFINITE <= alpha && alpha < beta && beta <= INFINITE);

    if (depth == 0) {
        return eval->evaluatePos(board);
    }

    movegen->genAllMoves(list);

    if(list.count == 0){
    	if(board->in_check){
    		return -32000 + ply; //mate
    	}else {
    		return 0; //stalemate
    	}
    }
    // sort movelist in descending order, captures are encoded in MSB, so they will be greater.
    std::sort(list.moves,list.moves+list.count,std::greater<int>());

    for (int i = 0; i < list.count;i++)  {
    	// only consider legal moves
    	if(board->makeMove(list.moves[i])){
            int val = -alphaBeta(depth - 1, ply + 1, -beta, -alpha);
            board->unmakeMove();
            if (val >= beta) return beta;
            if (val > alpha) {
                alpha = val;
            }
    	}else{
    		board->unmakeMove();
    	}
    }
    return alpha;
}


Move Search::bestMove(int depth)
{
	ToString tostring;
	Movelist list;
	Move best_move = NULLMOVE;
	int max_score = -64000;
	movegen->genAllMoves(list);
	for(int i = 0; i < list.count; i++){
		if(board->makeMove(list.moves[i])){
			int score = -alphaBeta(depth - 1, 1, -INFINITE, INFINITE);
			std::cout << score << " " << tostring.moveNotation(list.moves[i]) << std::endl;
			if(score > max_score){
				best_move = list.moves[i];
				max_score = score;
			}
		}
		board->unmakeMove();
	}
	return best_move;
}
