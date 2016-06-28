/*
 * Uci.cpp
 *
 *  Created on: Apr 19, 2015
 *      Author: max
 */

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "Uci.h"

using namespace std;

Uci::Uci(Board *board, Search *search) {
  this->board = board;
  this->search = search;
}

Uci::~Uci() {
  // TODO Auto-generated destructor stub
}

std::string Uci::getCommand() {
  std::string input;
  getline(cin, input);
  return input;
}

void Uci::sendCommand(const string &cmd) { cout << cmd << endl; }

void Uci::sendMove(const Move &move) {
  string movestring = "bestmove ";
  movestring += tostring.moveNotation(move);
  sendCommand(movestring);
}


void Uci::parseCommand(const string &cmd_string) {
  stringstream command;
  command << cmd_string;
  string token, fen;
  vector<string> moves;
  while (command >> token) {
    if (token == "uci") {
      sendCommand("Chess 0.1 by Max Breitenfeldt");
      sendCommand("id name Chess 0.1");
      sendCommand("id author Max Breitenfeldt");
      sendCommand("uciok");
    }

    if (token == "go") {
        command >> token;
        int depth = 7;
        if (token == "depth") {
            command >> depth;
        }
        Move bestmove = search->bestMove(depth);
        sendMove(bestmove);
    }

    if (token == "isready") {
      sendCommand("readyok");
    }

    if (token == "position") {
      command >> token;
      string fen;
      if (token == "startpos") {
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        command >> token;
      } else {
        do {
          command >> token;
          fen += token + " ";
        } while (token != "moves");
      }
      if (token == "moves") {
        while (command >> token) {
          moves.push_back(token);
        }
      }
      board->setupFEN(fen);
      board->setupMoves(moves);
    }
  }
  return;
}

void Uci::start() {
  while (1) {
    string input;
    getline(cin, input);
    if (!input.empty()) {
      parseCommand(input);
    }
    if (input == "quit") {
      break;
    }
  }
}
