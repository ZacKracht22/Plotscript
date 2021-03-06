#include "notebook_app.hpp"
#include "expression.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "ThreadSafeQueue.hpp"
#include "worker.hpp"

#include <QDebug>
#include <QString>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>

NotebookApp::NotebookApp() {
	//Instantiate the widgets used on the notebook app
	input = new InputWidget();
	output = new OutputWidget();
	QPushButton* start = new QPushButton("Start Kernel");
	start->setObjectName("start");
	QPushButton* stop = new QPushButton("Stop Kernel");
	stop->setObjectName("stop");
	QPushButton* reset = new QPushButton("Reset Kernel");
	reset->setObjectName("reset");
	QPushButton* interrupt = new QPushButton("Interrupt");
	interrupt->setObjectName("interrupt");

	//Create a worker and thread
	Worker main_worker(&input_queue, &output_queue);
	main_thread = std::thread(main_worker);

	QObject::connect(input, SIGNAL(shiftEnter()), this, SLOT(NewInterpret()));
	QObject::connect(start, SIGNAL(clicked()), this, SLOT(start_signal()));
	QObject::connect(stop, SIGNAL(clicked()), this, SLOT(stop_signal()));
	QObject::connect(reset, SIGNAL(clicked()), this, SLOT(reset_signal()));
	QObject::connect(interrupt, SIGNAL(clicked()), this, SLOT(interrupt_signal()));

	QHBoxLayout *b_layout = new QHBoxLayout;
	b_layout->addWidget(start);
	b_layout->addWidget(stop);
	b_layout->addWidget(reset);
	b_layout->addWidget(interrupt);

	buttons = new QWidget;
	buttons->setLayout(b_layout);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(buttons);
	layout->addWidget(input);
	layout->addWidget(output);

	setLayout(layout);
}

NotebookApp::~NotebookApp() {
	//Send the key word to end the worker's while(true) loop when the program is exited
	input_queue.push("die");
	main_thread.join();
}

//Interprets each item in a list and outputs them one at a time. 
void NotebookApp::recursiveListInterpret(std::vector<Expression>& list) {
	if (list.size() == 0) {
		return;
	}

	Expression exp = list[0];
	std::string evalExp = "";
	if (exp.getProperty("\"object-name\"") == Expression(Atom("\"point\""))) {
		output->outputPoint(exp, false);
	}
	else if (exp.getProperty("\"object-name\"") == Expression(Atom("\"line\""))) {
		output->outputLine(exp, false);
	}
	else if (exp.getProperty("\"object-name\"") == Expression(Atom("\"text\""))) {
		output->outputText(exp, false);
	}
	else {
		evalExp = expString(exp);
		output->outputExpression(QString::fromStdString(evalExp));
	}

	list.erase(list.begin());
	recursiveListInterpret(list);
}

//Slot that gets called when shift enter is pressed. It parses the input string from the input box if the thread is active,
//outputs an error if the thread is not active.
void NotebookApp::NewInterpret() {
		if (!main_thread.joinable()) {
			output->outputExpression(QString::fromStdString("Error: interpreter kernel not running"));
		}
		else {
			std::pair<std::string, Expression> ret;
			std::string inString = input->toPlainText().toStdString();

			input->setDisabled(true);
			input_queue.push(inString);
			output_queue.wait_and_pop(ret);
			input->setDisabled(false);

			if (ret.first.empty()) { //output expression
				Expression exp = ret.second;
				std::string evalExp = "";

				if (exp.getProperty("\"object-name\"") == Expression(Atom("\"point\""))) {
					output->outputPoint(exp, true);
				}
				else if (exp.getProperty("\"object-name\"") == Expression(Atom("\"line\""))) {
					output->outputLine(exp, true);
				}
				else if (exp.getProperty("\"object-name\"") == Expression(Atom("\"text\""))) {
					output->outputText(exp, true);
				}
				else if (exp.head().asSymbol() == "list") {
					output->clear();
					std::vector<Expression> list = exp.getTail();
					recursiveListInterpret(list);
				}
				else if (!exp.isHeadLambda()) {
					evalExp = expString(exp);
					output->outputExpression(QString::fromStdString(evalExp));
				}
				else {
					output->clear();
				}
			}
			else { //output error message
				std::string errorMessage = ret.first;
				output->outputExpression(QString::fromStdString(errorMessage));
			}

		}
		
}

//Slot gets called when the Start Kernel button is pressed. This starts a new thread if the current one is inactive.
void NotebookApp::start_signal() {
	if (!main_thread.joinable()) {
		Worker new_worker(&input_queue, &output_queue);
		main_thread = std::thread(new_worker);
	}
}

//Slot gets called when the Stop Kernel button is pressed. Kills the thread if it is active.
void NotebookApp::stop_signal() {
	if (main_thread.joinable()) {
		input_queue.push("die");
		main_thread.join();
	}
}

//Slot gets called when the Reset Kernel button is pressed. 
void NotebookApp::reset_signal() {

	if (main_thread.joinable()) {
		input_queue.push("die");
		main_thread.join();
		Worker new_worker(&input_queue, &output_queue);
		main_thread = std::thread(new_worker);
	}
	else{
		Worker new_worker(&input_queue, &output_queue);
		main_thread = std::thread(new_worker);
	}
	
}

void NotebookApp::interrupt_signal() {

	
}