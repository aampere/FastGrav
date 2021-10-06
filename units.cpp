#include "units.h"

Units::GravityUnit Units::displaygravunit = Ms2;//initializer for the static data member
void Units::setGravityDisplayUnit(GravityUnit u) {
	displaygravunit = u;
}
double Units::siToDisplayGrav(double sigravityvalue) {
	switch(displaygravunit) {
	case Ms2:
		return sigravityvalue;
	case MGal:
		return sigravityvalue*100000.0; //To get mGal from m/s^2, multiply by 100,000
	}
	return sigravityvalue;
}
double Units::displayToSiGrav(double displaygravityvalue) {
	switch(displaygravunit) {
	case Ms2:
		return displaygravityvalue;
	case MGal:
		return displaygravityvalue*0.00001; //To get m/s^2 from mGal, multiply by 0.00001 (divide by 100,000)
	}
	return displaygravityvalue;
}
QString Units::gravSuffix() {
	switch(displaygravunit) {
	case Ms2:
		return "m/s^2";
	case MGal:
		return "mGal";
	}
	return "GRAV";
}

Units::DistanceUnit Units::displaydistanceunit = M;//initializer for the static data member
void Units::setDistanceDisplayUnit(DistanceUnit u) {
	displaydistanceunit = u;
}
double Units::siToDisplayDistance(double sidistancevalue) {
	switch(displaydistanceunit) {
	case M:
		return sidistancevalue;
	case Ft:
		return sidistancevalue*3.280839895013123; //get feet from meters, divide by 0.3048
	}
	return sidistancevalue;
}
double Units::displayToSiDistance(double displaydistancevalue) {
	switch(displaydistanceunit) {
	case M:
		return displaydistancevalue;
	case Ft:
		return displaydistancevalue*0.3048; //To get meters from feet, multiply by 0.3048 exactly
	}
	return displaydistancevalue;
}
QString Units::distanceSuffix() {
	switch(displaydistanceunit) {
	case M:
		return "m";
	case Ft:
		return "ft";
	}
	return "DISTANCE";
}

Units::DensityUnit Units::displaydensityunit = Kgm3;//initializer for the static data member
void Units::setDensityDisplayUnit(DensityUnit u) {
	displaydensityunit = u;
}
double Units::siToDisplayDensity(double sidensityvalue) {
	switch(displaydensityunit) {
	case Kgm3:
		return sidensityvalue;
	case Gcm3:
		return sidensityvalue*0.001; //To get g/cm^3 from kg/m^3, multiply by 0.001 (divide by 1,000)
	}
	return sidensityvalue;
}
double Units::displayToSiDensity(double displaydensityvalue) {
	switch(displaydensityunit) {
	case Kgm3:
		return displaydensityvalue;
	case Gcm3:
		return displaydensityvalue*1000.0; //To get kg/m^3 from g/cm^3, multiply by 1,000
	}
	return displaydensityvalue;
}
QString Units::densitySuffix() {
	switch(displaydensityunit) {
	case Kgm3:
		return "kg/m^3";
	case Gcm3:
		return "g/cm^3";
	}
	return "DENSITY";
}

Units::MassUnit Units::displaymassunit = Kg;//initializer for the static data member
void Units::setMassDisplayUnit(MassUnit u) {
	displaymassunit = u;
}
double Units::siToDisplayMass(double simassvalue) {
	switch(displaymassunit) {
	case Kg:
		return simassvalue;
	case Lb:
		return simassvalue*2.2046226218488; //To get lbs from kgs divide by 0.45359237 exactly. (multiply by 2.2046226218488)
	}
	return simassvalue;
}
double Units::displayToSiMass(double displaymassvalue) {
	switch(displaymassunit) {
	case Kg:
		return displaymassvalue;
	case Lb:
		return displaymassvalue*0.45359237; //To get kg from lb multiply by 0.45359237 exactly
	}
	return displaymassvalue;
}
QString Units::massSuffix() {
	switch(displaymassunit) {
	case Kg:
		return "kg";
	case Lb:
		return "lb";
	}
	return "mass";
}