#include "logconsole.h"
#include <QTextEdit>

QTextEdit* LogConsole::globalconsole = nullptr;

void LogConsole::logline(QString str) {
	globalconsole->append(QString(str));
	//qDebug() << str;
}