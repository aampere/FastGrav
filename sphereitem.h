#ifndef SPHEREITEM_H
#define SPHEREITEM_H

#include "massitem.h"

class SphereItem : public MassItem
{
	Point center;
	double radius;
	double volume;

public:
	SphereItem(MainWindow* mw);
	~SphereItem();
	SphereItem* clone();

	double getGravityEffect(Point point);

	Point getCenter();
	double getRadius();
	void setCenter(Point d);
	void setRadius(double d);
};

#endif // SPHEREITEM_H
