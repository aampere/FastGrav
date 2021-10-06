#include "point.h"

Point::Point()
: x(0),y(0),z(0) {}

Point::Point(double s,double t,double u)
: x(s),y(t),z(u) {}


void Point::set(double s,double t,double u) {
   x = s;
   y = t;
   z = u;
}

void Point::setX(double x) {
   this->x = x;
}

void Point::setY(double y) {
   this->y = y;
}

void Point::setZ(double z) {
   this->z = z;
}