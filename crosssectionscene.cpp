#include "crosssectionscene.h"
#include "crosssection.h"
#include "mainwindow.h"
#include <QMouseEvent>
#include <QtDebug>
#include <cmath>

CrossSectionScene::CrossSectionScene(QWidget* parent, CrossSection* cs, MainWindow* mw)
: QWidget(parent), crosssection(cs), mainwindow(mw),handlesize(14), uiflag(true),
   scrolling(false), mousepressed(false), vertexgrabbed(false)
{
	setMouseTracking(true);
	hoverCheck(QPoint(0,0));
	repaint();
}

void CrossSectionScene::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.fillRect(0,0,width(),height(),Qt::white);
	QFont font = painter.font();
	font.setPixelSize(12);
	painter.setFont(font);
	QFontMetrics fm = painter.fontMetrics();
	QPen normalpen(QBrush(Qt::black), 1);
	painter.setPen(normalpen);

	if(crosssection && mainwindow) {
		rects.resize(mainwindow->massItems().size());
		auto margins = crosssection->getMargins();
		double leftmargin = margins[0];
		double rightmargin = margins[1];
        //double bottommargin = margins[2];
        //double topmargin = margins[3];


		ObservationSet* observationset = crosssection->getObservationSet();
		if(observationset) {
            std::vector<double> xs = crosssection->projectObservationSet();
			std::vector<double> zs;
			std::vector<Point> pts = observationset->getPoints();
			foreach(Point pt, pts) zs.push_back(pt.z); 
			painter.drawLines(crosssection->linesFromPoints(xs,zs));
            for(uint i=0; i!=xs.size(); ++i) {
				QPointF center(crosssection->getCrossSectionPoint(xs[i], zs[i]));
				painter.fillRect(QRect(center.x()-1, center.y()-1, 3, 3), Qt::black);
			}
		}

		auto massitems = mainwindow->massItems();
		int index = massitems.size()-1;//the int index of the current massitem. Start from the last item and work towards the first
		for(auto mi=massitems.end()-1; mi!=massitems.begin()-1; --mi) { //Start from the last item and work towards the first
			if((*mi)->isVisible()) {
				if(SphereItem* si = dynamic_cast<SphereItem*>(*mi)) {
					Point realcenter=si->getCenter();
					double pixelradius = crosssection->realToPixel(si->getRadius());
					painter.setBrush(si->getColor());
					QRectF rect = QRectF(crosssection->getCrossSectionPoint(crosssection->projectOntoCrossSection(realcenter.x, realcenter.y), realcenter.z),QSizeF(pixelradius*2.0, pixelradius*2.0)).translated(-pixelradius, -pixelradius);
					painter.drawEllipse(rect);
					rects[index] = rect; //no translation needed here. this rect is already in absolute coordinates, not relative to the massitem center, as the others are.
					if(uiflag && index<hover.size() && hover[index]) {
						QPointF rightp(rects[index].right(),rects[index].center().y());
						QImage scalearrows(":/scalearrows");
						painter.drawImage(rightp.x()-handlesize*0.5,rightp.y()-handlesize*0.5,scalearrows);
					}
				} else if(CylinderItem* ci = dynamic_cast<CylinderItem*>(*mi)) {
					Point realcenter = ci->getCenter();
					double pixelradius = crosssection->realToPixel(ci->getRadius());
					double azimuthrad = ci->getAzimuthrad() + atan2(crosssection->getY2()-crosssection->getY1(), crosssection->getX2()-crosssection->getX1());
                    double cylinderskewradius = std::abs(cos(azimuthrad)*pixelradius);
                    double cylinderskewlength = std::abs(sin(azimuthrad)*2.0*pixelradius);
					double az = fmod(azimuthrad, 3.14159);
					if(az<0) az+= 3.14159;
					QPointF center(crosssection->getCrossSectionPoint(crosssection->projectOntoCrossSection(realcenter.x, realcenter.y), realcenter.z));
					painter.translate(center);
					painter.setBrush(ci->getColor());
					if(az<=3.14159*.5) {
						painter.drawEllipse(QRectF(QPointF(0.5*cylinderskewlength-cylinderskewradius, -pixelradius), QSize(2.0*cylinderskewradius, 2.0*pixelradius)));//right face
						painter.setPen(Qt::NoPen);
						painter.drawRect(-cylinderskewlength*0.5, -pixelradius, cylinderskewlength, 2.0*pixelradius+1);
						painter.setPen(Qt::black);
						painter.drawLine(-cylinderskewlength*0.5, -pixelradius-1, cylinderskewlength*0.5, -pixelradius-1);
						painter.drawLine(-cylinderskewlength*0.5, pixelradius, cylinderskewlength*0.5, pixelradius);
						painter.drawEllipse(QRectF(QPointF(-cylinderskewradius-0.5*cylinderskewlength, -pixelradius), QSize(2.0*cylinderskewradius, 2.0*pixelradius)));//left face on top
					} else {
						painter.drawEllipse(QRectF(QPointF(-cylinderskewradius-0.5*cylinderskewlength, -pixelradius), QSize(2.0*cylinderskewradius, 2.0*pixelradius)));//left face
						painter.setPen(Qt::NoPen);
						painter.drawRect(-cylinderskewlength*0.5, -pixelradius, cylinderskewlength, 2.0*pixelradius+1);
						painter.setPen(Qt::black);
						painter.drawLine(-cylinderskewlength*0.5, -pixelradius-1, cylinderskewlength*0.5, -pixelradius-1);
						painter.drawLine(-cylinderskewlength*0.5, pixelradius, cylinderskewlength*0.5, pixelradius);
						painter.drawEllipse(QRectF(QPointF(0.5*cylinderskewlength-cylinderskewradius, -pixelradius), QSize(2.0*cylinderskewradius, 2.0*pixelradius)));//right on top
					}
					painter.resetTransform();
					rects[index] = QRectF(-cylinderskewradius-0.5*cylinderskewlength, -pixelradius, cylinderskewlength+2.0*cylinderskewradius, 2.0*pixelradius).translated(center);
					if(uiflag && index<hover.size() && hover[index]) {
						//QPointF rightp(rects[index].right(),rects[index].center().y());
						QPointF rightp(rects[index].center().x()+pixelradius,rects[index].center().y());
						QImage scalearrows(":/scalearrows");
						painter.drawImage(rightp.x()-handlesize*0.5,rightp.y()-handlesize*0.5,scalearrows);
					}
                } else if(CloudItem* cli = dynamic_cast<CloudItem*>(*mi)) {
					std::vector<QPointF> pts;
					auto realpts = cli->getPoints();
					int xmin = 1000000;
					int xmax =-1000000;
					int ymin = 1000000;
					int ymax =-1000000;
					for(auto realpt=realpts.begin(); realpt!=realpts.end(); ++realpt) {
						int px = crosssection->realToPixel(crosssection->projectOntoCrossSection(realpt->x+crosssection->getX1(), realpt->y+crosssection->getY1()));
						int py = crosssection->realToPixel(-realpt->z);
						pts.push_back(QPointF(px, py));
						xmin = std::min(xmin,px);
						xmax = std::max(xmax,px);
						ymin = std::min(ymin,py);
						ymax = std::max(ymax,py);
					}
					painter.setBrush(cli->getColor());
					QPointF center = crosssection->getCrossSectionPoint(crosssection->projectOntoCrossSection(cli->getCenter().x, cli->getCenter().y), cli->getCenter().z);
					painter.translate(center);
					for(auto pt=pts.begin(); pt!=pts.end(); ++pt) {
						painter.drawEllipse(*pt,2,2);
					}
					painter.resetTransform();
					rects[index] = QRectF(QPointF(xmin, ymin), QPointF(xmax, ymax)).translated(center);
                } else if(PolygonPrismItem* ppi = dynamic_cast<PolygonPrismItem*>(*mi)) {
					double azimuthrad = ppi->getAzimuthrad() + atan2(crosssection->getY2()-crosssection->getY1(), crosssection->getX2()-crosssection->getX1());		
					QVector<QPoint> pts;
					auto realpts = ppi->getVertices();
					int xmin = 1000000;
					int xmax =-1000000;
					int ymin = 1000000;
					int ymax =-1000000;
					for(auto realpt=realpts.begin(); realpt!=realpts.end(); ++realpt) {
						int px = crosssection->realToPixel(crosssection->projectOntoCrossSection(realpt->x*cos(ppi->getAzimuthrad())+crosssection->getX1(), -realpt->x*sin(ppi->getAzimuthrad())+crosssection->getY1()));
						int py = crosssection->realToPixel(-realpt->y);
						xmin = std::min(xmin,px);
						xmax = std::max(xmax,px);
						ymin = std::min(ymin,py);
						ymax = std::max(ymax,py);
						pts.push_back(QPoint(px, py));
					}
                    double polygonprismskewlength = std::abs(sin(azimuthrad)*2.0*std::max(xmax-xmin,ymax-ymin));
					double az = fmod(azimuthrad, 3.14159);
					QPolygon poly(pts);
					QPointF center(crosssection->getCrossSectionPoint(crosssection->projectOntoCrossSection(ppi->getCenter().x, ppi->getCenter().y), ppi->getCenter().z));
					painter.translate(center);
					painter.setBrush(ppi->getColor());
					if(az<0) az+= 3.14159;
					if(az<=3.14159*.5) {
						painter.drawPolygon(poly.translated(QPoint(+0.5*polygonprismskewlength, 0)));
						painter.drawPolygon(poly.translated(QPoint(-0.5*polygonprismskewlength, 0)));
					} else {
						painter.drawPolygon(poly.translated(QPoint(-0.5*polygonprismskewlength, 0)));
						painter.drawPolygon(poly.translated(QPoint(+0.5*polygonprismskewlength, 0)));
					}
					if(uiflag && az<0.01 && az>-0.01) {
						painter.setBrush(Qt::NoBrush);
						for(int i=0; i!=pts.size(); ++i) {
							painter.drawRect(pts[i].x()-1, pts[i].y()-1,3,3);
						}
					}
					painter.resetTransform();
					rects[index] = QRectF(-polygonprismskewlength*0.5+xmin,ymin,polygonprismskewlength+xmax-xmin, ymax-ymin).translated(center);
				}

				//Below area is for graphics that are drawn on every mass item, regardless of type. Label, handles, etc.
				Point realcenter=(*mi)->getCenter();
				QPointF centerpoint = crosssection->getCrossSectionPoint(crosssection->projectOntoCrossSection(realcenter.x, realcenter.y), realcenter.z); //the true center of the massitem, projected.
				QPointF middlepoint = rects[index].center(); //the geometric/graphical center of the rectangle. NOT the true center of the mass item
				
				if(uiflag && index<hover.size() && hover[index]) {
					painter.drawLine(centerpoint+QPointF(0,-4), centerpoint+QPointF(0,-1));//Draw the true center cross
					painter.drawLine(centerpoint+QPointF(0,4), centerpoint+QPointF(0,1));//
					painter.drawLine(centerpoint+QPointF(-4,0), centerpoint+QPointF(-1,0));//Draw the true center cross
					painter.drawLine(centerpoint+QPointF(4,0), centerpoint+QPointF(1,0));//
					font.setBold(true);
					painter.setFont(font);
					QImage movearrows(":/movearrows");
					painter.drawImage(middlepoint.x()-handlesize*0.5,middlepoint.y()-handlesize*0.5,movearrows);
				}
				if(uiflag) painter.drawText(middlepoint+QPointF(3,5)+QPointF(0,painter.fontMetrics().height()*0.7), (*mi)->getName());
				font.setBold(false);
				painter.setFont(font);
			}
			--index;
		}
		//Below: Draw gray edges and ticks
		painter.fillRect(0,0,leftmargin, 700, QColor(240,240,250));
		for(int i=0; i<=12; ++i) { //for now just draw 12 ticks. silly.
			int shiftedi = i-crosssection->getRealZOffset()/crosssection->getZTickInterval()-1; // i, shifted to account for realzoffset. So that ticks above z=0.0m are drawn.
			QPointF first = crosssection->getCrossSectionPoint(crosssection->getXMin(), -crosssection->getZTickInterval()*shiftedi);
			painter.drawLine(first, QPointF(first.x()-4, first.y()));
			QString tag = QString::number(Units::siToDisplayDistance(-crosssection->getZTickInterval()*shiftedi));
			int tagw = fm.width(tag);
			painter.drawText(QRect(first.x()-tagw-6, first.y()-fm.height()/2-1, tagw, fm.height()),Qt::AlignRight, tag);
		}
		painter.fillRect(width()-rightmargin, 0, rightmargin, 700, QColor(240,240,250));
		//
	}
}
void CrossSectionScene::hoverCheck(QPoint p) {
	hover.resize(rects.size());
	bool redraw = false; //switch this to true and redraw is any hover-state changes.
	for(int i=0; i!=hover.size(); ++i) {
		bool oldhover = hover[i];
		hover[i] = rects[i].adjusted(-handlesize/2,-handlesize/2,handlesize/2,handlesize/2).contains(p);
		redraw = redraw || (hover[i]!=oldhover);
	}
	if(redraw) {
		repaint();
	}
}
QPair<int,int> CrossSectionScene::handleCheck(QPoint p) {
	QPair<int,int> besthandle(-1,0); //this pair indicates which handle would be grabbed if the user clicked. the pair corresponds to dragginghandleindex,dragginghandletype
	handles.resize(rects.size());
	handles.fill(0);//reset all the handle states.
	auto massitems = mainwindow->massItems();
	handlehover = false;
	for(int i=rects.size()-1; i!=-1; --i) { //go through the items backwards, in "layer order"
		if(massitems[i]->isVisible()) {
			double pixelradius;
			if(SphereItem* si = dynamic_cast<SphereItem*>(massitems[i])) {
				pixelradius = crosssection->realToPixel(si->getRadius());
			} else if(CylinderItem* ci = dynamic_cast<CylinderItem*>(massitems[i])) {
				pixelradius = crosssection->realToPixel(ci->getRadius());
			} else {
				pixelradius = rects[i].width()*0.5;
			}
			QPointF middlep(rects[i].center());
			QPointF rightp(rects[i].center().x()+pixelradius,middlep.y());
			QRectF middle(middlep+QPointF(-handlesize*0.5,-handlesize*0.5),QSizeF(handlesize, handlesize));
			QRectF right(rightp+QPointF(-handlesize*0.5,-handlesize*0.5),QSizeF(handlesize, handlesize));
			if(middle.contains(p)) {
				handles[i] = 1;
			} else if(right.contains(p) && (massitems[i]->getType()=="SphereItem" || massitems[i]->getType()=="CylinderItem")) {
				handles[i] = 2;
			}
			if(handles[i]!=0) {
				besthandle.first = i;
				besthandle.second = handles[i];
				handlehover = true;
			}
		}
	}
	return besthandle;
}

