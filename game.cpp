#include <assert.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <string.h>

#include "board.h"
#include "movegen.h"
#include "evaluate.h"
#include "search.h"

#include "types.h"

using namespace std;

uint64_t perft(Board& board, Movegen& movegen,int depth)
{
    Movelist list;
    uint64_t nodes = 0;

    if (depth == 0) return 1;

    movegen.genAllMoves(list);

    for (int i = 0; i < list.count; i++) {
    	if(board.makeMove(list.moves[i])){
    		nodes += perft(board,movegen,depth - 1);
    	}
        board.unmakeMove();
    }
    return nodes;
}


uint64_t timePerft(Board& board, Movegen& movegen, int depth)
{
	clock_t startTime = clock();
	uint64_t nodes = perft(board,movegen,depth);
	double time = double( clock() - startTime ) / (double)CLOCKS_PER_SEC;

	cout << fixed << "Nodes: " << nodes << " Time: " << time << " NPS: " << (uint64_t)nodes/time << endl;
	return nodes;
}

vector<string> divide(Board& board, Movegen& movegen,int depth)
{
    Movelist movelist;
    vector<string> ret;
    movegen.genAllMoves(movelist);
    int perft_cnt = 0;

    for (int i = 0; i < movelist.count; i++) {
    	string movestring;
    	board.makeMove(movelist.moves[i]);
    	if(!board.isInCheck(board.them)){
    		int perft_tmp = perft(board,movegen,depth - 1);
    		perft_cnt += perft_tmp;

    		movestring += board.moveNotation(movelist.moves[i]);
    		movestring += to_string(perft_tmp);
    		ret.push_back(movestring);
    		cout << movestring << endl;
    	}
        board.unmakeMove();
    }
    cout << "Move count: " << perft_cnt << endl;
    return ret;
}

vector<string> divideInput()
{
	vector<string> ret;
	string input;
	getline(cin, input);
	while(!input.empty()){
		ret.push_back(input);
		getline(cin, input);
	}
	return ret;
}

//TODO ADD MORE TESTS
void runTests()
{
	Board board;
	Movegen movegen = Movegen(&board);
	cout << "Running tests" << endl;


	cout << "Test 1: " << endl;
	board.setupFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // starting position
	board.printBoard();
	if(timePerft(board,movegen,6) != 119060324){
		cout << "Test 1 fail" << endl;
		return;
	}
	cout << "Test 1 success" << endl;

	cout << "Test 2: " << endl;
	board.setupFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
	board.printBoard();
	if(timePerft(board,movegen,5) != 193690690){
		cout << "Test 2 fail" << endl;
		return;
	}
	cout << "Test 2 success" << endl;

	cout << "Test 3: " << endl;
	board.setupFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
	board.printBoard();
	if(timePerft(board,movegen,6) != 11030083){
		cout << "Test 3 fail" << endl;
		return;
	}
	cout << "Test 3 success" << endl;

	cout << "Test 4: " << endl;
	board.setupFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
	board.printBoard();
	if(timePerft(board,movegen,5) != 15833292){
		cout << "Test 4 fail" << endl;
		return;
	}
	cout << "Test 4 success" << endl;

	cout << "Test 5: " << endl;
	board.setupFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
	board.printBoard();
	if(timePerft(board,movegen,3) != 62379){
		cout << "Test 5 fail" << endl;
		return;
	}
	cout << "Test 5 success" << endl;

	cout << "Test 6: " << endl;
	board.setupFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
	board.printBoard();
	if(timePerft(board,movegen,5) != 164075551){
		cout << "Test 6 fail" << endl;
		return;
	}
	cout << "Test 6 success" << endl;

	cout << "All Tests successfull" << endl;
}

void compareDivide(vector<string> div1,vector<string> div2)
{
	for(auto s1 : div1){
		bool match_found = 0;
		for(auto s2 : div2){
			if(s1 == s2){
				cout << s1 << " Match found" << endl;
				match_found = 1;
			}
		}
		if(!match_found){
			cout << s1 << " No Match found" << endl;
		}
	}
}

