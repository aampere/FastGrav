#ifndef POLYGONPRISMITEM_H
#define POLYGONPRISMITEM_H
/*
This item represents an infinitely-long, horizontal prism with a polygon crosssection. AKA, a "2D" polygon.
This is the type of body whose gravitational effect Talwani (1959) first described.
The calculation used here is from I.J. Won and Michael Bevis (1987), "Computing the gravitational and magnetic anomalies due to a polygon: Algorithms and Fortran subroutines"
	which is an improved way to calculate the gravitational effect using the Talwani method.
The prism may have any azimuth, but will always be horizontal.
The order of polygon vertices *IS SIGNIFICANT.* They must be CW w/ respect to azimuth (Investigate this).
The polygon need not be convex, but its edges must not cross over each other. It is up to THE USER to ensure that edges do not cross.

PolygonPrismItem is intended to be used as an alternative to a simple infinite cylinder,
	or with other PolygonPrismItems to construct a 2D cross-section.

*/
#include <QFile>
#include <QTextStream>

#include "massitem.h"

class PolygonPrismItem : public MassItem
{
	Point center;
	double azimuth;//the cylinder axis goes through *center*, with *azimuth* azimuth degrees
	double azimuthrad;//azimuth in radians

	std::vector<Point> vertices;
	//List of vertices that define the prism cross-section. These points are defined in 2D, around a (0, 0) point.
	//Thus (-1, 0), (0, 1), (1, 0), (0, -1) defines a square diamond about the (0,0) axis.
	//The prism is situated in space using -center- and -azimuth-, but these cross-section points will not change unless edited.

public:
	PolygonPrismItem(MainWindow* mw);
	~PolygonPrismItem();
	PolygonPrismItem* clone();

	double getGravityEffect(Point point);

	Point getCenter();
	double getAzimuth();
	double getAzimuthrad();
	std::vector<Point> getVertices();
	void setCenter(Point d);
	void setAzimuth(double d);
	void setVertices(std::vector<Point> p);
	void setVertex(int i, double nx, double nz);
	void insertVertex(int i, double x, double z);
	void removeVertex(int i);
};

#endif // POLYGONPRISMITEM_H
