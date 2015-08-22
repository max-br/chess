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

	//profile();

	Test test;
	test.runTests();

	//uci.start();

	return 0;
}

