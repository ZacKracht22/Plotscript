#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include <QWidget>
#include <QLayout>
#include <fstream>

#include "input_widget.hpp"
#include "output_widget.hpp"
#include "interpreter.hpp"
#include "expression.hpp"


class NotebookApp: public QWidget{
Q_OBJECT

public:
	NotebookApp();
	int eval_from_stream(std::istream & stream);
	int eval_from_file(std::string filename);
	void error(const std::string & err_str);
	void recursiveListInterpret(std::vector<Expression>& list);

public slots:
	void NewInterpret();


private:
	InputWidget* input;
	OutputWidget* output;
	Interpreter m_interp;

};

#endif