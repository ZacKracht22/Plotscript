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
	QGraphicsTextItem * qgti = new QGraphicsTextItem(input);
	qgs->addItem(qgti);
}

void OutputWidget::outputPoint(Expression& exp) {
	clear();

	qreal x = exp.getTail().at(0).getHead().asNumber();
	qreal y = exp.getTail().at(1).getHead().asNumber();
	qreal width = exp.getProperty("\"size\"").getHead().asNumber();
	qreal height = width;
	QGraphicsEllipseItem* point = new QGraphicsEllipseItem(x,y,width,height);
	QBrush brush(Qt::SolidPattern);
	point->setBrush(brush);
	qgs->addItem(point);

	qDebug() << "I am point";
	qDebug() << "X: " << x;
	qDebug() << "Y: " << y;
	qDebug() << "Width: " << width;
	qDebug() << "Height: " << height;
}

void OutputWidget::outputLine(const Expression& exp) {
	exp.isHeadList();
	qDebug() << "I am line";
}

void OutputWidget::outputText(const Expression& exp) {
	exp.isHeadList();
	qDebug() << "I am text";
}

void OutputWidget::clear() {
	qgs->clear();
}

