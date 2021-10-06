#include "saveresultsui.h"
#include "global.h"
#include <QTextStream>

SaveResultsUI::SaveResultsUI(QWidget* parent, ObservationSet* os)
: QDialog(parent), observationset(os)
{
	ui.setupUi(this);

	connect(ui.cancelbutton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui.savebutton, SIGNAL(clicked()), this, SLOT(saveSlot()));
	connect(ui.header, SIGNAL(textChanged(QString)), this, SLOT(updatePreview()));
	connect(ui.tab, SIGNAL(clicked()), this, SLOT(updatePreview()));
	connect(ui.semicolon, SIGNAL(clicked()), this, SLOT(updatePreview()));
	connect(ui.comma, SIGNAL(clicked()), this, SLOT(updatePreview()));
	connect(ui.space, SIGNAL(clicked()), this, SLOT(updatePreview()));
	connect(ui.other, SIGNAL(clicked()), this, SLOT(updatePreview()));
	connect(ui.otherstring, SIGNAL(textChanged(QString)), this, SLOT(updatePreview()));
	connect(ui.nodata, SIGNAL(textChanged(QString)), this, SLOT(updatePreview()));

	ui.header->setText(QString("Gravity results for \"%1\"").arg(os->getName()));
	ui.nodata->setText(QString::number(noData(), 'g',12));
	updatePreview();
}

void SaveResultsUI::updatePreview() {
	previewtext = "";
	QString delim = "\t";
	if(ui.tab->isChecked()) delim = "\t";
	else if(ui.semicolon->isChecked()) delim = ";";
	else if(ui.comma->isChecked()) delim = ",";
	else if(ui.space->isChecked()) delim = " ";
	else if(ui.other->isChecked()) delim = ui.otherstring->text();

	previewtext += ui.header->text() + "\n";
	previewtext += QString("Length unit: %1\n").arg(Units::distanceSuffix());
	previewtext += QString("Acceleration unit: %1\n").arg(Units::gravSuffix());
	previewtext += QString("Nodata value: %1\n").arg(ui.nodata->text());
	previewtext += QString("x%1y%2z%3emp%4mod\n").arg(delim).arg(delim).arg(delim).arg(delim);
	std::vector<Point> pts = observationset->getPoints();
	std::vector<double> emp = observationset->getEmpGravityVals();
    for(uint i=0; i!=pts.size(); ++i) {
		previewtext += QString("%1%2%3%4%5%6%7%8%9\n")
			.arg(QString::number(Units::siToDisplayDistance(pts[i].x),'g',12)).arg(delim)
			.arg(QString::number(Units::siToDisplayDistance(pts[i].y),'g',12)).arg(delim)
			.arg(QString::number(Units::siToDisplayDistance(pts[i].z),'g',12)).arg(delim)
			.arg(isNoData(emp[i])?ui.nodata->text():QString::number(Units::siToDisplayGrav(emp[i]),'g',12)).arg(delim)
			.arg(QString::number(Units::siToDisplayGrav(observationset->getOffsetGravityEffect(i)),'g',12));
	}
	ui.preview->setText(previewtext);
}

void SaveResultsUI::saveSlot() {
	QFileDialog dialog(this,"Save Results");
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setLabelText(QFileDialog::Accept,"Save");
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setNameFilter("Text file (*.txt)");
	if( dialog.exec() ) { 
		QString fileName = dialog.selectedFiles().first();
		if(!fileName.endsWith(".txt")) fileName = fileName+".txt";
		QFile file(fileName);
		if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QTextStream out(&file);
			out << previewtext;
		}
		file.close();
	}
}
