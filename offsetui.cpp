#include "offsetui.h"
#include "mainwindow.h"
#include "units.h"
#include "global.h"
#include <QMessageBox>
#include <QtDebug>

OffsetUI::OffsetUI(MainWindow* mw)
: QDialog(mw), mainwindow(mw)
{
	ui.setupUi(this);

	connect(ui.applybutton, SIGNAL(clicked()), this, SLOT(applySlot()));
	connect(ui.cancelbutton, SIGNAL(clicked()), this, SLOT(reject()));

	connect(ui.bestfit, SIGNAL(clicked()), this, SLOT(bestfitClickedSlot()));
	connect(ui.anchor, SIGNAL(clicked()), this, SLOT(anchorClickedSlot()));
	connect(ui.manual, SIGNAL(clicked()), this, SLOT(manualClickedSlot()));

	connect(ui.manualoffset, SIGNAL(editingFinished()), this, SLOT(setManualOffsetFromLineEdit()));

	connect(ui.anchorset, SIGNAL(currentIndexChanged(int)), this, SLOT(anchorsetChanged(int)));
}

void OffsetUI::applySlot() {
	if(ui.bestfit->isChecked()) {
		accept();
	} else if(ui.anchor->isChecked()) {
		if(ui.anchorset->currentIndex()>0 && ui.anchorpoint->currentIndex()>0) {
			accept();
		} else {
			QMessageBox::information(this, "Invalid Anchor", "Please select a valid observation point to serve as your anchor.");
		}
	} else if(ui.manual->isChecked()) {
		bool ok;
		ui.manualoffset->text().toDouble(&ok);
		if(ok) {
			accept();
		} else {
			QMessageBox::information(this, "Invalid Manual Offset", "You entered an invalid manual offset.");
		}
	}
}
void OffsetUI::setManualOffsetFromLineEdit() {
	QString str = ui.manualoffset->text();
	bool ok;
	double val = str.toDouble(&ok);
	if(ok) {
		manualoffsetvalue = val;
	} else {
		ui.manualoffset->setText(QString::number(manualoffsetvalue));//revert the LineEdit's contents
	}
}
void OffsetUI::setManualOffsetValue(double d) {
	manualoffsetvalue = d;
	ui.manualoffset->setText(QString::number(manualoffsetvalue));
}

void OffsetUI::bestfitClickedSlot() {
	ui.anchorset->setEnabled(false);
	ui.anchorpoint->setEnabled(false);
	ui.manualoffset->setEnabled(false);
}
void OffsetUI::anchorClickedSlot() {
	ui.anchorset->setEnabled(true);
	ui.anchorpoint->setEnabled(true);
	ui.manualoffset->setEnabled(false);
}
void OffsetUI::manualClickedSlot() {
	ui.anchorset->setEnabled(false);
	ui.anchorpoint->setEnabled(false);
	ui.manualoffset->setEnabled(true);
}

void OffsetUI::anchorsetChanged(int index) {
	if(index<=0) { //-1 is a possibility
		ui.anchorpoint->clear();
		//ui.anchorpoint->addItem("[No empirical points in this observation set]");
	} else {
		ui.anchorpoint->clear();
		ui.anchorpoint->addItem("Select empirical data point");
		int osindex = index-1;
		if(mainwindow) {
			auto observationsets = mainwindow->observationSets();
			auto emppts = observationsets[osindex]->getEmpGravityVals();
			auto pts = observationsets[osindex]->getPoints();
            for(uint j=0; j!=emppts.size(); ++j) {
				if(!isNoData(emppts[j])) {
					ui.anchorpoint->addItem(QString("%1  (%2, %3, %4)%6  %5%7").arg(j).arg(Units::siToDisplayDistance(pts[j].x)).arg(Units::siToDisplayDistance(pts[j].y)).arg(Units::siToDisplayDistance(pts[j].z)).arg(Units::siToDisplayGrav(emppts[j]))
						.arg(Units::distanceSuffix()).arg(Units::gravSuffix()));
				}
			}
			if(ui.anchorpoint->count()==1) {
				ui.anchorpoint->clear();
				ui.anchorpoint->addItem("[No empirical points in this observation set]");
			}
		}
	}
}
