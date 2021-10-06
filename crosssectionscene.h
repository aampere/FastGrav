#ifndef CROSSSECTIONSCENE_H
#define CROSSSECTIONSCENE_H
#include <QWidget>
class CrossSection;
class MainWindow;
class PolygonPrismItem;

class CrossSectionScene : public QWidget
{
	Q_OBJECT

	CrossSection* crosssection;
	MainWindow* mainwindow;

	int handlesize;

	bool uiflag; //true if ui elements should be shown. Basically, uiflag is true always except for when the user uses the "Save..." action. When saving the image, handles, centers, names, and vertices will not be show;

public:
	CrossSectionScene(QWidget* parent, CrossSection* cs, MainWindow* mw);
	
	int closestVertex(QPoint p, PolygonPrismItem*& polygonprismitem, int& vip); //Finds ONLY the closest vertex, and sets polygonprismitem and vip by reference, which together define the closes vertex. The distance from the cursor to that vertex is returned.
	std::vector<int> checkForVertex(QPoint p, std::vector<PolygonPrismItem*>& ppis, std::vector<int>& vip); //This function fill the list of ppis by reference, and a parallel list of vip's (the index of the grabbed vertex), and returns a parallel list of distances from cursor to the vertex defined by the ppis[i] and vip[i].
	std::vector<int> checkForEdge(QPoint p, std::vector<PolygonPrismItem*>& ppis, std::vector<int>& vi1); //This function fills the list of ppis by reference, and a parallel list of v1's (the edge's first vertex), and returns a parallel list of distances from cursor to the edge defined by ppis[i] and vi1[i]

	void hoverCheck(QPoint p);
	QPair<int,int> handleCheck(QPoint p);

	void setUiFlag(bool b);

protected:
	QPoint lastpos;
	QVector<QRectF> rects;
	QVector<bool> hover;
	QVector<bool> visible;
	QVector<int> handles;	//each element corresponds to a massitem.
							//int==0, then no handle is hovered for that item.
							//int==1, movehandle is hovered.
							//int==2, scalehandle is hovered.
	int dragginghandleindex;//index of the ***MassItem*** that owns the handle that is being dragged.
	int dragginghandletype; //0 if no handle is begin dragged. 1, 2, etc to indicate the type of handle that is being dragged, if there is a handle being dragged.
	bool handlehover; //if any handle is hovered over, this is true. If no handle is hovered over, this is false.

	bool scrolling;

	bool mousepressed;
	bool vertexgrabbed;
	PolygonPrismItem* grabbeditem;	//taken together, grabbeditem and grabbedvindex point to a specific vertex of a specfic item. mouseMoveEvent() can use these to move the real vertices of the PolygonPrismItem.
	int grabbedvindex;				//

	void paintEvent(QPaintEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

};
#endif