void CrossSectionScene::mouseMoveEvent(QMouseEvent* event) {
	QWidget::mouseMoveEvent(event);
	setUiFlag(true);
	if(mousepressed && vertexgrabbed) {
		//move the grabbed vertex appropriately, causing live updates.
		crosssection->updateVertexFromPostion(grabbeditem, grabbedvindex, event->pos());
	} else if(mousepressed && dragginghandleindex!=-1 && dragginghandletype==1) {
		QPointF centerpoint = crosssection->getCrossSectionPoint(crosssection->projectOntoCrossSection(mainwindow->massItems()[dragginghandleindex]->getCenter().x, mainwindow->massItems()[dragginghandleindex]->getCenter().y), mainwindow->massItems()[dragginghandleindex]->getCenter().z); //the true center of the massitem, projected.
		QPointF middlepoint = rects[dragginghandleindex].center(); //the geometric/graphical center of the rectangle. NOT the true center of the mass item
		QPointF middlecenteroffset = centerpoint-middlepoint; //The item is being dragged by it's middle handle, which may not be at the same location and the item's true "center"
		crosssection->updateMassItemFromPosition(dragginghandleindex, event->pos()+middlecenteroffset);
	} else if(mousepressed && dragginghandleindex!=-1 && dragginghandletype==2) {
		//QPointF centerpoint = crosssection->getCrossSectionPoint(crosssection->projectOntoCrossSection(mainwindow->massItems()[dragginghandleindex]->getCenter().x, mainwindow->massItems()[dragginghandleindex]->getCenter().y), mainwindow->massItems()[dragginghandleindex]->getCenter().z); //the true center of the massitem, projected.
		QPointF middlepoint = rects[dragginghandleindex].center(); //the geometric/graphical center of the rectangle. NOT the true center of the mass item
		//QPointF middlecenteroffset = centerpoint-middlepoint; //The item is being dragged by it's middle handle, which may not be at the same location and the item's true "center"

		crosssection->updateMassItemFromRadius(dragginghandleindex, event->pos().x()-middlepoint.x());
	}
	
	if(scrolling && crosssection) {
		int ydif = event->pos().y() - lastpos.y();
		crosssection->scrollRealZOffset(ydif);
	}
	bool originalhandlehover = handlehover;
	handleCheck(event->pos()); //updates handlehover for the new position
	
	if( (event->pos().x()<crosssection->getMargins()[0] && lastpos.x()>=crosssection->getMargins()[0] && !mousepressed) 
		|| (originalhandlehover==false && handlehover==true && !mousepressed) || (handlehover && !mousepressed) ) { //getMargins()[0] is the leftmargin
		setCursor(Qt::OpenHandCursor);
	} else if( (event->pos().x()>=crosssection->getMargins()[0] && lastpos.x()<crosssection->getMargins()[0] && !mousepressed) 
		|| (originalhandlehover==true && handlehover==false && !mousepressed) ) { //getMargins()[0] is the leftmargin
		setCursor(Qt::ArrowCursor);
	} else {

	}
	lastpos = event->pos();
	hoverCheck(event->pos());
}
void CrossSectionScene::mousePressEvent(QMouseEvent* event) {
	QWidget::mousePressEvent(event);

	mousepressed = true;
	QPair<int,int> besthandle = handleCheck(event->pos()); //note than handleCheck also repopulates the handles Vector. And sets handlehover.
	PolygonPrismItem* closestitem = nullptr;
	int vindex;
	if(besthandle.first!=-1 && besthandle.second!=0){
		dragginghandleindex = besthandle.first;
		dragginghandletype = besthandle.second;
	} else if(closestVertex(event->pos(), closestitem, vindex) < 7.0) {
		vertexgrabbed = true;
		grabbeditem = closestitem;
		grabbedvindex = vindex;
	} else if(event->pos().x()<crosssection->getMargins()[0]) { //getMargins()[0] is the leftmargin
		scrolling = true;
	}
	if(handlehover 
		|| event->pos().x()<crosssection->getMargins()[0] ) {
		setCursor(Qt::ClosedHandCursor);
	}
}
void CrossSectionScene::mouseReleaseEvent(QMouseEvent* event) {
	QWidget::mouseReleaseEvent(event);
	mousepressed = false;
	dragginghandleindex = -1; //no item has an active handle
	dragginghandletype = 0; //redundant, but clear: handletype of 0 indicates no active handle.
	vertexgrabbed = false;
	scrolling = false;
	if(handlehover || event->pos().x()<crosssection->getMargins()[0]) {
		setCursor(Qt::OpenHandCursor);
	} else {
		setCursor(Qt::ArrowCursor);
	}
}

