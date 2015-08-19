/*
 * Uci.h
 *
 *  Created on: Apr 19, 2015
 *      Author: max
 */

#include <string>

#ifndef SRC_UCI_H_
#define SRC_UCI_H_

class Uci {
public:
	Uci();
	virtual ~Uci();


	std::string getCommand();
	void sendCommand();

	void parseCommand(const std::string& cmd);
	void initEngine();

};

#endif /* SRC_UCI_H_ */
