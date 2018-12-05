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
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QtMath>
#include <QRectF>
#include <QDebug>
#include <iostream>

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

//When a point is needing to be drawn on the scene this function gets called.
//param clearFlag indicates whether the scene needs to be cleared first.
void OutputWidget::outputPoint(Expression& exp, bool clearFlag) {
	if(clearFlag) clear();

	//Get the parameters from the make-point expression
	qreal width = exp.getProperty("\"size\"").head().asNumber();
	qreal height = width;
	qreal x = exp.getTail().at(0).head().asNumber();
	x = x - (width / 2);
	qreal y = exp.getTail().at(1).head().asNumber();
	y = y - (width / 2);

	//Error if the point's width is negative
	if (width < 0) {
		outputExpression(QString::fromStdString("Error: point size cannot be negative"));
	}
	else {
		QGraphicsEllipseItem* point = new QGraphicsEllipseItem(x, y, width, height);
		QBrush brush(Qt::SolidPattern);
		point->setBrush(brush);
		point->setScale(1);
		point->setPen(Qt::NoPen);
		qgs->addItem(point);

		qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->fitInView(qgs->itemsBoundingRect(), Qt::KeepAspectRatio);
	}
}

//When a line is needing to be drawn on the scene this function gets called.
//param clearFlag indicates whether the scene needs to be cleared first.
void OutputWidget::outputLine(Expression& exp, bool clearFlag) {
	if (clearFlag) clear();

	int thickness = exp.getProperty("\"thickness\"").head().asNumber();

	if (thickness < 0) {
		outputExpression(QString::fromStdString("Error: line thickness cannot be negative"));
	}
	else {
		qreal x1 = exp.getTail().at(0).getTail().at(0).head().asNumber();
		qreal x2 = exp.getTail().at(1).getTail().at(0).head().asNumber();
		qreal y1 = exp.getTail().at(0).getTail().at(1).head().asNumber();
		qreal y2 = exp.getTail().at(1).getTail().at(1).head().asNumber();

		QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);

		QPen pen(Qt::SolidLine);
		pen.setWidth(thickness);
		line->setPen(pen);
		line->setScale(1);
		qgs->addItem(line);

		qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->fitInView(qgs->itemsBoundingRect(), Qt::KeepAspectRatio);
	}

}

//When any text is needing to be drawn on the scene this function gets called.
//param clearFlag indicates whether the scene needs to be cleared first.
void OutputWidget::outputText(Expression& exp, bool clearFlag) {
	if(clearFlag) clear();

	Atom shouldBeList = exp.getProperty("\"position\"").head();
	std::vector<Expression> point = exp.getProperty("\"position\"").getTail();

	if (shouldBeList != Atom("list") || point.size() != 2) {
		outputExpression(QString::fromStdString("Error: position must be a point"));
	}
	else {
		std::string temp = exp.head().asString();
		temp.erase(0, 1);
		temp.erase(temp.length() - 1, 1);
		QString text = QString::fromStdString(temp);
		qgti = new QGraphicsTextItem(text);

		double scaleFactor = exp.getProperty("\"text-scale\"").head().asNumber();
		double textRotation = exp.getProperty("\"text-rotation\"").head().asNumber();

		auto font = QFont("Monospace");
		font.setStyleHint(QFont::TypeWriter);
		font.setPointSize(1);
		qgti->setFont(font);
		qgti->setScale(scaleFactor);

		double centerX = point.at(0).head().asNumber();
		double centerY = point.at(1).head().asNumber();
		qreal defaultWidth = qgti->boundingRect().width();
		qreal defaultHeight = qgti->boundingRect().height();

		qgti->setPos(QPointF(centerX - (defaultWidth / 2), centerY - (defaultHeight / 2)));

		if (-qRadiansToDegrees(textRotation) == -90) {
			qgti->setTransformOriginPoint(defaultWidth /2, defaultHeight /2);
			qgti->setRotation(-qRadiansToDegrees(textRotation));
		}
		else {
			qgti->setRotation(-qRadiansToDegrees(textRotation));
		}

		qgs->addItem(qgti);

		qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->fitInView(qgs->itemsBoundingRect(), Qt::KeepAspectRatio);

	}


}

void OutputWidget::clear() {
	qgs->clear();
}

QGraphicsTextItem* OutputWidget::getTextItem() { 
	return qgti; 
}





