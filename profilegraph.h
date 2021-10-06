#ifndef PROFILEGRAPH_H
#define PROFILEGRAPH_H

#include <QWidget>
#include <QImage>
#include <QPainter>
class ObservationSet;

class ProfileGraph : public QWidget
{
	Q_OBJECT

	std::vector<double> xmod; //forward-modeled data
	std::vector<double> ymod; //forward-modeled data
	std::vector<double> yemp; //emperical data
	double xmin; //in value units
	double xmax;
	double ymin;
	double ymax;

    ObservationSet* observationset;
    QPoint mousepos;

	int leftmargin, rightmargin, bottommargin, topmargin; //in pixels

	double xtickinterval; //in value units
	double ytick0;
	double ytickinterval; //in value units
	//There is guaranteed to be a tick at ytick0, and at multiples of ytickinterval away from ytick0

	QImage infobox;

public:
	ProfileGraph(QWidget *parent = 0);
	~ProfileGraph();
	void setObservationSet(ObservationSet* os);
	void mouseMoveEvent(QMouseEvent * event);

	void setEmpiricalPoints(std::vector<double> ny);
	void setModeledPoints(std::vector<double> nx, std::vector<double> ny);
	QList<QPointF> getEmpiricalGraphPoints();
	QList<QPointF> getModeledGraphPoints();
	double closestEmpiricalPoint(QPoint p, int& ei);
	double closestModeledPoint(QPoint p, int& mi);

	void fillInfoBox(QPoint p);

	void setXMinMax(double a, double b);
	void setYMinMax(double a, double b);

	void setYTick0(double d);
	void setYTickInterval(double d);

	double getXTickInterval();

protected:
	void paintEvent(QPaintEvent *event);

	QVector<QLineF> linesFromPoints(std::vector<double> nx, std::vector<double> ny);
	QPointF getGraphPoint(double nx, double ny);
	QPointF getRealXYFromGraphPoint(QPoint p);

};
#endif
