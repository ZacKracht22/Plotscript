#include "output_widget.hpp"

#include <QWidget>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QDebug>

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
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

void OutputWidget::clear() {
	qgs->clear();
}

