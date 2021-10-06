#include "editcylinderitemui.h"
#include "cylinderitem.h"
#include "mainwindow.h"
#include "units.h"
#include <QColorDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QtDebug>

EditCylinderItemUI::EditCylinderItemUI(QWidget* parent, CylinderItem* c, MainWindow* mw) : QDialog(parent), ci(c), mainwindow(mw) {
	ui.setupUi(this);

	connect(ui.applybutton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.cancelbutton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(this, SIGNAL(accepted()), this, SLOT(applySlot()));
	if(ci) {
        ui.densityu->setText(Units::densitySuffix()); ui.xu->setText(Units::distanceSuffix()); ui.yu->setText(Units::distanceSuffix()); ui.zu->setText(Units::distanceSuffix()); ui.radiusu->setText(Units::distanceSuffix());
		ui.name->setText(ci->getName());
		ui.active->setChecked(ci->isActive());
		ui.visible->setChecked(ci->isVisible());
			QPixmap coloricon(16,16); coloricon.fill(ci->getColor());
			color = ci->getColor();
			ui.color->setIcon(QIcon(coloricon));
			connect(ui.color, SIGNAL(clicked()), this, SLOT(selectColorSlot()));
		connect(ui.snapazimuth, SIGNAL(clicked()), this, SLOT(snapAzimuthSlot()));
		ui.density->setValue(Units::siToDisplayDensity(ci->getDensity()));
		ui.x->setValue(Units::siToDisplayDistance(ci->getCenter().x));
		ui.y->setValue(Units::siToDisplayDistance(ci->getCenter().y));
		ui.z->setValue(Units::siToDisplayDistance(ci->getCenter().z));
		ui.azimuth->setValue(ci->getAzimuth());
		ui.radius->setValue(ci->getRadius());
	}
}
void EditCylinderItemUI::selectColorSlot() {
	QColor newcol = QColorDialog::getColor(ci->getColor());
	if(newcol.isValid()){
		QPixmap coloricon(16,16); coloricon.fill(newcol);
		ui.color->setIcon(QIcon(coloricon));
		color = newcol;
	}
}
void EditCylinderItemUI::snapAzimuthSlot() {
	//code duplication with EditPolygonPrismItemUI
	QVBoxLayout* layout = new QVBoxLayout();
	QLabel* label = new QLabel("Snap azimuth to an existing Observation Set:");
	QComboBox* observationsetcombo = new QComboBox();
	std::vector<ObservationSet*> osets = mainwindow->observationSets();
	for(auto obs=osets.begin(); obs!=osets.end(); ++obs) {
		observationsetcombo->addItem((*obs)->getName());
	}
	QDialogButtonBox* buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
	layout->addWidget(label);
	layout->addWidget(observationsetcombo);
	layout->addWidget(buttonbox);
	QDialog* dialog = new QDialog(this);
	connect(buttonbox, SIGNAL(accepted()), dialog, SLOT(accept()));
	connect(buttonbox, SIGNAL(rejected()), dialog, SLOT(reject()));
	dialog->setWindowTitle("Snap azimuth");
	dialog->setLayout(layout);
	if(dialog->exec() == QDialog::Accepted && !osets.empty()) {
		//ObservationSet::getAzimuth() gets the azimuth from the first point to the last point.
		//We want to set the prism's azimuth -90 from that. Also, add 720 and fmod by 360 to guarantee the azimuth is in (0, 360).
		//	The Item code doesn't care if the azimuth is not in that range, but it looks nicer.
		ui.azimuth->setValue(fmod(osets[observationsetcombo->currentIndex()]->getAzimuth()-90+720,360));
		if(ui.azimuth->value()==360.00) ui.azimuth->setValue(0.00); //rounding of 359.999, eg, can cause 360.00 to be displayed as azimuth, since the field only has 2 decimal places. Replace 360.00 with 0.00.
	}
}
void EditCylinderItemUI::applySlot() {
	if(ci) {
		ci->setName(ui.name->text());
		ci->setActive(ui.active->isChecked());
		ci->setVisible(ui.visible->isChecked());
		ci->setColor(color);
		ci->setDensity(Units::displayToSiDensity(ui.density->value()));
		ci->setCenter(Point(Units::displayToSiDistance(ui.x->value()),Units::displayToSiDistance(ui.y->value()),Units::displayToSiDistance(ui.z->value())));
		ci->setAzimuth(ui.azimuth->value());
		ci->setRadius(ui.radius->value());
	}
}
