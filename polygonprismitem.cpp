#include "polygonprismitem.h"
#include "units.h"
#include "global.h"
#include <QtDebug>
PolygonPrismItem::PolygonPrismItem(MainWindow* mw)
: MassItem(mw), center(0.0,0.0,0.0), azimuth(0.0), azimuthrad(0.0) {
	type = "PolygonPrismItem";
	//vertices.push_back(Point( 3.00, 0.00));
	//vertices.push_back(Point( 0.71,-0.71));
	//vertices.push_back(Point( 0.00,-1.00));
	//vertices.push_back(Point(-0.71,-0.71));
	//vertices.push_back(Point(-1.00, 0.00));
	//vertices.push_back(Point(-0.71, 0.71));
	//vertices.push_back(Point( 0.00, 1.00));
	//vertices.push_back(Point( 0.71, 0.71));
	

}
PolygonPrismItem::~PolygonPrismItem() {

}
PolygonPrismItem* PolygonPrismItem::clone() {
	return new PolygonPrismItem(*this);
}

double PolygonPrismItem::getGravityEffect(Point p) {
	//The Talwani(1959) method is appplied here, using the more efficient algorithm of Won and Bevis (1987)

	double axisx = sin(azimuthrad);
	double axisy = cos(azimuthrad); //axisx and axisy define a vector from center that points along the cylinder axis

	double obsx = p.x-center.x;
	double obsy = p.y-center.y;
	//change of coordinate method below
	
	//let a=<axisx,axisy,0>, the azimuth vector, define one basis vector of the new coord system.
	//The other basis vector will be the unit vector orthogonal to the azimuth vector, and pointing to the right of the azimuth vector.
	//This other basis vector is found by <axisx,axisy,0>x<0,0,1>, which gives the vector b=<axisy,-axisx,0>
	//p=<obsx,obsy,0> is some linear combination of a and b.     p = A*a + B*b
	//Find B, this is the distance from the axis to the observation point, where right is positive.
	double scalarB = axisy*obsx-axisx*obsy;
	double xydistancetoaxis = -scalarB;

	//change of coordinate method above
	/*//Square root method below
	double obsmag2 = obsx*obsx + obsy*obsy;

	double dotproduct = axisx*obsx + axisy*obsy; // = |axis||obs|cos(Theta)
	double axislength = dotproduct; //recall that |axis| is 1.0
	double xydistancetoaxis2 = obsmag2-axislength*axislength;
	double xydistancetoaxis = sqrt(xydistancetoaxis2);
	//xydistancetoaxis is POSITIVE at this point, regardless of which side of the azimuth vector p is on. Below, we negate xydistancetoaxis if p is to the right of the azimuth vector.
	if(axisx==0.0) {
		if(obsx>0) {
			xydistancetoaxis = -xydistancetoaxis;
		}
	} else if(p.y<(axisy/axisx)*(p.x-center.x)+center.y) {
		xydistancetoaxis = -xydistancetoaxis;
	}
	*/ //Square root method above
	double zdistancetoaxis = p.z-center.z; 
	//Above simply deals with the azimuth issue, and is the same for horizontal cylinders.
	//Below, the method of Talwani(1959) and Won and Bevis (1987) is implemented.
	
	double zsum = 0; //The sum in (1) of Won and Bevis (1987)
	for(int i=0; i<vertices.size(); ++i) {
		double zinc = -999;
		int j = (i+1)%vertices.size(); //j is the index of the "next" point, looping around to point 0
		double xi = xydistancetoaxis + vertices[i].x;
		double zi = zdistancetoaxis - vertices[i].y;
		double xj = xydistancetoaxis + vertices[j].x;
		double zj = zdistancetoaxis - vertices[j].y;
		//QString caseflag = "NONE  ";
		if((xi==0&&zi==0) || (xj==0&&zj==0)) {
			zinc = 0.0;
			//caseflag = "ONPT  ";
		} else if(xi==xj) {
			double ri = sqrt(xi*xi+zi*zi);
			double rj = sqrt(xj*xj+zj*zj);
			zinc = xi*log(rj/ri);
			//caseflag = "EQX   ";
		} else {
			double A = (xj-xi)*(xi*zj-xj*zi)/( pow(xj-xi,2) + pow(zj-zi,2)  );
			double B = (zj-zi)/(xj-xi);
			double ri = sqrt(xi*xi+zi*zi);
			double rj = sqrt(xj*xj+zj*zj);
			double thetai = atan2(zi,xi);
			double thetaj = atan2(zj,xj);
			double thetaiminusthetaj;
			bool go = true;
			//caseflag = "NOSIGN";
			if(((zi<0) != (zj<0)) ) {   //if have opposize signs
				if(xi*zj<xj*zi && zj>=0) {
					thetai = thetai + 2*pi;
					//caseflag = "A     ";
				}
				if(xi*zj>xj*zi && zi>=0) {
					thetaj = thetaj + 2*pi;
					//caseflag = "B     ";
				}
				if(xi*zj==xj*zi) {
					zinc = 0.0;
					go = false;
					//caseflag = "C     ";
				}
			}
			thetaiminusthetaj = thetai - thetaj;
			if(go) {
				zinc = A*( (thetaiminusthetaj) + B*log(rj/ri) );
			}
		}
		if(zinc == -999) {
			logline(QString("PolygonPrism line sum, critical error. %1").arg(getName()));
		}
		zsum += zinc;
	}
	
	return 2*constG*density*zsum; 
}

Point PolygonPrismItem::getCenter() {
	return center;
}
double PolygonPrismItem::getAzimuth() {
	return azimuth;
}
double PolygonPrismItem::getAzimuthrad() {
	return azimuthrad;
}
std::vector<Point> PolygonPrismItem::getVertices() {
	return vertices;
}

void PolygonPrismItem::setCenter(Point p) {
	center = p;
	effectChanged();
}
void PolygonPrismItem::setAzimuth(double d) {
	azimuth = d;
	azimuthrad = azimuth/180.0*pi;
	effectChanged();
}
void PolygonPrismItem::setVertices(std::vector<Point> p) {
	vertices = p;
	effectChanged();
}
void PolygonPrismItem::setVertex(int i, double nx, double nz) {
	if(i>=0 && i<vertices.size()) {
		vertices[i] = Point(nx, nz);
	}
	effectChanged();
}
void PolygonPrismItem::insertVertex(int i, double x, double z) {
	if(i>=0 && i<vertices.size()) {
		vertices.insert(vertices.begin()+i+1, Point(x,z));
	}
	effectChanged();
}
void PolygonPrismItem::removeVertex(int i) {
	if(i>=0 && i<vertices.size()) {
		vertices.erase(vertices.begin()+i);
	}
	effectChanged();
}
