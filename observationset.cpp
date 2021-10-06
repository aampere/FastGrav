#include "observationset.h"
#include "global.h"
#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDialogButtonBox>
#include <QtDebug>


ObservationSet::ObservationSet(std::vector<Point> pts, MainWindow* mw)
: mainwindow(mw), table(nullptr), tablewindow(nullptr), sourcefile("NOSOURCE"), listwidgetitem(nullptr), hasunits(false), valid(true), points(pts)   /*UPDATE OTHER CONSTRUCTORS TOO!*/ {
	 //NOT CURRENTLY USED ANYWHERE
} 
ObservationSet::ObservationSet(QString fname, MainWindow* mw)
: mainwindow(mw), table(nullptr), tablewindow(nullptr), sourcefile(fname), listwidgetitem(nullptr), hasunits(false) , valid(true)/*UPDATE OTHER CONSTRUCTORS TOO!*/ {
	QString absolutefilename = sourcefile;
	//Now choose whether the file will be saved in the project as a relative path or absolute
	//Code duplication with EditCloudItemUI::setSourceSlot(), several modifications
	bool hasproject = !(mainwindow->getCurrentProjectFile()=="");
	bool absolute = true;
	if(hasproject) {
		QVBoxLayout* pathlayout = new QVBoxLayout();
		QLabel* pathlabel = new QLabel("Refer to this source file by its absolute path or its relative path?");
		QRadioButton* absoluteradio = new QRadioButton("Absolute path"); absoluteradio->setChecked(true);
		QRadioButton* relativeradio = new QRadioButton("Relative path");
		QDialogButtonBox* pathbuttonbox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, mainwindow);
		pathlayout->addWidget(pathlabel);
		pathlayout->addWidget(absoluteradio); pathlayout->addWidget(relativeradio);
		pathlayout->addWidget(pathbuttonbox);
		QDialog* pathdialog = new QDialog(mainwindow); pathdialog->setWindowTitle("Choose path type");
		connect(pathbuttonbox, SIGNAL(accepted()), pathdialog, SLOT(accept()));
		pathdialog->setLayout(pathlayout);
		pathdialog->exec();
		absolute = absoluteradio->isChecked();
	}
	if(absolute) {
		sourcefile = absolutefilename;
	} else {
		sourcefile = QDir::current().relativeFilePath(absolutefilename);
	}
	//Now select import units
	QDialog* unitwindow = new QDialog(mainwindow);
	unitsui.setupUi(unitwindow);
	connect(unitsui.cancelbutton, SIGNAL(clicked()), unitwindow, SLOT(reject()));
	connect(unitsui.okbutton, SIGNAL(clicked()), unitwindow, SLOT(accept()));
	unitsui.acceleration->clear();
	unitsui.acceleration->addItem("m/s^2");
	unitsui.acceleration->addItem("mGal");
	unitsui.acceleration->setCurrentIndex(static_cast<int>(Units::displaygravunit));
	unitsui.distance->clear();
	unitsui.distance->addItem("m");
	unitsui.distance->addItem("ft");
	unitsui.distance->setCurrentIndex(static_cast<int>(Units::displaydistanceunit));
	if(unitwindow->exec() == QDialog::Accepted) {
		initialize(fname, static_cast<Units::GravityUnit>(unitsui.acceleration->currentIndex()),  static_cast<Units::DistanceUnit>(unitsui.distance->currentIndex()),mw );
	} else {
		hasunits = false;
	}
} 
ObservationSet::ObservationSet(QString fname, Units::GravityUnit gravu, Units::DistanceUnit distu, MainWindow* mw)
: mainwindow(mw), table(nullptr), tablewindow(nullptr), sourcefile(fname), listwidgetitem(nullptr), hasunits(true), valid(true) {
	initialize(fname, gravu, distu, mw);
}
void ObservationSet::initialize(QString fname, Units::GravityUnit gravu, Units::DistanceUnit distu, MainWindow* mw) {
	hasunits = true;
	distanceu = distu;
	gravityu = gravu;
	initializeFromFile(fname);//this function sets -points- and -name-
	calculateGravityEffects();//for now, call this automatically upon construction.
}
ObservationSet::~ObservationSet() {

}

bool ObservationSet::hasUnits() {
	return hasunits;
}
bool ObservationSet::isValid() {
	return valid;
}

