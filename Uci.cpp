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

#include "Board.h"
#include "Search.h"
#include "Stringer.h"
#include "Uci.h"

using namespace std;


Uci::Uci(Board* board, Search* search,Stringer* stringer) {
	this->board = board;
	this->search = search;
	this->stringer = stringer;
}

Uci::~Uci() {
	// TODO Auto-generated destructor stub
}

std::string Uci::getCommand()
{
	std::string input;
	getline(cin, input);
	return input;
}

void Uci::sendCommand(const string& cmd)
{
	cout << cmd << endl;
}

void Uci::sendMove(const Move& move)
{
	string movestring = "bestmove ";
	movestring += stringer->moveNotation(move);
	sendCommand(movestring);
}

void Uci::parseCommand(const string& cmd_string)
{

	stringstream command;
	command << cmd_string;
	string word;
	while(command >> word)
	{
		if(word == "uci"){
			sendCommand("Chess 0.1 by Max Breitenfeldt");
			sendCommand("id name Chess 0.1");
			sendCommand("id author Max Breitenfeldt");
			sendCommand("uciok");
		}
		if(word == "isready"){
			sendCommand("readyok");
		}
		if(word == "position"){
			string next_word;
			string fen;
			while (command >> next_word){
				if(next_word == "fen"){
					command.get();
					getline(command, fen);
				}
				if(next_word == "startpos"){
					fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
				}

			}
			board->setupFEN(fen);
			stringer->printBoard();
		}
		if(word == "go"){
			command >> word;
			int depth = 6;
			if(word == "depth"){
				command >> depth;
			}
			Move bestmove = search->bestMove(depth);
			sendMove(bestmove);
		}
		if(word == "quit"){
			return;
		}
	}
}

void Uci::start()
{
	while(1){
		string input;
		getline(cin, input);
		if(!input.empty()){
			parseCommand(input);
		}
		if(input == "quit"){
			break;
		}
		usleep(100);
	}
}
