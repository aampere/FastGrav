#ifndef CROSSSECTION_H
#define CROSSSECTION_H
#include "ui_crosssectionui.h"
#include "crosssectionscene.h"
#include "yaxisui.h"
#include <QSignalMapper>
#include <QMdiSubWindow>
class MainWindow;
class MassItem;
class ObservationSet;
class CrossSectionItem;

class CrossSection : public QWidget
{
	Q_OBJECT

	MainWindow* mainwindow;
	ObservationSet* observationset;
	QMdiSubWindow* window;

	bool automaticcrossbounds; //If the cross section bounds are automatic, then x1, x2, y1, y2 are pegged to the first and last points in the observationset.
	double x1, y1, x2, y2;	//Boundary points for the cross section. (x1,y1) corresponds to the left edge of both graphs, (x2,y2) corresponds to the right side of both graphs.
	double ylow, yhigh;		//Y-axis bounds for the profile graph.
	double ytick0, ytickinterval;
	bool automaticyaxis;	//Are the range and tick parameters automatically calculated?
	double xmin, xmax;		//low/high for the projected x-axis (x-axis in graph, as well as the horizontal direction in the cross section)
	int leftmargin, rightmargin, bottommargin, topmargin; //in pixels
	double realzoffset; //The observation set may not be right at or around 0.0m elevation. This value is the real z elevation in meters that is at the horizontal topmargin line.

	double zval; 
	QPixmap leftaxispixmap;
	QPixmap observationpointspixmap;

	QAction* savecrosssection;
	
	std::vector<PolygonPrismItem*> vertexitems;
	std::vector<int> vertexvis;
	std::vector<PolygonPrismItem*> edgeitems;
	std::vector<int> edgevis;
	//QAction* addvertexhere;
	//QSignalMapper* mapper;
	//////////////////////////////////////////////////////////////////////
	CrossSectionScene scene;

	int selectedeindex; //selected empirical index. The index of the empirical point that was right-clicked.
	QAction* anchorhere;
	QAction* saveprofile;

	QPoint menupoint;
public:
	CrossSection(MainWindow* mw, QWidget *parent=0);
	~CrossSection();
	void setWindow(QMdiSubWindow* win);
	QMdiSubWindow* getWindow();
	void closeWindow();
	QByteArray saveSplitter();
	void restoreSplitter(QByteArray state);
	void syncUI();

	void clearObservationSet(ObservationSet* os);
	ObservationSet* getObservationSet();
	int getObservationSetIndex();

	double projectOntoCrossSection(double px, double py);
    std::vector<double> projectObservationSet();
	void updateCrossSection();
	void pushUnitUpdate();

	void updateObservationSetCombo();
	void automaticCrossBoundaries();	//sets x1,y1,x2,y2 automatically once
	void automaticYBoundaries();		//sets y-axis bounds automatically once
	QList<double> getMargins(); //leftmargin, rightmargin, bottommargin, topmargin
	bool getAutomaticCrossBounds();
	double getX1();
	double getX2();
	double getY1();
	double getY2();
	double getYLow();
	double getYHigh();
	double getYTick0();
	double getYTickInterval();
	bool isYAxisAutomatic();
	double getXMin();
	double getXMax();
	double getRealZOffset();
	double getZTickInterval();

	void setAutomaticCrossBounds(bool b);
	void setYLow(double d);
	void setYHigh(double d);
	void setYTick0(double d);
	void setYTickInterval(double d);
	void setYAxisAutomatic(bool b);
	void setXMin(double d);
	void setXMax(double d);
	void setRealZOffset(double rzo);

	void scrollRealZOffset(int px);

	//void addCrossSectionItem(int index);
	//void removeCrossSectionItem(int index);
	//void initializeCrossSectionItems();		//When the CrossSection is created, a cross section item for each extant MassItem* needs to be created.
	//MassItem* getMassItemFromCrossSectionItem(CrossSectionItem* ci);
	void updateMassItemFromPosition(int i, QPointF pt);
	void updateMassItemFromRadius(int i, double r);
	void updateVertexFromPostion(PolygonPrismItem* ppi, int vindex, QPointF pt);
	QPointF getCrossSectionPoint(double realx, double realz);
	std::pair<double, double> getRealXZfromCrossSectionPoint(QPointF pt);
	double pixelToReal(double d);//pixel input, returns Realworld distance.
	double realToPixel(double d);//real-world distancce input, returns pixel distance.
	QVector<QLineF> linesFromPoints(std::vector<double> nx, std::vector<double> ny);

	//std::vector<CrossSectionItem*> crossSectionItems();

public Q_SLOTS:
    void setObservationSet(int i);
	void setX1(double d);
	void setY1(double d);
	void setX2(double d);
	void setY2(double d);
	//void setYLowFromLineEdit();
	//void setYHighFromLineEdit();
	void automaticCheck();
	
	//void updateCrossSectionItems();			//recalculate the position, size, color, etc of all cross section items, and update the items to reflect the new properties.

	void customCrossSectionMenuSlot(const QPoint& p);
	
	void sendThisVertexLeftSlot(int i);
	void sendThisVertexRightSlot(int i);
	void removeThisVertexSlot(int i);
	void addVertexHereSlot(int i);
	
	void saveCrossSectionImage();

	void customProfileMenuSlot(const QPoint& p);
	void anchorProfileHere();
	void saveProfileImage();

	void openYAxisDialog();

private:
	Ui::CrossSectionClass ui;
};

#endif // CROSSSECTION_H