int CrossSectionScene::closestVertex(QPoint p, PolygonPrismItem*& polygonprismitem, int& vip) {
	//Code duplication: closestVertex( ), checkForVertex( ) and checkForEdge( ) all share code/concept. See header for description of each.
	int closestmousedistance = 10000;
	PolygonPrismItem* closestitem = nullptr; //Actually, this is the item that owns the closest vertex
	int closestvindex = -1; //This is the index of a vertex in closestitem
	auto massitems = mainwindow->massItems();
	for(auto mi=massitems.begin(); mi!=massitems.end(); ++mi) {
		if(PolygonPrismItem* ppi = dynamic_cast<PolygonPrismItem*>(*mi)) {
			double az = fmod(ppi->getAzimuthrad() + atan2(crosssection->getY2()-crosssection->getY1(), crosssection->getX2()-crosssection->getX1()), 3.14159);
			if(az>-0.01 && az<0.01 && ppi->isVisible()) {
				auto vertices = ppi->getVertices();
				QPointF center = crosssection->getCrossSectionPoint(crosssection->projectOntoCrossSection(ppi->getCenter().x, ppi->getCenter().y), ppi->getCenter().z);
                for(uint vi=0; vi!=vertices.size(); ++vi) {
					int px = crosssection->realToPixel(crosssection->projectOntoCrossSection(vertices[vi].x*cos(ppi->getAzimuthrad())+crosssection->getX1(), -vertices[vi].x*sin(ppi->getAzimuthrad())+crosssection->getY1()));
					int py = crosssection->realToPixel(-vertices[vi].y);
					QPointF vertex = QPoint(px, py) + center;
					int dist = sqrt( pow(vertex.x()-p.x(),2) + pow(vertex.y()-p.y(),2) );
					if(dist<closestmousedistance) {
						closestmousedistance = dist;
						closestitem = ppi;
						closestvindex = vi;
					}
				}
			}
		}
	}
	if(closestitem) {
		polygonprismitem = closestitem;
		vip = closestvindex;
	}
	return closestmousedistance;
}

