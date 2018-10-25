#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>
#include <QString>

class QGraphicsView;
class QGraphicsScene;

class OutputWidget: public QWidget{
Q_OBJECT

public:

	OutputWidget(QWidget *parent = nullptr);
	void outputExpression(QString input);
	void clear();

private:

	QGraphicsView * qgv;
	QGraphicsScene * qgs;

};

#endif