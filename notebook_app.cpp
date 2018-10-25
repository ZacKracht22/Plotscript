#include "notebook_app.hpp"
#include "expression.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"

#include <QDebug>
#include <QString>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

NotebookApp::NotebookApp() {
	input = new InputWidget();
	output = new OutputWidget();

	QObject::connect(input, SIGNAL(shiftEnter()), this, SLOT(NewInterpret()));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(input);
	layout->addWidget(output);
	setLayout(layout);

	eval_from_file(STARTUP_FILE);
}

void NotebookApp::NewInterpret() {

		std::string inString = input->toPlainText().toStdString();
		std::istringstream expression(inString);

		if (!m_interp.parseStream(expression)) {
			error("Invalid Expression. Could not parse.");
		}
		else {
			try {
				Expression exp = m_interp.evaluate();

				std::string evalExp = "";
				if (exp.getProperty("\"object-name\"") == Expression(Atom("\"point\""))) {
					output->outputPoint(exp);
				}
				else if (exp.getProperty("\"object-name\"") == Expression(Atom("\"line\""))) {
					output->outputLine(exp);
				}
				else if (exp.getProperty("\"object-name\"") == Expression(Atom("\"text\""))) {
					output->outputText(exp);
				}
				else if (!exp.isHeadLambda()) {
					evalExp = expString(exp);
					output->outputExpression(QString::fromStdString(evalExp));
				}
				else {
					output->outputExpression(QString::fromStdString(evalExp));
				}
				

			}
			catch (const SemanticError & ex) {
				output->outputExpression(QString::fromStdString(ex.what()));
			}
		}
}

int NotebookApp::eval_from_file(std::string filename) {

	std::ifstream ifs(filename);

	if (!ifs) {
		error("Could not open file for reading.");
		return EXIT_FAILURE;
	}

	return eval_from_stream(ifs);
}

int NotebookApp::eval_from_stream(std::istream & stream) {

	if (!m_interp.parseStream(stream)) {
		error("Invalid Program. Could not parse.");
		return EXIT_FAILURE;
	}
	else {
		try {
			Expression exp = m_interp.evaluate();
		}
		catch (const SemanticError & ex) {
			output->outputExpression(QString::fromStdString(ex.what()));
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

void NotebookApp::error(const std::string & err_str) {
	std::string error = "Error: " + err_str;
	output->outputExpression(QString::fromStdString(error));

}