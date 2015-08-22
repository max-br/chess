/*
 * Uci.h
 *
 *  Created on: Apr 19, 2015
 *      Author: max
 */

#ifndef SRC_UCI_H_
#define SRC_UCI_H_

class Uci
{
public:
	Uci(Board* board, Search* search, Stringer* stringer);
	virtual ~Uci();

	Board* board;
	Search* search;
	Stringer* stringer;

	std::string getCommand();
	void sendCommand(const std::string& cmd);
	void sendMove(const Move& move);

	void parseCommand(const std::string& cmd_string);
	void start();

};

#endif /* SRC_UCI_H_ */
