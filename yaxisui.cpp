#include "yaxisui.h"
#include "units.h"

YAxisUI::YAxisUI(CrossSection* cs)
:QDialog(cs), crosssection(cs)
{
	ui.setupUi(this);
	connect(ui.applybutton, SIGNAL(clicked()), this, SLOT(applySlot()));
	connect(ui.cancelbutton,SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui.automatic, SIGNAL(clicked()), this, SLOT(automaticClicked()));
	connect(ui.manual, SIGNAL(clicked()), this, SLOT(manualClicked()));
	connect(ui.ymin, SIGNAL(editingFinished()), this, SLOT(setYMinFromLineEdit()));
	connect(ui.ymax, SIGNAL(editingFinished()), this, SLOT(setYMaxFromLineEdit()));
	connect(ui.tick0, SIGNAL(editingFinished()), this, SLOT(setTick0FromLineEdit()));
	connect(ui.tickinterval, SIGNAL(editingFinished()), this, SLOT(setTickIntervalFromLineEdit()));

	if(crosssection->isYAxisAutomatic()) {
		ui.automatic->setChecked(true);
		automaticClicked();
	} else {
		ui.manual->setChecked(true);
		manualClicked();
	}
	ui.ymin->setText(QString::number(Units::siToDisplayGrav(crosssection->getYLow())));
	ui.ymax->setText(QString::number(Units::siToDisplayGrav(crosssection->getYHigh())));
	ui.tick0->setText(QString::number(Units::siToDisplayGrav(crosssection->getYTick0())));
	ui.tickinterval->setText(QString::number(Units::siToDisplayGrav(crosssection->getYTickInterval())));
	ui.rangeu->setText(Units::gravSuffix());
	ui.tick0u->setText(Units::gravSuffix());
	ui.tickintervalu->setText(Units::gravSuffix());
}

void YAxisUI::applySlot() {
	//need to validate the double strings!!
	accept();
}
void YAxisUI::automaticClicked() {
	ui.ymin->setEnabled(false);
	ui.to->setEnabled(false);
	ui.ymax->setEnabled(false);
	ui.rangeu->setEnabled(false);
	ui.havetickat->setEnabled(false);
	ui.tick0->setEnabled(false);
	ui.tick0u->setEnabled(false);
	ui.andatintervalsof->setEnabled(false);
	ui.tickinterval->setEnabled(false);
	ui.tickintervalu->setEnabled(false);
}
void YAxisUI::manualClicked() {
	ui.ymin->setEnabled(true);
	ui.to->setEnabled(true);
	ui.ymax->setEnabled(true);
	ui.rangeu->setEnabled(true);
	ui.havetickat->setEnabled(true);
	ui.tick0->setEnabled(true);
	ui.tick0u->setEnabled(true);
	ui.andatintervalsof->setEnabled(true);
	ui.tickinterval->setEnabled(true);
	ui.tickintervalu->setEnabled(true);
} 

void YAxisUI::setYMinFromLineEdit() {
	QString str = ui.ymin->text();
	bool ok;
	double val = str.toDouble(&ok);
	if(ok) {
		ymin = val;
	} else {
		ui.ymin->setText(QString::number(ymin));//revert the LineEdit's contents
	}
}
void YAxisUI::setYMaxFromLineEdit() {
	QString str = ui.ymax->text();
	bool ok;
	double val = str.toDouble(&ok);
	if(ok) {
		ymax = val;
	} else {
		ui.ymax->setText(QString::number(ymax));//revert the LineEdit's contents
	}
}
void YAxisUI::setTick0FromLineEdit() {
	QString str = ui.tick0->text();
	bool ok;
	double val = str.toDouble(&ok);
	if(ok) {
		tick0 = val;
	} else {
		ui.tick0->setText(QString::number(tick0));//revert the LineEdit's contents
	}
}
void YAxisUI::setTickIntervalFromLineEdit() {
	QString str = ui.tick0->text();
	bool ok;
	double val = str.toDouble(&ok);
	if(ok) {
		tick0 = val;
	} else {
		ui.tick0->setText(QString::number(tick0));//revert the LineEdit's contents
	}
}