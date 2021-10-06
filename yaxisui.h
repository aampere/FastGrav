#ifndef YAXISUI_H
#define YAXISUI_H

#include <QDialog>
#include "ui_yaxisui.h"
#include "crosssection.h"

class YAxisUI : public QDialog {
	Q_OBJECT

	CrossSection* crosssection;
	double ymin;
	double ymax;
	double tick0;
	double tickinterval;
public:
	YAxisUI(CrossSection* cs);

	Ui::YAxisClass ui;

public Q_SLOTS:
	void applySlot();
	void automaticClicked();
	void manualClicked();

	void setYMinFromLineEdit();
	void setYMaxFromLineEdit();
	void setTick0FromLineEdit();
	void setTickIntervalFromLineEdit();
};
#endif