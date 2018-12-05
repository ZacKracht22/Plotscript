#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <fstream>
#include <thread>

#include "input_widget.hpp"
#include "output_widget.hpp"
#include "interpreter.hpp"
#include "expression.hpp"
#include "ThreadSafeQueue.hpp"
#include "worker.hpp"

//NotebookApp is the top level widget used in the gui for the plotscript project.
//It has 4 horizontal push buttons along the top, underneath of them an inputwidget to input text,
//and under that an outputwidget to show the result of a plotscript evaluation (or kernel error).
//There are 4 slots in the widget, one for when shift-enter is pressed by the inputwidget to evaluate an expression
//and one for each push button.
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
	void start_signal();
	void stop_signal();
	void reset_signal();
	void interrupt_signal();

private:
	InputWidget* input;
	OutputWidget* output;
	QWidget* buttons;
};

#endif