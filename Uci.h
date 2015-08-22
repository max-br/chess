/*
 * Uci.h
 *
 *  Created on: Apr 19, 2015
 *      Author: max
 */

#ifndef SRC_UCI_H_
#define SRC_UCI_H_

class Uci {
public:
	virtual ~Uci();

	Uci(Board* board, Search* search);

	Board* board;
	Search* search;

	std::string getCommand();
	void sendCommand(const std::string& cmd);
	void sendMove(const Move& move);

	void parseCommand(const std::string& cmd_string);
	void start();

};

#endif /* SRC_UCI_H_ */
