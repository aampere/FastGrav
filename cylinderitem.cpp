#include "cylinderitem.h"
#include <QtDebug>

CylinderItem::CylinderItem(MainWindow* mw)
: MassItem(mw), center(0.0,0.0,0.0), azimuth(0.0), azimuthrad(0.0),
  radius(0.0)
{
	type = "CylinderItem";
}
CylinderItem::~CylinderItem() {

}
CylinderItem* CylinderItem::clone() {
	return new CylinderItem(*this);
}

double CylinderItem::getGravityEffect(Point p) {
	//Formula for z-component of a cylinder whose axis is in the x-y plane
	//2G*pi*radius^2*density/
	//(x^2 + z^2)
	//
	//where x is the horizontal "x-y" distance to the cylinder axis, and z is the vertical distance to the axis.

	double axisx = sin(azimuthrad);
	double axisy = cos(azimuthrad); //axisx and axisy define a vector from center that points along the cylinder axis

	double obsx = p.x-center.x;
	double obsy = p.y-center.y;
	double obsmag2 = obsx*obsx + obsy*obsy;

	double dotproduct = axisx*obsx + axisy*obsy; // = |axis||obs|cos(Theta)
	double axislength = dotproduct; //recall that |axis| is 1.0
    double xydistancetoaxis2 = obsmag2-axislength*axislength;
	double zdistancetoaxis = p.z-center.z;

	if( (xydistancetoaxis2+zdistancetoaxis*zdistancetoaxis) >= radius*radius ) { //if the observation point is inside the cylinder, use the standard formula
		return 2.0*constG*pi*radius*radius*zdistancetoaxis*density/
		(xydistancetoaxis2+zdistancetoaxis*zdistancetoaxis);

	} else { //If the observation point is within the cylinder, the outer "shell" of the cylinder ceases to have an effect, so radius is replaces with "r" (distance to axis) which then cancels because r^2 is in numerator and denominator.
		return 2.0*constG*pi*zdistancetoaxis*density;
	}
}

Point CylinderItem::getCenter() {
	return center;
}
double CylinderItem::getRadius() {
	return radius;
}
double CylinderItem::getAzimuth() {
	return azimuth;
}
double CylinderItem::getAzimuthrad() {
	return azimuthrad;
}

void CylinderItem::setCenter(Point p) {
	center = p;
	effectChanged();
}
void CylinderItem::setRadius(double d) {
	radius = d;
	effectChanged();
}
void CylinderItem::setAzimuth(double d) {
	azimuth = d;
	azimuthrad = azimuth/180.0*pi;
	effectChanged();
}
