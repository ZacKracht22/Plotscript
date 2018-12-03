#ifndef WORKER_HPP
#define WORKER_HPP

#include "ThreadSafeQueue.hpp"
#include "expression.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>


class Worker
{
public:

	Worker(ThreadSafeQueue<std::string> *q1, ThreadSafeQueue<std::pair<std::string,Expression>> *q2)
	{
		m_queue_in = q1;
		m_queue_out = q2;
	}


	void operator()() const
	{
		Interpreter interp;

		std::ifstream ifs(STARTUP_FILE);
		interp.parseStream(ifs);
		Expression temp = interp.evaluate();

		while (true) { //edit this
			std::string line;
			m_queue_in->wait_and_pop(line);

			std::istringstream expression(line);

			std::pair<std::string, Expression> returnPair;

			if (!interp.parseStream(expression)) {
				returnPair.first = "Error: Invalid Expression. Could not parse.";
				//std::cerr << "Error: Invalid Expression. Could not parse." << std::endl;
			}
			else {
				try {
					returnPair.first = "";
					returnPair.second = interp.evaluate();
					//Expression exp = interp.evaluate();
					//std::cout << exp << std::endl;
				}
				catch (const SemanticError & ex) {
					returnPair.first = ex.what();
					//std::cerr << ex.what() << std::endl;
				}
			}
			m_queue_out->push(returnPair);
		}
	}

private:
	ThreadSafeQueue<std::string> * m_queue_in;
	ThreadSafeQueue<std::pair<std::string, Expression>> * m_queue_out;
	Interpreter interp;
};



#endif