std::vector<int> CrossSectionScene::checkForVertex(QPoint p, std::vector<PolygonPrismItem*>& ppis, std::vector<int>& vip) {
	//Code duplication: closestVertex( ), checkForVertex( ) and checkForEdge( ) all share code/concept. See header for description of each.
	std::vector<int> closestmousedistance;
	std::vector<PolygonPrismItem*> closestitem; //Actually, this is the item that owns the closest vertex
	std::vector<int> closestvindex; //This is the index of a vertex in closestitem
	auto massitems = mainwindow->massItems();
	for(auto mi=massitems.begin(); mi!=massitems.end(); ++mi) {
		if(PolygonPrismItem* ppi = dynamic_cast<PolygonPrismItem*>(*mi)) {
			double az = fmod(ppi->getAzimuthrad() + atan2(crosssection->getY2()-crosssection->getY1(), crosssection->getX2()-crosssection->getX1()), 3.14159);
			if(az>-0.01 && az<0.01 && ppi->isVisible()) {
				closestmousedistance.push_back(10000);
				closestitem.push_back(ppi);
				closestvindex.push_back(0);
				auto vertices = ppi->getVertices();
				QPointF center = crosssection->getCrossSectionPoint(crosssection->projectOntoCrossSection(ppi->getCenter().x, ppi->getCenter().y), ppi->getCenter().z);
                for(uint vi=0; vi!=vertices.size(); ++vi) {
					int px = crosssection->realToPixel(crosssection->projectOntoCrossSection(vertices[vi].x*cos(ppi->getAzimuthrad())+crosssection->getX1(), -vertices[vi].x*sin(ppi->getAzimuthrad())+crosssection->getY1()));
					int py = crosssection->realToPixel(-vertices[vi].y);
					QPointF vertex = QPoint(px, py) + center;
					int dist = sqrt( pow(vertex.x()-p.x(),2) + pow(vertex.y()-p.y(),2) );
					if(dist<closestmousedistance[closestmousedistance.size()-1]) {
						closestmousedistance[closestmousedistance.size()-1] = dist;
						closestitem[closestitem.size()-1] = ppi;
						closestvindex[closestvindex.size()-1] = vi;
					}
				}
			}
		}
	}
	ppis = closestitem;
	vip = closestvindex;
	return closestmousedistance;
}

