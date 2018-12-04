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
	int eval_from_stream(std::istream & stream);
	int eval_from_file(std::string filename);
	void error(const std::string & err_str);
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