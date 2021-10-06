#include "clouditem.h"
#include "global.h"
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <cmath>

CloudItem::CloudItem(MainWindow* mw)
: MassItem(mw), center(0.0,0.0,0.0), pointmass(0.0), source(""), distanceunit(Units::M) {
	type = "CloudItem";
}
CloudItem::~CloudItem() {

}
CloudItem* CloudItem::clone() {
	return new CloudItem(*this);
}
double CloudItem::getGravityEffect(Point p) {
	//Formula for z-component of a point-mass, where x,y,z are the coords of (obs. point)-(mass point)
	//zmG/(r^3)
	//where r = sqrt(x^2+y^2+z^2)
	double sumeffect = 0.0;
	for(auto cloudpoint=pts.begin(); cloudpoint!=pts.end(); ++cloudpoint) {
		double z = p.z-center.z-cloudpoint->z;//Z
		double x = p.x-center.x-cloudpoint->x;//X
		double y = p.y-center.y-cloudpoint->y;//Y
        double r3 = pow(x*x+y*y+z*z, 1.5); //r^(3)
		if(r3!=0)
			sumeffect += z*constG*pointmass/r3; // Just add them up one by one. It might be good to add them smallest to largest in a very large cloud...?
	}
	return sumeffect;
}

Point CloudItem::getCenter() {
	return center;
}
double CloudItem::getPointmass() {
	return pointmass;
}
std::vector<Point> CloudItem::getPoints() {
	return pts;
}
Units::DistanceUnit CloudItem::getDistanceUnit() {
	return distanceunit;
}
QString CloudItem::getSource() {
	return source;
}

void CloudItem::setCenter(Point p) {
	center = p;
	effectChanged();
}
void CloudItem::setPointmass(double d) {
	pointmass = d;
	effectChanged();
}
void CloudItem::setPoints(std::vector<Point> p) {
	pts = p;
	effectChanged();
}
bool CloudItem::setPointsFromFile(QString f) {
	//parse file into points.
	QFile file(f);
	if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		//We are about to temporarily change the global display units for exploit the display unit conversion functions
		Units::DistanceUnit originaldistanceunit = Units::displaydistanceunit; //We'll restore the original unit types at the end of setPointsFromFile().
		Units::displaydistanceunit = distanceunit;

		QTextStream in(&file);
		QString line;
		QStringList parts;
		double xval;
		double yval;
		double zval;
		bool okx;
		bool oky;
		bool okz;
		bool badpointmass = false;
		std::vector<Point> ps;
		while(!in.atEnd()) {
			line = in.readLine();
			parts = line.split("\t");
			//All following points are point masses.
			if(parts.size()==3) {
				xval = Units::displayToSiDistance(parts[0].toDouble(&okx));
				yval = Units::displayToSiDistance(parts[1].toDouble(&oky));
				zval = Units::displayToSiDistance(parts[2].toDouble(&okz));
				if(okx&&oky&&okz) {
					ps.push_back(Point(xval, yval, zval));
				} else {
					badpointmass = true;
				}
			} else {
				badpointmass = true;
			}
		}
		if(badpointmass) logline("CloudItem file parse: at lease one line could not be parsed. Some points may not have been added.");
		setPoints(ps);
		Units::displaydistanceunit = originaldistanceunit; //Reset the display unit to whatever it was before
		return true;
	}
	return false;
}
void CloudItem::setDistanceUnit(int u) {
	distanceunit = static_cast<Units::DistanceUnit>(u);
}
void CloudItem::setSource(QString src) {
	if(setPointsFromFile(src)) { //this function calls effectChanged()
		source = src;
	} else {
		source = "[invalid source]";
		setPoints(std::vector<Point>());//The source was bad. Replace the current points with a empty list of points.
	}
}
