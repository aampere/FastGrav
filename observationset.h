#ifndef OBSERVATIONSET_H
#define OBSERVATIONSET_H

#include <QObject>
#include <QListWidget>
#include <qstring.h>
#include "ui_observationsetunitsui.h"
#include "point.h"
#include "massitem.h"
#include "units.h"
#include "observationsettable.h"
#include <qmdisubwindow.h>
class MainWindow;

class ObservationSet : public QObject
{
	Q_OBJECT

	MainWindow* mainwindow;
	ObservationSetTable* table;
	QMdiSubWindow* tablewindow;

	QString name; 
	QString sourcefile;
	QListWidgetItem* listwidgetitem;
	bool hasunits;
	bool valid;											//A bad source (or a bad parse?) would cause this observation set to be invalid

	std::vector<Point> points;							//Points should not change position in this vector. If the points vector is replaced, gravity effects should be recalculated.
	std::vector<double> empgravityvals;					//Empirically observed gravity values, in m/s^2. These values come from the user, as in, produced from a gravity meter, or authored arbitrarily.
	//std::vector<MassItem*> massitems;					//Pointer to MainWindow's massitems vector. If a MassItem* is removed from MainWindow's vector, then that MassItem's gravity effect should be removed from the gravityeffects vector, which will preserve everything's order.
	std::vector< std::vector<double> > gravityeffects;	//ex.: gravityeffects[1][7] refers to Point 1 and MassItem* 7


public:
	ObservationSet(std::vector<Point> pts, MainWindow* mw); //not used anywhere, currently
	ObservationSet(QString fname, MainWindow* mw); //used when user creates an observation set through the UI
	ObservationSet(QString fname, Units::GravityUnit gravu, Units::DistanceUnit distu, MainWindow* mw); //used when a project file creates an observation set
	void initialize(QString fname, Units::GravityUnit gravu, Units::DistanceUnit distu, MainWindow* mw);
	~ObservationSet();

	bool hasUnits();
	bool isValid();

	void setTable(ObservationSetTable* ost, QMdiSubWindow* sub);
	void closeTable();
	ObservationSetTable* getTable();
	void updateTable();

	void calculateGravityEffects();
	void calculateGravityEffect(int index);			//massitem[index] has changed, and its effect needs to be recalculated for all points.
	double getSumGravityEffect(int index);			//For point[index], get the sum of all massitems' effects on that point.
	double getOffsetGravityEffect(int index);		//Simply adds the global gravity offset to the SumGravityEffect( )
	void removeMassItemGravityEffect(int index);	//if a MassItem is removed (aka deleted) from the mainwindow, its corresponding gravity effect must be deleted from the cache.
	void addMassItemGravityEffect(int index);		//if a MassItem is added to the mainwindow, space must be made for its effect in the gravityeffects cache. This will be common.

	QString getName();
	std::vector<Point> getPoints();
	std::vector<double> getEmpGravityVals();
	QString getSourceFile();
	int getSize();
	double getAzimuth();							//The azimuth of the vector pointing from the first point to the last. Returned in degrees. Calculated on the fly.
	void setName(QString n);

	void initializeFromFile(QString fname);
	
	QListWidgetItem* listWidgetItem();
	void setListWidgetItem(QListWidgetItem* lwi);

	Ui::ObservationSetUnitsUIClass unitsui;
	Units::DistanceUnit distanceu;
	Units::GravityUnit gravityu;
};

#endif // OBSERVATIONSET_H
