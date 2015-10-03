/*
 * ToString.cpp
 *
 *  Created on: Aug 22, 2015
 *      Author: max
 */

#include "ToString.h"

#include <bitset>
#include <iostream>
#include <string>
#include <string.h>

#include "Board.h"
#include "types.h"

ToString::ToString() {
}

ToString::~ToString() {
	// TODO Auto-generated destructor stub
}


// Iterates through the bits of a bitboard and puts in the outputstring if there is a match
void ToString::parseBB(bitboard bb,const char figure, char* output){
	std::bitset<64> bset(bb);
	for(size_t i = 0; i < bset.size(); ++i){
		if(bset[i]){
			output[i] = figure;
		}
	}
}




// parses bitboards to string and prints
void ToString::print(Board& board){
	char boardstring[64];
	memset(boardstring, '.',64);

	parseBB(board.getPawns(WHITE), 'P', boardstring);
	parseBB(board.getKnights(WHITE), 'N', boardstring);
	parseBB(board.getBishops(WHITE), 'B', boardstring);
	parseBB(board.getRooks(WHITE), 'R', boardstring);
	parseBB(board.getQueens(WHITE), 'Q',boardstring);
	parseBB(board.getKings(WHITE), 'K', boardstring);

	parseBB(board.getPawns(BLACK), 'p', boardstring);
	parseBB(board.getKnights(BLACK), 'n', boardstring);
	parseBB(board.getBishops(BLACK), 'b', boardstring);
	parseBB(board.getRooks(BLACK), 'r', boardstring);
	parseBB(board.getQueens(BLACK), 'q',boardstring);
	parseBB(board.getKings(BLACK), 'k', boardstring);

	// endl every 8 chars, prints in reverse to correct orientation
	for(int i = 7; i > -1; --i){
		for(int j = 0; j != 8; ++j){
			std::cout << boardstring[i*8+j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

// parses bitboards to string and prints
void ToString::print(Board* board){
	char boardstring[64];
	memset(boardstring, '.',64);

	parseBB(board->getPawns(WHITE), 'P', boardstring);
	parseBB(board->getKnights(WHITE), 'N', boardstring);
	parseBB(board->getBishops(WHITE), 'B', boardstring);
	parseBB(board->getRooks(WHITE), 'R', boardstring);
	parseBB(board->getQueens(WHITE), 'Q',boardstring);
	parseBB(board->getKings(WHITE), 'K', boardstring);

	parseBB(board->getPawns(BLACK), 'p', boardstring);
	parseBB(board->getKnights(BLACK), 'n', boardstring);
	parseBB(board->getBishops(BLACK), 'b', boardstring);
	parseBB(board->getRooks(BLACK), 'r', boardstring);
	parseBB(board->getQueens(BLACK), 'q',boardstring);
	parseBB(board->getKings(BLACK), 'k', boardstring);

	// endl every 8 chars, prints in reverse to correct orientation
	for(int i = 7; i > -1; --i){
		for(int j = 0; j != 8; ++j){
			std::cout << boardstring[i*8+j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}


void ToString::printDebug(Board& board)
{
	for(int i = 1; i != PIECE_BB_SIZE; ++i){
		std::cout << "Piecetype: " << i << ": " << board.piece_BB[i] << std::endl;
		printBB(board.piece_BB[i]);
	}
	std::cout << std::endl;

	for(int line = 7; line >= 0; --line){
		for(int row = 0; row != 8; ++row){
			square sq = line*8+row;
			std::cout << board.squares[sq];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << "En passant: " << board.en_passant[0] << " " << board.en_passant[1] << std::endl;
	std::cout << "color us: " << board.us << " them: " << board.them << std::endl;

	print(board);
}

// Prints a bitboard as a chessboard
void ToString::printBB(bitboard bb)
{
	bitboard tmp = bb;
	for(int i = 7; i > -1; --i){
		for(int j = 0; j != 8; ++j){
			if((tmp >> (i*8+j)) & 1){
				std::cout << "1";
			}else{
				std::cout << "0";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

std::string ToString::squareNotation(square sq)
{
	char letter = (sq % 8) + 97;
	char number = (int)sq / 8 + 49;
	std::string coord;
	coord += letter;
	coord += number;
	return coord;
}

std::string ToString::moveNotation(Move m)
{
	std::string ret;
	if(m == NULLMOVE){
		return "0000";
	}

	ret += squareNotation(extractFrom(m));
	ret += squareNotation(extractTo(m));

	if(extractPromotion(m)){
		if(extractPromotion(m) == BISHOP){
			ret += 'b';
		}
		if(extractPromotion(m) == KNIGHT){
			ret += 'n';
		}
		if(extractPromotion(m) == ROOK){
			ret += 'r';
		}
		if(extractPromotion(m) == QUEEN){
			ret += 'q';
		}
	}

	ret += " ";

	return ret;
}

void ToString::printMove(Move m)
{
	std::cout << "From: " << extractFrom(m) << " To: " << extractTo(m) << " Piece: "
			<< extractPiece(m) << " Capture: " << extractCapture(m) << " Promotion: "
			<< extractPromotion(m) << " EP: " << extractEP(m) << "Castle: "
			<< extractMovetype(m) << " Move: " << m << std::endl;
}
