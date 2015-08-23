#include <iostream>

#include "Board.h"
#include "Evaluate.h"
#include "Movegen.h"
#include "Uci.h"
#include "Search.h"
#include "ToString.h"
#include "types.h"
#include "Test.h"

using namespace std;


// encodes a move
inline Move encodeMove(const square& from, const square& to,
		const Piece& piece, const Piece& capture = EMPTY, const Piece& promotion = EMPTY, const square& EP = 0, const Movetype& type = NONE)
{
	return encodeFrom(from) | encodeTo(to) | encodePiece(piece) | encodeCapture(capture) | encodePromotion(promotion) | encodeEP(EP) | encodeMovetype(type);
}


int main()
{
	Board board;
	Evaluate eval;
	Movegen movegen = Movegen(&board);
	Search search(&board, &eval, &movegen);
	Uci uci(&board, &search);

	ToString tostring;
	Test test;

	//test.profile();

	uci.start();

	return 0;
}

