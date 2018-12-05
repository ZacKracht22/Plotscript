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

//Worker class for the producer/consumer structure of handling the programs threads
class Worker
{
public:

	//And instance takes in q1 as the input queue and q2 as the output queue
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

		//While the worker is active
		while (true) {
			std::string line;
			m_queue_in->wait_and_pop(line); //Wait for an input from the message queue and pop it as a string to parse
			if (line == "die") break; //Die is a keyword to kill the kernel and break the loop
			std::istringstream expression(line);

			std::pair<std::string, Expression> returnPair;

			if (!interp.parseStream(expression)) {
				returnPair.first = "Error: Invalid Expression. Could not parse.";
			}
			else {
				try {
					//If a successful parse with no semantic errors, make the output string blank and set the output expression to the evaluation
					returnPair.first = "";
					returnPair.second = interp.evaluate();
				}
				catch (const SemanticError & ex) {
					//If an error gets thrown, send it through the output string
					returnPair.first = ex.what();
				}
			}
			//Push the evaluation to the output message queue
			m_queue_out->push(returnPair);
		}
	}

private:
	ThreadSafeQueue<std::string> * m_queue_in;
	ThreadSafeQueue<std::pair<std::string, Expression>> * m_queue_out;
	Interpreter interp;
};



#endif