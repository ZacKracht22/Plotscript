#include "output_widget.hpp"
#include "expression.hpp"
#include "environment.hpp"

#include <QWidget>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QDebug>
#include <QBrush>
#include <QPen>

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
	QString name = QString::fromStdString("output");
	setObjectName(name);

	qgs = new QGraphicsScene();
	qgv = new QGraphicsView(qgs);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(qgv);
	setLayout(layout);
}

void OutputWidget::outputExpression(QString input) {
	clear();
	qgti = new QGraphicsTextItem(input);
	qgs->addItem(qgti);
}

void OutputWidget::outputPoint(Expression& exp, bool clearFlag) {
	if(clearFlag) clear();

	qreal width = exp.getProperty("\"size\"").getHead().asNumber();
	qreal height = width;
	qreal x = exp.getTail().at(0).getHead().asNumber();
	x = x - (width / 2);
	qreal y = exp.getTail().at(1).getHead().asNumber();
	y = y - (width / 2);

	if (width < 0) {
		outputExpression(QString::fromStdString("Error: point size cannot be negative"));
	}
	else {
		QGraphicsEllipseItem* point = new QGraphicsEllipseItem(x, y, width, height);
		QBrush brush(Qt::SolidPattern);
		point->setBrush(brush);
		qgs->addItem(point);
	}
}

void OutputWidget::outputLine(Expression& exp, bool clearFlag) {
	if (clearFlag) clear();

	int thickness = exp.getProperty("\"thickness\"").getHead().asNumber();

	if (thickness < 0) {
		outputExpression(QString::fromStdString("Error: line thickness cannot be negative"));
	}
	else {
		qreal x1 = exp.getTail().at(0).getTail().at(0).getHead().asNumber();
		qreal x2 = exp.getTail().at(1).getTail().at(0).getHead().asNumber();
		qreal y1 = exp.getTail().at(0).getTail().at(1).getHead().asNumber();
		qreal y2 = exp.getTail().at(1).getTail().at(1).getHead().asNumber();

		QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);

		QPen pen(Qt::SolidLine);
		pen.setWidth(thickness);
		line->setPen(pen);
		qgs->addItem(line);
	}
}


void OutputWidget::outputText(Expression& exp, bool clearFlag) {
	if(clearFlag) clear();

	Atom shouldBeList = exp.getProperty("\"position\"").getHead();
	std::vector<Expression> point = exp.getProperty("\"position\"").getTail();

	if (shouldBeList != Atom("list") || point.size() != 2) {
		outputExpression(QString::fromStdString("Error: position must be a point"));
	}
	else {
		std::string temp = exp.getHead().asString();
		temp.erase(0, 1);
		temp.erase(temp.length() - 1, 1);
		QString text = QString::fromStdString(temp);
		qgti = new QGraphicsTextItem(text);
		
		int posX = point.at(0).getHead().asNumber();
		int posY = point.at(1).getHead().asNumber();
		qgti->setPos(posX,posY);

		qgs->addItem(qgti);
	}


}

void OutputWidget::clear() {
	qgs->clear();
}

QGraphicsTextItem* OutputWidget::getTextItem() { return qgti; }


