#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <QPlainTextEdit>
#include <QString>

//Input Widget is a widget used in the notebook_app gui that inherits 
//from QPlainTextEdit. It is a text box that emits a signal to be evaluated when shift-enter is pressed on the keyboard.
class InputWidget : public QPlainTextEdit {
	Q_OBJECT

	public:

		InputWidget(QWidget *parent = nullptr);

		void keyPressEvent(QKeyEvent *e);

	signals:

		void shiftEnter();


};

#endif