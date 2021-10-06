#ifndef LOGCONSOLE_H
#define LOGCONSOLE_H
#include <QTextEdit>
#include <QtDebug>

class LogConsole {

public:
	static QTextEdit* globalconsole;
	static void logline(QString str);
};

#endif