void ObservationSet::setTable(ObservationSetTable* ost, QMdiSubWindow* sub) {
	if(table && ost!=nullptr) {
		logline("Observation set " + getName() + " overwriting its table.");
	}
	table = ost;
	tablewindow = sub;
	if(table) {
		table->setTitleLabel(getName());
	}
	updateTable();
}
void ObservationSet::closeTable() {
	table = nullptr;
	if(tablewindow) {
		tablewindow->close();
	}
	tablewindow = nullptr;
}
ObservationSetTable* ObservationSet::getTable() {
	return table;
}
void ObservationSet::updateTable() {
	if(table) {
		table->clearContents();
		table->setRowColumnCount(points.size(), 3);
        for(uint i=0; i!=points.size(); ++i) {
			QTableWidgetItem* pointitem = new QTableWidgetItem(QString("(%0, %1, %2)").arg(Units::siToDisplayDistance(points[i].x)).arg(Units::siToDisplayDistance(points[i].y)).arg(Units::siToDisplayDistance(points[i].z)));
			QTableWidgetItem* empitem = new QTableWidgetItem( isNoData(empgravityvals[i])?"":QString::number(Units::siToDisplayGrav(empgravityvals[i])) );				//display nothing in this cell if there is no emprical data at this point
			QTableWidgetItem* effectitem = new QTableWidgetItem(QString::number(Units::siToDisplayGrav(getOffsetGravityEffect(i))));
			pointitem->setFlags(pointitem->flags() & ~Qt::ItemIsEditable);
			empitem->setFlags(empitem->flags() & ~Qt::ItemIsEditable);
			effectitem->setFlags(effectitem->flags() & ~Qt::ItemIsEditable);
			table->setItem(i,0,pointitem);
			table->setItem(i,1,empitem);
			table->setItem(i,2,effectitem);
			table->setColumnHeaders(QStringList()<<QString("Location (%1)").arg(Units::distanceSuffix())<<QString("Empirical (%1)").arg(Units::gravSuffix())<<QString("Modeled (%1)").arg(Units::gravSuffix()));
		}
	}
}

void ObservationSet::calculateGravityEffects() {
	//recall: gravityeffects[1][7] refers to Point 1 and MassItem* 7
	gravityeffects.clear();
	gravityeffects.resize(points.size());
	auto massitems = mainwindow->massItems();
    for(uint pi=0; pi!=points.size(); ++pi) {
		gravityeffects[pi].resize(massitems.size());
        for(uint mi=0; mi!=massitems.size(); ++mi) {
			gravityeffects[pi][mi] = massitems[mi]->getGravityEffect(points[pi]);
		}
	}
	updateTable();
}
void ObservationSet::calculateGravityEffect(int index) {
	//recall: gravityeffects[1][7] refers to Point 1 and MassItem* 7
	auto massitems = mainwindow->massItems();
	if(index<massitems.size()) {
        for(uint i=0; i!=points.size(); ++i) {
			gravityeffects[i][index] = massitems[index]->getGravityEffect(points[i]);
		}
	} else {
		logline("ObservationSet::getSumGravityEffect( ) " + getName() + " has bad index.");
	}
	updateTable();
}
double ObservationSet::getSumGravityEffect(int index) {
	//recall: gravityeffects[1][7] refers to Point 1 and MassItem* 7
	auto massitems = mainwindow->massItems();
	double result = 0.0;
	if(index<gravityeffects.size()) {
        for(uint j=0; j!=gravityeffects[index].size(); ++j) {
			if(massitems[j]->isActive())
				result += gravityeffects[index][j];
		}
	} else {
		logline("ObservationSet::getSumGravityEffect( ) " + getName() + " has bad index.");
		return 0.0;
	}
	return result;
}
double ObservationSet::getOffsetGravityEffect(int index) {
	if(mainwindow) {
		return getSumGravityEffect(index) + mainwindow->getGravityOffset();
	}
	return getSumGravityEffect(index);
}
void ObservationSet::removeMassItemGravityEffect(int index) {
	//recall: gravityeffects[1][7] refers to Point 1 and MassItem* 7
    for(uint pi=0; pi!=points.size(); ++pi) {
		if(index < gravityeffects[pi].size())
			gravityeffects[pi].erase(gravityeffects[pi].begin()+index);
		else
			logline("ObservationSet::removeMassItemGravityEffect( ) " + getName() + " bad remove index.");
	}
	updateTable();
}
void ObservationSet::addMassItemGravityEffect(int index) {
	//recall: gravityeffects[1][7] refers to Point 1 and MassItem* 7
    for(uint pi=0; pi!=points.size(); ++pi) {
		if(index < gravityeffects[pi].size()+1)
			gravityeffects[pi].insert(gravityeffects[pi].begin()+index, 0.0); //insert 0.0 for now, below the actual gravity effect will be calculated.
		else
			logline("ObservationSet::addMassItemGravityEffect( ) "+getName() + " bad add index.");
	}
	calculateGravityEffect(index); 
}

