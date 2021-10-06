#include "editclouditemui.h"
#include "clouditem.h"
#include "mainwindow.h"
#include "units.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QtDebug>

EditCloudItemUI::EditCloudItemUI(QWidget* parent, CloudItem* c, MainWindow* mw) : QDialog(parent), cli(c), mainwindow(mw) {
	ui.setupUi(this);

	connect(ui.applybutton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.cancelbutton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(this, SIGNAL(accepted()), this, SLOT(applySlot()));
	if(cli) {
		ui.pointmassu->setText(Units::massSuffix()); ui.xu->setText(Units::distanceSuffix()); ui.yu->setText(Units::distanceSuffix()); ui.zu->setText(Units::distanceSuffix());
		ui.name->setText(cli->getName());
		ui.active->setChecked(cli->isActive());
		ui.visible->setChecked(cli->isVisible());
			QPixmap coloricon(16,16); coloricon.fill(cli->getColor());
			color = cli->getColor();
			ui.color->setIcon(QIcon(coloricon));
			connect(ui.color, SIGNAL(clicked()), this, SLOT(selectColorSlot()));
		ui.source->setText(cli->getSource());
		connect(ui.browse, SIGNAL(clicked()), this, SLOT(setSourceSlot()));
		ui.unit->clear(); ui.unit->addItem("m"); ui.unit->addItem("ft"); ui.unit->setCurrentIndex(static_cast<int>(cli->getDistanceUnit()));
		ui.pointmass->setValue(Units::siToDisplayMass(cli->getPointmass()));
		ui.x->setValue(Units::siToDisplayDistance(cli->getCenter().x));
		ui.y->setValue(Units::siToDisplayDistance(cli->getCenter().y));
		ui.z->setValue(Units::siToDisplayDistance(cli->getCenter().z));
	}
}
void EditCloudItemUI::selectColorSlot() {
	QColor newcol = QColorDialog::getColor(cli->getColor());
	if(newcol.isValid()){
		QPixmap coloricon(16,16); coloricon.fill(newcol);
		ui.color->setIcon(QIcon(coloricon));
		color = newcol;
	}
}
void EditCloudItemUI::setSourceSlot() {
	QFileDialog dialog(this,"Set Point Cloud source");
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter("Text files (*.txt)");
	if( dialog.exec() ) { 
		QString absolutefilename = dialog.selectedFiles().first();
		//Now choose whether the file will be saved in the project as a relative path or absolute
		//Code duplication with ObservationSet::ObservationSet( )
		bool hasproject = !(mainwindow->getCurrentProjectFile()=="");
		bool absolute = true;
		if(hasproject) {
			QVBoxLayout* pathlayout = new QVBoxLayout();
			QLabel* pathlabel = new QLabel("Refer to this source file by its absolute path or its relative path?\nRelative paths are with respect to your project file.");
			QRadioButton* absoluteradio = new QRadioButton("Absolute path"); absoluteradio->setChecked(true);
			QRadioButton* relativeradio = new QRadioButton("Relative path");
			QDialogButtonBox* pathbuttonbox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
			pathlayout->addWidget(pathlabel);
			pathlayout->addWidget(absoluteradio); pathlayout->addWidget(relativeradio);
			pathlayout->addWidget(pathbuttonbox);
			QDialog* pathdialog = new QDialog(this); pathdialog->setWindowTitle("Choose path type");
			connect(pathbuttonbox, SIGNAL(accepted()), pathdialog, SLOT(accept()));
			pathdialog->setLayout(pathlayout);
			pathdialog->exec();
			absolute = absoluteradio->isChecked();
		}
		if(absolute) {
			ui.source->setText(absolutefilename);
		} else {
			ui.source->setText(QDir::current().relativeFilePath(absolutefilename));
		}
	}
}
void EditCloudItemUI::applySlot() {
	if(cli) {
		cli->setName(ui.name->text());
		cli->setActive(ui.active->isChecked());
		cli->setVisible(ui.visible->isChecked());
		cli->setColor(color);
		cli->setDistanceUnit(ui.unit->currentIndex()); //must go before setSource( )
		cli->setSource(ui.source->text());
		cli->setPointmass(Units::displayToSiMass(ui.pointmass->value()));
		cli->setCenter(Point(Units::displayToSiDistance(ui.x->value()),Units::displayToSiDistance(ui.y->value()),Units::displayToSiDistance(ui.z->value())));
	}
}