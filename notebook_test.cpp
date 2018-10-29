#include <QTest>
#include <QtTest/QtTest>
#include <QtWidgets>
#include <QDebug>

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

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier, 200);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");

	QString output = textItem->toPlainText();

	QVERIFY(output == "NONE");
}

void NotebookTest::cosPi() {


	QString testInput = "(cos pi)";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier, 200);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");

	QString output = textItem->toPlainText();

	QVERIFY(output == "(-1)");
}

void NotebookTest::complexInput() {


	QString testInput = "(+ I 1)";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier, 200);

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

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier, 200);

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

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier, 200);

	QGraphicsView* qgv = outputWidget->findChild<QGraphicsView*>();

	QList<QGraphicsItem*> items = qgv->items();

	QVERIFY(items.size() == 2);

}

void NotebookTest::eIPi() {


	QString testInput = "(^ e (- (* I pi)))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier, 200);

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

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier, 200);

	QGraphicsTextItem* textItem = outputWidget->getTextItem();
	QVERIFY2(textItem, "Could not find text item");

	QString output = textItem->toPlainText();

	QVERIFY(output == "(\"The Title\")");
}

void NotebookTest::lambdaNoOutput() {

	QString testInput = "(define inc (lambda (x) (+ x 1)))";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier, 200);

	QGraphicsView* qgv = outputWidget->findChild<QGraphicsView*>();

	QList<QGraphicsItem*> items = qgv->items();

	QVERIFY(items.size() == 0);

}

void NotebookTest::defaultPointSize() {

	QString testInput = "(make-point 0 0)";

	inputWidget->insertPlainText(testInput);

	QTest::keyClick(inputWidget, Qt::Key_Enter, Qt::ShiftModifier, 200);

	QGraphicsView* qgv = outputWidget->findChild<QGraphicsView*>();

	QList<QGraphicsItem*> items = qgv->items();

	QVERIFY(items.size() == 1);

}






QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
