#ifndef CLOUDITEM_H
#define CLOUDITEM_H
/*
CloudItem represents a "cloud" of point-mass points.
A cloud of point-masses in close proximity to each other approximates a 3D solid with a density of [total mass]/[total volume of "envelope" around points]

The variable MassItem::density is not meaningful in this child class, at least not until I have some way to calculate an envelope volume around the cloud. That is difficult.
Instead, the user should assign mass to points, to produce an approximate density suitable to the user.
	For now, all points in an item have the same mass. Proximity of points to one another is the only way to have a non-uniform density distribution.
*/
#include "massitem.h"
#include "units.h"

class CloudItem : public MassItem
{
	Point center;
	double pointmass; //Each point has this mass. pointmass*pts.size() is the total mass of the cloud.

	std::vector<Point> pts; // list of points, where coordinates are relative to the center point. Thus a point at (1,2,3) is 1m east, 2m north, and 3m above the center point.

	QString source;
    Units::DistanceUnit distanceunit;
public:
	CloudItem(MainWindow* mw);
	~CloudItem();
	CloudItem* clone();

	double getGravityEffect(Point point);

	Point getCenter();
	double getPointmass();
	std::vector<Point> getPoints();
	Units::DistanceUnit getDistanceUnit();
	QString getSource();
	void setCenter(Point d);
	void setPointmass(double d);
	void setPoints(std::vector<Point> p);
	bool setPointsFromFile(QString f);
	void setDistanceUnit(int u);
	void setSource(QString src);

};

#endif // CLOUDITEM_H
