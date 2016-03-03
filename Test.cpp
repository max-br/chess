/*
 * Benchmark.cpp
 *
 *  Created on: Aug 22, 2015
 *      Author: max
 */

#include "Test.h"

#include <iostream>
#include <string>
#include <vector>

#include "Evaluate.h"
#include "Search.h"
#include "types.h"
#include "ToString.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;

Test::Test() {}

Test::~Test() {
  // TODO Auto-generated destructor stub
}

uint64_t Test::perft(Board &board, Movegen &movegen, int depth) {
  Movelist list;
  uint64_t nodes = 0;

  if (depth == 0)
    return 1;

  movegen.genAllMoves(list);

  for (int i = 0; i < list.count; i++) {
    if (board.makeMove(list.moves[i])) {
      nodes += perft(board, movegen, depth - 1);
    }
    board.unmakeMove();
  }
  return nodes;
}

uint64_t Test::timePerft(Board &board, Movegen &movegen, int depth) {
  clock_t startTime = clock();
  uint64_t nodes = perft(board, movegen, depth);
  double time = double(clock() - startTime) / (double)CLOCKS_PER_SEC;

  cout << std::fixed << "Nodes: " << nodes << " Time: " << time
       << " NPS: " << (uint64_t)nodes / time << endl;
  return nodes;
}

vector<string> Test::divide(Board &board, Movegen &movegen, int depth) {
  Movelist list;
  ToString tostring;

  vector<string> ret;
  movegen.genAllMoves(list);
  int perft_cnt = 0;

  for (int i = 0; i < list.count; i++) {
    string movestring;
    board.makeMove(list.moves[i]);
    if (!board.isInCheck(board.them)) {
      int perft_tmp = perft(board, movegen, depth - 1);
      perft_cnt += perft_tmp;

      movestring += tostring.moveNotation(list.moves[i]);
      // movestring += std::to_string(perft_tmp);
      ret.push_back(movestring);
      cout << movestring << endl;
    }
    board.unmakeMove();
  }
  cout << "Move count: " << perft_cnt << endl;
  return ret;
}

void Test::perftTests() {
  Board board;
  Movegen movegen = Movegen(&board);
  ToString tostring;

  cout << "Running tests" << endl;

  cout << "Test 1: " << endl;
  board.setupFEN(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // starting
                                                                   // position
  tostring.print(board);
  if (timePerft(board, movegen, 6) != 119060324) {
    cout << "Test 1 fail" << endl;
    return;
  }
  cout << "Test 1 success" << endl;

  cout << "Test 2: " << endl;
  board.setupFEN(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  tostring.print(board);
  if (timePerft(board, movegen, 5) != 193690690) {
    cout << "Test 2 fail" << endl;
    return;
  }
  cout << "Test 2 success" << endl;

  cout << "Test 3: " << endl;
  board.setupFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
  tostring.print(board);
  if (timePerft(board, movegen, 6) != 11030083) {
    cout << "Test 3 fail" << endl;
    return;
  }
  cout << "Test 3 success" << endl;

  cout << "Test 4: " << endl;
  board.setupFEN(
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
  tostring.print(board);
  if (timePerft(board, movegen, 5) != 15833292) {
    cout << "Test 4 fail" << endl;
    return;
  }
  cout << "Test 4 success" << endl;

  cout << "Test 5: " << endl;
  board.setupFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
  tostring.print(board);
  if (timePerft(board, movegen, 3) != 62379) {
    cout << "Test 5 fail" << endl;
    return;
  }
  cout << "Test 5 success" << endl;

  cout << "Test 6: " << endl;
  board.setupFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/"
                 "R4RK1 w - - 0 10");
  tostring.print(board);
  if (timePerft(board, movegen, 5) != 164075551) {
    cout << "Test 6 fail" << endl;
    return;
  }
  cout << "Test 6 success" << endl;

  cout << "All Tests successfull" << endl;
}

void Test::profile() {
  Board board;
  Evaluate eval;
  Movelist list;
  Movegen movegen = Movegen(&board);
  Search search(&board, &eval, &movegen);
  ToString tostring;

  board.setupFEN(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // starting
                                                                   // position
  tostring.print(board);
  cout << search.bestMove(6) << endl;

  board.setupFEN(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  tostring.print(board);
  cout << search.bestMove(6) << endl;

  board.setupFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
  tostring.print(board);
  cout << search.bestMove(6) << endl;

  board.setupFEN(
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
  tostring.print(board);
  cout << search.bestMove(6) << endl;

  board.setupFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
  tostring.print(board);
  cout << search.bestMove(6) << endl;

  board.setupFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/"
                 "R4RK1 w - - 0 10");
  tostring.print(board);
  cout << search.bestMove(6) << endl;
}