std::vector<int> CrossSectionScene::checkForEdge(QPoint p, std::vector<PolygonPrismItem*>& ppis, std::vector<int>& vi1) {
	//Code duplication: closestVertex( ), checkForVertex( ) and checkForEdge( ) all share code/concept. See header for description of each.
	std::vector<int> closestdistance;
	std::vector<PolygonPrismItem*> lineowneritem;
	std::vector<int> bestv1;

	auto massitems = mainwindow->massItems();
	for(auto mi=massitems.begin(); mi!=massitems.end(); ++mi) {
		if(PolygonPrismItem* ppi = dynamic_cast<PolygonPrismItem*>(*mi)) {
			double az = fmod(ppi->getAzimuthrad() + atan2(crosssection->getY2()-crosssection->getY1(), crosssection->getX2()-crosssection->getX1()), 3.14159);
			if(az>-0.01 && az<0.01 && ppi->isVisible()) {
				closestdistance.push_back(10000);
				lineowneritem.push_back(ppi);
				bestv1.push_back(0);
				auto vertices = ppi->getVertices();
				QPointF center = crosssection->getCrossSectionPoint(crosssection->projectOntoCrossSection(ppi->getCenter().x, ppi->getCenter().y), ppi->getCenter().z);
                for(uint i=0; i!=vertices.size(); ++i) {
					int j = (i+1)%vertices.size(); //j is the "next" vertex index
					QPointF vertexi = center + QPointF(crosssection->realToPixel(crosssection->projectOntoCrossSection(vertices[i].x*cos(ppi->getAzimuthrad())+crosssection->getX1(), -vertices[i].x*sin(ppi->getAzimuthrad())+crosssection->getY1())) , crosssection->realToPixel(-vertices[i].y)) ; 
					QPointF vertexj = center + QPointF(crosssection->realToPixel(crosssection->projectOntoCrossSection(vertices[j].x*cos(ppi->getAzimuthrad())+crosssection->getX1(), -vertices[j].x*sin(ppi->getAzimuthrad())+crosssection->getY1())) , crosssection->realToPixel(-vertices[j].y)) ; 
					QPointF jivector = vertexj - vertexi;
					QPointF pivector = p - vertexi;
					double dotproduct = jivector.x()*pivector.x()+jivector.y()*pivector.y();
					double jimag = sqrt(jivector.x()*jivector.x() + jivector.y()*jivector.y());
					double proj = dotproduct/jimag;
					double disttoline = sqrt((pivector.x()*pivector.x() + pivector.y()*pivector.y()) - proj*proj);
					if(disttoline<closestdistance[closestdistance.size()-1] && proj>0 && proj<jimag) {
						closestdistance[closestdistance.size()-1] = disttoline;
						lineowneritem[lineowneritem.size()-1] = ppi;
						bestv1[bestv1.size()-1] = i;
					}
				}
			}
		}
	}
	ppis = lineowneritem;
	vi1 = bestv1;
	return closestdistance;
}

void CrossSectionScene::setUiFlag(bool b) {
	bool switchedflag(b!=uiflag);
	uiflag = b;
	if(switchedflag) repaint();
}
