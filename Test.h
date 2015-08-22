/*
 * Benchmark.h
 *
 *  Created on: Aug 22, 2015
 *      Author: max
 */

#ifndef SRC_TEST_H_
#define SRC_TEST_H_

#include <string>
#include <vector>

#include "Board.h"
#include "Movegen.h"

class Test {
public:
	Test();
	virtual ~Test();


	uint64_t perft(Board& board,Movegen& movegen, int depth);
	uint64_t timePerft(Board& board,Movegen& movegen, int depth);

	//std::vector<std::string> divide(Board& board,Movegen& movegen,int depth);

	void profile();
	void runTests();
};

#endif /* SRC_TEST_H_ */
