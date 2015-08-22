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

int main()
{
	Board board;
	Evaluate eval;
	Movegen movegen = Movegen(&board);
	Search search(&board, &eval, &movegen);
	Uci uci(&board, &search);

	Test test;
	test.perftTests();

	//uci.start();

	return 0;
}

