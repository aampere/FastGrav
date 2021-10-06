#ifndef POINT_H
#define POINT_H

struct Point {

	Point();
	Point(double x,double y,double z=0.0); //Create a point from x, y,z
	void set(double x,double y,double z=0.0); //Explicitly set x,y,z
	void setX(double);
	void setY(double);
	void setZ(double);   

	double x;
	double y;
	double z;
};
#endif //POINT_H