#include "sphereitem.h"

SphereItem::SphereItem(MainWindow* mw)
: MassItem(mw), center(0.0,0.0,0.0), radius(0.0), volume(0.0) {
	type = "SphereItem";
}
SphereItem::~SphereItem() {

}
SphereItem* SphereItem::clone() {
	return new SphereItem(*this);
}

double SphereItem::getGravityEffect(Point p) {
	//Formula for z-component of a point-mass, where x,y,z are the coords of (obs. point)-(mass point)
	//zmG/(r^3)
	//where r = sqrt(x^2+y^2+z^2)
	//
	double z = p.z-center.z;
	double x = p.x-center.x;
	double y = p.y-center.y;
	double r3 = pow(x*x+y*y+z*z, 1.5); //r^(3)
	if(r3>=radius*radius*radius) {
		return z*constG*volume*density/r3; //Use pointmass formula if the observation point is outside the sphere

	} else { //If the observation point is within the sphere, then mass outside between r and radius "doesn't matter" (cancels itself out). So use r as radius.
		return z*constG*4.0/3.0*pi*density; //r3 on top and bottom of fraction cancels.
	}
}

Point SphereItem::getCenter() {
	return center;
}
double SphereItem::getRadius() {
	return radius;
}

void SphereItem::setCenter(Point p) {
	center = p;
	effectChanged();
}
void SphereItem::setRadius(double d) {
    radius = std::abs(d);
	volume = 4.0/3.0*pi*radius*radius*radius;
	effectChanged();
}