void divideDebug(Board& board, Movegen& movegen, const Color& color, int depth)
{
	vector<string> div_res = divide(board,movegen,depth);
	vector<string> div_in = divideInput();
	compareDivide(div_res, div_in);
}

int alphaBeta(Board& board, Movegen& movegen, Evaluate& eval,int depth, int ply, int alpha, int beta, Line* line_ptr) {
    Line line;
    Movelist list;

    assert(-INFINITE <= alpha && alpha < beta && beta <= INFINITE);

    if(board.inCheck && depth == 0){
    	depth++;
    }

    if (depth == 0) {
    	line_ptr->movecount = 0;
        return eval.evaluatePos(board);
    }

    movegen.genAllMoves(list);

    if(list.count == 0){
    	if(board.inCheck){
    		return -32000 + ply; //mate
    	}else {
    		return 0; //stalemate
    	}
    }

    for (int i = 0; i < list.count;++i)  {
    	// only consider legal moves
    	if(board.makeMove(list.moves[i])){
            int val = -alphaBeta(board,movegen,eval,depth - 1, ply + 1, -beta, -alpha, &line);
            board.unmakeMove();
            if (val >= beta) return beta;
            if (val > alpha) {
                alpha = val;
                line_ptr->moves[0] = list.moves[i];
                memcpy(line_ptr->moves + 1, line.moves, line.movecount * sizeof(Move));
                line_ptr->movecount = line.movecount + 1;
            }
    	}else{
    		board.unmakeMove();
    	}
    }
    return alpha;
}


Move searchMove(Board& board, Movegen& movegen, Evaluate& eval, int depth)
{
	Line line;
	alphaBeta(board,movegen,eval,depth,0,-INFINITE,INFINITE,&line);
	return line.moves[0];
}

void profile()
{
	Board board;
	Evaluate eval;
	Movelist list;
	Movegen movegen = Movegen(&board);

	board.setupFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // starting position
	board.printBoard();
	cout << searchMove(board,movegen,eval,6) << endl;

	board.setupFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
	board.printBoard();
	cout << searchMove(board,movegen,eval,6) << endl;

	board.setupFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
	board.printBoard();
	cout << searchMove(board,movegen,eval,6) << endl;

	board.setupFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
	board.printBoard();
	cout << searchMove(board,movegen,eval,6) << endl;

	board.setupFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
	board.printBoard();
	cout << searchMove(board,movegen,eval,5) << endl;

	board.setupFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
	board.printBoard();
	cout << searchMove(board,movegen,eval,6) << endl;
}

string getCommand()
{
	string input;
	getline(cin, input);
	return input;
}

void sendCommand(const string& cmd)
{
	cout << cmd << endl;
}

void sendMove(Board& board,const Move& move)
{
	string movestring = "bestmove ";
	movestring += board.moveNotation(move);
	sendCommand(movestring);
}

void parseCommand(Board& board, Movegen& movegen, Evaluate& eval, const string& cmd_string)
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
			board.setupFEN(fen);
			board.printBoard();
		}
		if(word == "go"){
			command >> word;
			int depth = 6;
			if(word == "depth"){
				command >> depth;
			}
			Move bestmove = searchMove(board,movegen,eval,depth);
			sendMove(board, bestmove);
		}
		if(word == "quit"){
			return;
		}
	}
}

void startUci(Board& board, Movegen& movegen, Evaluate& eval)
{
	while(1){
		string input;
		getline(cin, input);
		if(!input.empty()){
			parseCommand(board,movegen,eval,input);
		}
		if(input == "quit"){
			break;
		}
		usleep(100);
	}
}

int main()
{
	Board board;
	Evaluate eval;
	Movelist list;
	Movegen movegen = Movegen(&board);

	//profile();
	runTests();

	//startUci(board,movegen,eval);

	return 0;
}

