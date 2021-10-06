#include "editpolygonprismitemui.h"
#include "polygonprismitem.h"
#include "mainwindow.h"
#include "units.h"
#include <QColorDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QtDebug>

EditPolygonPrismItemUI::EditPolygonPrismItemUI(QWidget* parent, PolygonPrismItem* p, MainWindow* mw) : QDialog(parent), ppi(p), mainwindow(mw) {
	ui.setupUi(this);
	
	connect(ui.applybutton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.cancelbutton, SIGNAL(clicked()), this, SLOT(reject()));//need to delete as well? not just close? Or make it non-pointer....
	connect(this, SIGNAL(accepted()), this, SLOT(applySlot()));
	if(ppi) {
		ui.densityu->setText(Units::densitySuffix()); ui.xu->setText(Units::distanceSuffix()); ui.yu->setText(Units::distanceSuffix()); ui.zu->setText(Units::distanceSuffix());
		ui.name->setText(ppi->getName());
		ui.active->setChecked(ppi->isActive());
		ui.visible->setChecked(ppi->isVisible());
			QPixmap coloricon(16,16); coloricon.fill(ppi->getColor());
			color = ppi->getColor();
			ui.color->setIcon(QIcon(coloricon));
			connect(ui.color, SIGNAL(clicked()), this, SLOT(selectColorSlot()));
		connect(ui.snapazimuth, SIGNAL(clicked()), this, SLOT(snapAzimuthSlot()));
		ui.density->setValue(Units::siToDisplayDensity(ppi->getDensity()));
		ui.x->setValue(Units::siToDisplayDistance(ppi->getCenter().x));
		ui.y->setValue(Units::siToDisplayDistance(ppi->getCenter().y));
		ui.z->setValue(Units::siToDisplayDistance(ppi->getCenter().z));
		ui.azimuth->setValue(ppi->getAzimuth());
		//set table of points 
		auto vertices = ppi->getVertices();
		ui.vertices->setRowCount(vertices.size());
		ui.vertices->resizeRowsToContents();
        for(uint i=0; i!=vertices.size(); ++i) {
			ui.vertices->setItem(i,0, new QTableWidgetItem(QString::number(Units::siToDisplayDistance(vertices[i].x))));
			ui.vertices->setItem(i,1, new QTableWidgetItem(QString::number(Units::siToDisplayDistance(vertices[i].y))));
		}
		connect(ui.vertices, SIGNAL(cellChanged(int, int)), this, SLOT(validateCell(int, int)));
	}

	connect(ui.addvertex, SIGNAL(clicked()), this, SLOT(addVertex()));

	ui.vertices->setContextMenuPolicy(Qt::ActionsContextMenu);
	QAction* deleterows = new QAction("Delete these rows", ui.vertices);
	connect(deleterows, SIGNAL(triggered()), this, SLOT(deleteRows()));
	ui.vertices->addAction(deleterows);
}
void EditPolygonPrismItemUI::validateCell(int i, int j) {
	QString text = ui.vertices->item(i,j)->text();
	bool ok;
	text.toDouble(&ok);
	if(!ok) {
		ui.vertices->item(i,j)->setText("0.0");
	}
}
void EditPolygonPrismItemUI::addVertex() {
	ui.vertices->setRowCount(ui.vertices->rowCount()+1);
	if(ui.vertices->rowCount()==1) {
		ui.vertices->setItem(0,0, new QTableWidgetItem("0.0"));
		ui.vertices->setItem(0,1, new QTableWidgetItem("0.0"));
	} else {
		ui.vertices->setItem(ui.vertices->rowCount()-1,0, new QTableWidgetItem(ui.vertices->item(ui.vertices->rowCount()-2, 0)->text()));
		ui.vertices->setItem(ui.vertices->rowCount()-1,1, new QTableWidgetItem(ui.vertices->item(ui.vertices->rowCount()-2, 1)->text()));
	}
}
void EditPolygonPrismItemUI::deleteRows() {
	QSet<int> deletedrows;
	auto ranges = ui.vertices->selectedRanges();
	for(auto range=ranges.begin(); range!=ranges.end(); ++range) { //loop through the selected ranges.
		for(int rowi=range->topRow(); rowi!=range->bottomRow()+1; ++rowi) {
			deletedrows << rowi;
		}
	}
	QList<int> deletedrowslist = deletedrows.toList();
	std::sort(deletedrowslist.begin(),deletedrowslist.end(),std::greater<int>()); //sort in *descending* order
	for(auto r=deletedrowslist.begin(); r!=deletedrowslist.end(); ++r) {
		ui.vertices->removeRow(*r);//remove them one at a time. the indices are in descending order, so this is okay.
	}
	ui.vertices->setRangeSelected(QTableWidgetSelectionRange(0,0,0,0),false);
}
void EditPolygonPrismItemUI::selectColorSlot() {
	QColor newcol = QColorDialog::getColor(ppi->getColor());
	if(newcol.isValid()){
		QPixmap coloricon(16,16); coloricon.fill(newcol);
		ui.color->setIcon(QIcon(coloricon));
		color = newcol;
	}
}
void EditPolygonPrismItemUI::snapAzimuthSlot() {
	//code duplication with EditCylinderItemUI
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
void EditPolygonPrismItemUI::applySlot() {
	if(ppi) {
		ppi->setName(ui.name->text());
		ppi->setActive(ui.active->isChecked());
		ppi->setVisible(ui.visible->isChecked());
		ppi->setColor(color);
		ppi->setDensity(Units::displayToSiDensity(ui.density->value()));
		ppi->setCenter(Point(Units::displayToSiDistance(ui.x->value()),Units::displayToSiDistance(ui.y->value()),Units::displayToSiDistance(ui.z->value())));
		ppi->setAzimuth(ui.azimuth->value());
		std::vector<Point> newvertices;
		for(int i=0; i!=ui.vertices->rowCount(); ++i) {
			newvertices.push_back(Point(Units::displayToSiDistance(ui.vertices->item(i,0)->text().toDouble()), Units::displayToSiDistance(ui.vertices->item(i,1)->text().toDouble())));
		}
		ppi->setVertices(newvertices);
	}
}
