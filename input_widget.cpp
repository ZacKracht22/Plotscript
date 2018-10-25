#include "input_widget.hpp"

#include <QString>
#include <QObject>

InputWidget::InputWidget(QWidget *parent):QPlainTextEdit(parent){
	QString name = QString::fromStdString("input");
	setObjectName(name);
}

void InputWidget::keyPressEvent(QKeyEvent *e) {

	if ((e->modifiers() & Qt::ShiftModifier) && ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))) {
		emit shiftEnter();
	}
	QPlainTextEdit::keyPressEvent(e);
}