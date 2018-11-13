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
		point->setScale(1);
		point->setPen(Qt::NoPen);
		qgs->addItem(point);

		qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->fitInView(qgs->itemsBoundingRect(), Qt::KeepAspectRatio);
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
		line->setScale(1);
		qgs->addItem(line);

		qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		qgv->fitInView(qgs->itemsBoundingRect(), Qt::KeepAspectRatio);

		/*qDebug() << "Adding line";
		qDebug() << "Line has first point of " << x1 << "," << y1;
		qDebug() << "Line has second point of " << x2 << "," << y2;*/
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

		double scaleFactor = exp.getProperty("\"text-scale\"").getHead().asNumber();
		double textRotation = exp.getProperty("\"text-rotation\"").getHead().asNumber();

		auto font = QFont("Monospace");
		font.setStyleHint(QFont::TypeWriter);
		font.setPointSize(1);
		qgti->setFont(font);

		qgti->setScale(scaleFactor);

		int centerX = point.at(0).getHead().asNumber();
		int centerY = point.at(1).getHead().asNumber();
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





