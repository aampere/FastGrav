#ifndef MASSITEM_H
#define MASSITEM_H

#include <QObject>
#include <QListWidget>
#include <cmath>
#include <qstring.h>
#include <QColor>
#include "point.h"
class MainWindow;

class MassItem
{
		
	MainWindow* mainwindow;

protected:
	const double constG;
	const double pi;
	QString name;
	double density;
	bool active;
	bool visible;
	QColor color;
	QString type; //String that contains the name of the specific type of MassItem, e.g., "SphereItem"

	QListWidgetItem* listwidgetitem;

public:
	MassItem(MainWindow* mw);
	~MassItem();
	virtual MassItem* clone()=0;//pure virtual clone function that allows derived masses to be cloned.

	virtual double getGravityEffect(Point p)=0; //pure virtual function; There will never be a non-specific MassItem, and each specific type of MassItem will have its own way to calculate Gravity Effect.
	virtual Point getCenter()=0;

	void effectChanged();

	void setActive(bool a);
	bool isActive();
	void setVisible(bool v);
	bool isVisible();
	void setColor(QColor col);
	QColor getColor();

	QString getName();
	double getDensity();
	QString getType();
	void setName(QString);
	void setDensity(double d);

	
	QString getFullLabel();
	QListWidgetItem* listWidgetItem();
	void setListWidgetItem(QListWidgetItem* lwi);
};

#endif // MASSITEM_H
