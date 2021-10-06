#ifndef CYLINDERITEM_H
#define CYLINDERITEM_H

#include "massitem.h"

class CylinderItem : public MassItem
{
	Point center;
	double azimuth;//the cylinder axis goes through *center*, with *azimuth* azimuth degrees
	double azimuthrad;//azimuth in radians
	double radius;

public:
	CylinderItem(MainWindow* mw);
	~CylinderItem();
	CylinderItem* clone();

	double getGravityEffect(Point point);

	Point getCenter();
	double getRadius();
	double getAzimuth();
	double getAzimuthrad();
	void setCenter(Point d);
	void setRadius(double d);
	void setAzimuth(double d);
};

#endif // CYLINDERITEM_H
