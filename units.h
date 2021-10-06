#ifndef UNITS_H
#define UNITS_H

#include <QString>
//This class is intended to be wholly static.
//It maintains the display units (i.e., what units the user has chose to view their project in)
//and provides functions to convert between SI units and display units.
//In general, where values are stored in this program, *they are stored in SI units*, and converted to display units at the last second for display.
//Conversely, when the user enters a united value, it is immediately converted to SI units for storage.

class Units {
	
public:
    enum UnitClass{Gravity, Distance, Density, Mass}; //Not currently used anywhere. Intended as a simple list of unit types.

    enum GravityUnit{Ms2, MGal}; //  m/s^2, mGal
	static GravityUnit displaygravunit;
	static void setGravityDisplayUnit(GravityUnit u);
	static double siToDisplayGrav(double sigravityvalue);
	static double displayToSiGrav(double displaygravityvalue);
	static QString gravSuffix();
	
    enum DistanceUnit{M, Ft}; // Meters, Feet
	static DistanceUnit displaydistanceunit;
	static void setDistanceDisplayUnit(DistanceUnit u);
	static double siToDisplayDistance(double sidistancevalue);
	static double displayToSiDistance(double displaydistancevalue);
	static QString distanceSuffix();

    enum DensityUnit{Kgm3, Gcm3}; // kg/m^3, g/cm^3
	static DensityUnit displaydensityunit;
	static void setDensityDisplayUnit(DensityUnit u);
	static double siToDisplayDensity(double sidensityvalue);
	static double displayToSiDensity(double displaydensityvalue);
	static QString densitySuffix();

    enum MassUnit{Kg, Lb}; // kg, lb
	static MassUnit displaymassunit;
	static void setMassDisplayUnit(MassUnit u);
	static double siToDisplayMass(double simassvalue);
	static double displayToSiMass(double displaymassvalue);
	static QString massSuffix();
};
#endif
