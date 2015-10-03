/*
 * Evaluate.cpp
 *
 *  Created on: Mar 9, 2015
 *      Author: max
 */

#include <iostream>
#include <string>
#include <sstream>

#include "bitfunc.h"
#include "Evaluate.h"
#include "types.h"

Evaluate::Evaluate() {
	square_value_init[PAWN] =   "0 0 0 0 0 0 0 0 "
								"50 50 50 50 50 50 50 50 "
								"10 10 20 30 30 20 10 10 "
			 	 	 	 	 	"5 5 10 25 25 10 5 5 "
			 	 	 	 	 	"0 0 0 20 20 0 0 0 "
			 	 	 	 	 	"5 -5 -10 0 0 -10 -5 5 "
			 	 	 	 	 	"5 10 10 -20 -20 10 10 5 "
			 	 	 	 	 	"0 0 0 0 0 0 0 0 ";

	square_value_init[KNIGHT] = "-50 -40 -30 -30 -30 -30 -40 -50 "
								"-40 -20 0 0 0 0 -20 -40 "
								"-30 0 10 15 15 10 0 -30 "
								"-30 5 15 20 20 15 5 -30 "
								"-30 0 15 20 20 15 0 -30 "
								"-30 5 10 15 15 10 5 -30 "
								"-40 -20 0 5 5 0 -20 -40 "
								"-50 -40 -30 -30 -30 -30 -40 -50 ";

	square_value_init[BISHOP] = "-20 -10 -10 -10 -10 -10 -10 -20 "
								"-10 0 0 0 0 0 0 -10 "
								"-10 0 5 10 10 5 0 -10 "
								"-10 5 5 10 10 5 5 -10 "
								"-10 0 10 10 10 10 0 -10 "
								"-10 10 10 10 10 10 10 -10 "
								"-10 5 0 0 0 0 5 -10 "
								"-20 -10 -10 -10 -10 -10 -10 -20 ";

	square_value_init[ROOK] = 	"0 0 0 0 0 0 0 0 "
								"5 10 10 10 10 10 10 5 "
								"-5 0 0 0 0 0 0 -5 "
								"-5 0 0 0 0 0 0 -5 "
								"-5 0 0 0 0 0 0 -5 "
								"-5 0 0 0 0 0 0 -5 "
								"-5 0 0 0 0 0 0 -5 "
								"0 0 0 5 5 0 0 0 ";

	square_value_init[QUEEN] =	"-20 -10 -10 -5 -5 -10 -10 -20 "
								"-10 0 0 0 0 0 0 -10 "
								"-10 0 5 5 5 5 0 -10 "
								"-5 0 5 5 5 5 0 -5 "
								"0 0 5 5 5 5 0 -5 "
								"-10 5 5 5 5 5 0 -10 "
								"-10 0 5 0 0 0 0 -10 "
								"-20 -10 -10 -5 -5 -10 -10 -20 ";

	square_value_init[KING] =	"-30 -40 -40 -50 -50 -40 -40 -30 "
								"-30 -40 -40 -50 -50 -40 -40 -30 "
								"-30 -40 -40 -50 -50 -40 -40 -30 "
								"-30 -40 -40 -50 -50 -40 -40 -30 "
								"-20 -30 -30 -40 -40 -30 -30 -20 "
								"-10 -20 -20 -20 -20 -20 -20 -10 "
								"20 20 0 0 0 0 20 20 "
								"20 30 0 0 0 0 30 20 ";

	initValueTables();
}

Evaluate::~Evaluate() {
	// TODO Auto-generated destructor stub
}

void Evaluate::initValueTables()
{
	for(int piece = PAWN; piece <= KING; ++piece){
		std::stringstream table;
		table << square_value_init[piece];

		for(int line = 7; line >= 0; --line){
			for(int row = 0; row != 8; ++row){
				square sq = line*8+row;
				int value;
				table >> value;
				square_value[0][piece][sq] = value;
			}
			std::cout << std::endl;
		}

		table << square_value_init[piece];
		for(int line = 0; line != 8; ++line){
			for(int row = 0; row != 8; ++row){
				square sq = line*8+row;
				int value;
				table >> value;
				square_value[1][piece][sq] = value;
			}
		}

	}

}

// Evaluates material on a Board
// Value:
// 3P < N < B < R < Q
int Evaluate::material(const Board* board, const Color& color)
{
	int mat = 0;
	mat += popCount(board->getPawns(color)) * 100;
	mat += popCount(board->getKnights(color)) * 320;
	mat += popCount(board->getBishops(color)) * 330;
	mat += popCount(board->getRooks(color)) * 500;
	mat += popCount(board->getQueens(color)) * 900;
	return mat;
}

int Evaluate::piecePosition(bitboard& pieces, const Color& color,const Piece& type)
{
	int value = 0;
	if(color == WHITE){
		while(pieces){
			square pos = pop_lsb(pieces);
			value += square_value[0][type][pos];
		}
	}
	if(color == BLACK){
		while(pieces){
			square pos = pop_lsb(pieces);
			value += square_value[1][type][pos];
		}
	}
	return value;

}

int Evaluate::position(const Board* board, const Color& color)
{
	int pos = 0;
	bitboard piece_bb = board->getPawns(color);
	pos += piecePosition(piece_bb,color,PAWN);

	piece_bb = board->getKnights(color);
	pos += piecePosition(piece_bb,color,KNIGHT);

	piece_bb = board->getBishops(color);
	pos += piecePosition(piece_bb,color,BISHOP);

	piece_bb = board->getRooks(color);
	pos += piecePosition(piece_bb,color,ROOK);

	piece_bb = board->getQueens(color);
	pos += piecePosition(piece_bb,color,QUEEN);

	piece_bb = board->getKings(color);
	pos += piecePosition(piece_bb,color,KING);
	return pos;
}

int Evaluate::evaluatePos(const Board* board)
{
	int score = 0;
	score += material(board,board->us) - material(board, board->them);
	score += position(board,board->us);
	return score;
}

