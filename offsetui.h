#ifndef OFFSETUI_H
#define OFFSETUI_H
#include <QDialog>
#include "ui_offsetui.h"
class MainWindow;

class OffsetUI : public QDialog {
	Q_OBJECT

	double manualoffsetvalue;
	MainWindow* mainwindow;
public:
	OffsetUI(MainWindow* mw);

	Ui::OffsetUIClass ui;
public Q_SLOTS:
	void applySlot();
	void setManualOffsetFromLineEdit();
	void setManualOffsetValue(double d);

	void bestfitClickedSlot();
	void anchorClickedSlot();
	void manualClickedSlot();

	void anchorsetChanged(int index);
};

#endif 