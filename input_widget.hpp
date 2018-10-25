#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <QPlainTextEdit>
#include <QString>

class InputWidget : public QPlainTextEdit {
	Q_OBJECT

	public:

		InputWidget(QWidget *parent = nullptr);

		void keyPressEvent(QKeyEvent *e);

	signals:

		void shiftEnter();


};

#endif