#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>
#include <QString>

#include "expression.hpp"

class QGraphicsView;
class QGraphicsScene;

class OutputWidget: public QWidget{
Q_OBJECT

public:

	OutputWidget(QWidget *parent = nullptr);
	void outputExpression(QString input);
	void outputPoint(Expression& exp);
	void outputLine(const Expression& exp);
	void outputText(const Expression& exp);
	void clear();

private:

	QGraphicsView * qgv;
	QGraphicsScene * qgs;

};

#endif