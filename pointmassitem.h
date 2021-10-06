#ifndef POINTMASSITEM_H
#define POINTMASSITEM_H

#include "massitem.h"

class PointMassItem : public MassItem
{
	Q_OBJECT


public:
	PointMassItem();
	~PointMassItem();

	double getGravityEffect(double point);
};

#endif // POINTMASSITEM_H
