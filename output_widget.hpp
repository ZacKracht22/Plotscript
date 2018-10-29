#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>
#include <QString>

#include "expression.hpp"

class QGraphicsView;
class QGraphicsScene;
class QGraphicsTextItem;

class OutputWidget: public QWidget{
Q_OBJECT

public:

	OutputWidget(QWidget *parent = nullptr);
	void outputExpression(QString input);
	void outputPoint(Expression& exp, bool clearFlag);
	void outputLine(Expression& exp, bool clearFlag);
	void outputText(Expression& exp, bool clearFlag);
	void clear();
	QGraphicsTextItem* getTextItem();

	friend class NotebookTest;

private:

	QGraphicsView * qgv;
	QGraphicsScene * qgs;
	QGraphicsTextItem * qgti;

};

#endif