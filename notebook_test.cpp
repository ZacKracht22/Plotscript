#include <QTest>
#include <QtTest/QtTest>
#include <QtWidgets>
#include <QDebug>
#include <iostream>

#include "notebook_app.hpp"
#include "input_widget.hpp"
#include "output_widget.hpp"

class NotebookTest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();
  void cleanup();
  void GetPropertyNoneCase();
  void cosPi();
  void complexInput();
  void listOfPoints();
  void listOfLines();
  void eIPi();
  void theTitle();
  void lambdaNoOutput();
  void defaultPointSize();
  void listPointTextAndLine();
  void errorTest();
  void listTest();
  void Milestone3Task1();
  void testDiscretePlotLayout();
  void testContinuousPlotLayout();

private:

	NotebookApp test_app;
	InputWidget* inputWidget;
	OutputWidget* outputWidget;
};

void NotebookTest::initTestCase(){
	test_app.show();

	inputWidget = test_app.findChild<InputWidget *>();
	QVERIFY2(inputWidget, "Could not find input widget");

	outputWidget = test_app.findChild<OutputWidget *>();
	QVERIFY2(outputWidget, "Could not find output widget");
}

void NotebookTest::cleanup() {
	inputWidget->clear();
}

void NotebookTest::GetPropertyNoneCase() {


	QString testInput = "(get-property \"key\" (3))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");

	QString output = textItem->toPlainText();

	QVERIFY(output == "NONE");
}

void NotebookTest::cosPi() {


	QString testInput = "(cos pi)";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");

	QString output = textItem->toPlainText();

	QVERIFY(output == "(-1)");
}

void NotebookTest::complexInput() {


	QString testInput = "(+ I 1)";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");

	QString output = textItem->toPlainText();

	QVERIFY(output == "(1,1)");
}

void NotebookTest::listOfPoints() {

	QString testInput = R"((list
(set-property "size" 5 (make-point 0 0))
(set-property "size" 5 (make-point 10 10))
(set-property "size" 5 (make-point 20 20))
))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsView* qgv = outputWidget->findChild<QGraphicsView*>();

	QList<QGraphicsItem*> items = qgv->items();

	QVERIFY(items.size() == 3);

}

void NotebookTest::listOfLines() {

	QString testInput = R"((list
(set-property "thickness" 1 (make-line (make-point 0 0) (make-point 1 1)))
(set-property "thickness" 2 (make-line (make-point 5 5) (make-point 6 6)))
))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsView* qgv = outputWidget->findChild<QGraphicsView*>();

	QList<QGraphicsItem*> items = qgv->items();

	QVERIFY(items.size() == 2);

}

void NotebookTest::eIPi() {


	QString testInput = "(^ e (- (* I pi)))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");

	QString output = textItem->toPlainText();

	QVERIFY(output == "(-1,-1.22465e-16)");
}

void NotebookTest::theTitle() {


	QString testInput = R"((begin
(define title "The Title")
(title)
))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");

	QString output = textItem->toPlainText();

	QVERIFY(output == "(\"The Title\")");
}

void NotebookTest::lambdaNoOutput() {

	QString testInput = "(define inc (lambda (x) (+ x 1)))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsView* qgv = outputWidget->findChild<QGraphicsView*>();

	QList<QGraphicsItem*> items = qgv->items();

	QVERIFY(items.size() == 0);

}

void NotebookTest::defaultPointSize() {

	QString testInput = "(make-point 0 0)";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsView* qgv = outputWidget->findChild<QGraphicsView*>();

	QList<QGraphicsItem*> items = qgv->items();

	QVERIFY(items.size() == 1);

}

void NotebookTest::listPointTextAndLine() {

	QString testInput = R"((list
(set-property "size" 5 (make-point 0 0))
(set-property "thickness" 5 (make-line (make-point (- 10) (- 10)) (make-point 0 0)))
(set-property "position" (make-point 10 10) (make-text "Hello World!"))
))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsView* qgv = outputWidget->findChild<QGraphicsView*>();

	QList<QGraphicsItem*> items = qgv->items();

	QVERIFY(items.size() == 3);

}

