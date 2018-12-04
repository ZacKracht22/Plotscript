#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include <QWidget>
#include <QLayout>
#include <fstream>
#include <thread>

#include "input_widget.hpp"
#include "output_widget.hpp"
#include "interpreter.hpp"
#include "expression.hpp"
#include "ThreadSafeQueue.hpp"
#include "worker.hpp"


class NotebookApp: public QWidget{
Q_OBJECT

public:
	NotebookApp();
	~NotebookApp();
	void recursiveListInterpret(std::vector<Expression>& list);

	ThreadSafeQueue<std::string> input_queue;
	ThreadSafeQueue<std::pair<std::string, Expression>> output_queue;
	std::thread main_thread;

public slots:
	void NewInterpret();

private:
	InputWidget* input;
	OutputWidget* output;
	
};

#endif