QString ObservationSet::getName() {
	return name;
}
std::vector<Point> ObservationSet::getPoints(){
	return points;
}
std::vector<double> ObservationSet::getEmpGravityVals() {
	return empgravityvals;
}
QString ObservationSet::getSourceFile() {
	return sourcefile;
}
int ObservationSet::getSize() {
	return points.size();
}
double ObservationSet::getAzimuth() {
	//calculating on the fly, because it is never called repeatedly.
	//returned in degrees
	if(!points.empty()) {
		return atan2(points[points.size()-1].x-points[0].x, points[points.size()-1].y-points[0].y)*180/3.14159265359;
	}
	return 0.0;
}
void ObservationSet::setName(QString n) {
	name = n;
	if(listwidgetitem)
		listwidgetitem->setText(name);
}

void ObservationSet::initializeFromFile(QString fname) {
	Units::GravityUnit originalgravityunit = Units::displaygravunit; //We are about to temporarily change the global display units for exploit the display unit conversion functions
	Units::DistanceUnit originaldistanceunit = Units::displaydistanceunit; //We'll restore the original unit types at the end of initializeFromFile().
	Units::displaygravunit = gravityu;
	Units::displaydistanceunit = distanceu;

	QFile file(fname);

	std::vector<Point> pts;
	std::vector<double> evals;
	QString osname;
	logline("Reading Observation Set from file " + fname +" ...");
	if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&file);
		bool firstline = true;
		while(!in.atEnd()) {
			QString line = in.readLine();
			logline(line);
			if(firstline) {
				osname = line; //Always, the first line is NOT read as data, but as a title/header
				firstline = false;
			} else {
				QStringList parts = line.split("\t");
				double x;
				double y;
				double z;
				double emp;
				if(parts.size()==4) {  //x y z empiricalgrav
					//Note: Empirical gravity nodata is -999999
					bool okx;
					bool oky;
					bool okz;
					bool okemp;
					x = Units::displayToSiDistance(parts[0].toDouble(&okx));
					y = Units::displayToSiDistance(parts[1].toDouble(&oky));
					z = Units::displayToSiDistance(parts[2].toDouble(&okz));
					if(isNoData(parts[3].toDouble(&okemp))) {
						emp = parts[3].toDouble(&okemp); //if the empirical data is no data, don't do unit conversion on the value, it needs to STAY the nodata value!
					} else {
						emp = Units::displayToSiGrav(parts[3].toDouble(&okemp)); //if it is NOT nodata, do the unit conversion
					}
					if(okx&&oky&&okz) {
						pts.push_back(Point(x,y,z));
					} else {
						logline(QString("\tObservationSet read from file: \"%1 %2 %3\" Not all point components could be converted to double.").arg(parts[0]).arg(parts[1]).arg(parts[2]));
					}
					if(okemp) {
						//Note: Empirical gravity nodata is -999999
						evals.push_back(emp);
					} else {
						logline(QString("\tObservationSet read from file: empirical gravity value \"%1\" could not be converted to double.").arg(parts[4]));
					}
				} else {
					logline(QString("\tObservationSet read from file: 4 data expected. Line skipped."));
					break;
				}
			}
		}
		file.close();
	} else {
		logline(QString("Observation Set file could not be opened: %1").arg(fname));
		valid = false; //member variable valid, identifies this observationset as bad
	}
	Units::displaygravunit = originalgravityunit; //Reset the display unit to whatever it was before
	Units::displaydistanceunit = originaldistanceunit; //Reset the display unit to whatever it was before
	points = pts;
	if(points.empty()){
		valid = false;
	} else {
		valid = true;
	}
	empgravityvals = evals;
	setName(osname);
	//I could add something here where the observation set automatically removes/deletes itself if the file fails to open, or has no points
}

QListWidgetItem* ObservationSet::listWidgetItem() {
	return listwidgetitem;
}
void ObservationSet::setListWidgetItem(QListWidgetItem* lwi) {
	listwidgetitem = lwi;
}