void NotebookTest::errorTest() {


	QString testInput = "(lambda (x))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");

	QString output = textItem->toPlainText();

	QVERIFY(output == "Error during evaluation: invalid number of arguments to define");
}

void NotebookTest::listTest() {

	QString testInput = R"((list
(1) (2) (3) (4) (5)
))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsView* qgv = outputWidget->findChild<QGraphicsView*>();

	QList<QGraphicsItem*> items = qgv->items();

	QVERIFY(items.size() == 1);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");

	QString output = textItem->toPlainText();

	QVERIFY(output == "(5)");

}

void NotebookTest::Milestone3Task1() {

	QString testInput = "(make-text \"hi\")";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");



}

/*
findLines - find lines in a scene contained within a bounding box
with a small margin
*/
int findLines(QGraphicsScene * scene, QRectF bbox, qreal margin) {

	QPainterPath selectPath;

	QMarginsF margins(margin, margin, margin, margin);
	selectPath.addRect(bbox.marginsAdded(margins));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numlines(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsLineItem::Type) {
			numlines += 1;
		}
	}

	return numlines;
}

/*
findPoints - find points in a scene contained within a specified rectangle
*/
int findPoints(QGraphicsScene * scene, QPointF center, qreal radius) {

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numpoints(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsEllipseItem::Type) {
			numpoints += 1;
		}
	}

	return numpoints;
}

/*
findText - find text in a scene centered at a specified point with a given
rotation and string contents
*/
int findText(QGraphicsScene * scene, QPointF center, qreal rotation, QString contents) {

	int numtext(0);
	foreach(auto item, scene->items(center)) {
		if (item->type() == QGraphicsTextItem::Type) {
			QGraphicsTextItem * text = static_cast<QGraphicsTextItem *>(item);
			if ((text->toPlainText() == contents) &&
				(text->rotation() == rotation) &&
				(text->pos() + text->boundingRect().center() == center)) {
				numtext += 1;
			}
		}
	}

	return numtext;
}

/*
intersectsLine - find lines in a scene that intersect a specified rectangle
*/
int intersectsLine(QGraphicsScene * scene, QPointF center, qreal radius) {

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
	scene->setSelectionArea(selectPath, Qt::IntersectsItemShape);

	int numlines(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsLineItem::Type) {
			numlines += 1;
		}
	}

	return numlines;
}

void NotebookTest::testDiscretePlotLayout() {

	std::string program = R"( 
(discrete-plot (list (list -1 -1) (list 1 1)) 
    (list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label") ))
)";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	QCOMPARE(items.size(), 17);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 2.0;
	double scaley = 20.0 / 2.0;

	double xmin = scalex*-1;
	double xmax = scalex * 1;
	double ymin = scaley*-1;
	double ymax = scaley * 1;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Title")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-1")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("1")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-1")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("1")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

	// check the abscissa axis
	QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

	// check the point at (-1,-1)
	QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);

	// check the point at (1,1)
	QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1);
}

void NotebookTest::testContinuousPlotLayout() {
	std::string program = R"(
	(begin 
	(define f (lambda (x) (+ x 0)))
	(continuous-plot f (list -1 1) 
	(list (list "title" "A continuous linear function") 
	(list "abscissa-label" "x") 
	(list "ordinate-label" "y") )))
	)";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	auto items = scene->items();
	QCOMPARE(items.size(), 63);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 2.0;
	double scaley = 20.0 / 2.0;

	double xmin = scalex*-1;
	double xmax = scalex * 1;
	double ymin = scaley*-1;
	double ymax = scaley * 1;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("A continuous linear function")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("x")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("y")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-1")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("1")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-1")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("1")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 1);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 1);

	// check the abscissa axis
	QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);


}








QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
