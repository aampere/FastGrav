#include "crosssection.h"
#include <QVector2D>
#include <QtDebug>
#include "observationset.h"
#include "mainwindow.h"
//#include "crosssectionitem.h"
#include "global.h"
#include "units.h"
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

CrossSection::CrossSection(MainWindow* mw, QWidget *parent) 
:QWidget(parent), mainwindow(mw), observationset(nullptr),window(nullptr),
x1(0), y1(0), x2(1), y2(0),
ylow(0), yhigh(1),
ytick0(0.0), ytickinterval(0.001), automaticyaxis(true),
xmin(0), xmax(1), //notice that this corresponds to the default x1,y1,x2,y2
leftmargin(100), rightmargin(20), bottommargin(30), topmargin(25), //It would make sense if these were tied/synced with the profilegraph's margins. For now they are not.
realzoffset(8),
zval(-1),
scene(this, this, mw)
{
	ui.setupUi(this);
	updateObservationSetCombo();
	connect(ui.obscombo,SIGNAL(currentIndexChanged(int)), this, SLOT(setObservationSet(int)));
	connect(ui.x1,SIGNAL(valueChanged(double)),this,SLOT(setX1(double)));
	connect(ui.y1,SIGNAL(valueChanged(double)),this,SLOT(setY1(double)));
	connect(ui.x2,SIGNAL(valueChanged(double)),this,SLOT(setX2(double)));
	connect(ui.y2,SIGNAL(valueChanged(double)),this,SLOT(setY2(double)));
    connect(ui.automaticcrossbounds,SIGNAL(clicked()),this,SLOT(automaticCheck()));
	//connect(ui.ylow,SIGNAL(editingFinished()),this,SLOT(setYLowFromLineEdit()));
	//connect(ui.yhigh,SIGNAL(editingFinished()),this,SLOT(setYHighFromLineEdit()));
	//connect(ui.automaticybounds,SIGNAL(clicked()),this,SLOT(automaticCheck()));
	connect(ui.yaxisoptions, SIGNAL(clicked()), this, SLOT(openYAxisDialog()));

	scene.setContextMenuPolicy(Qt::CustomContextMenu);
	connect(&scene, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customCrossSectionMenuSlot(QPoint)));
	savecrosssection = new QAction("Save...", this);
	//addvertexhere = new QAction("Add vertex here", this);
	connect(savecrosssection, SIGNAL(triggered()), this, SLOT(saveCrossSectionImage()));
	
	connect(ui.profilegraph, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customProfileMenuSlot(QPoint)));
	anchorhere = new QAction("Anchor profile here", this);
	saveprofile = new QAction("Save...", this);
	ui.profilegraph->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(anchorhere, SIGNAL(triggered()), this, SLOT(anchorProfileHere()));
	connect(saveprofile, SIGNAL(triggered()), this, SLOT(saveProfileImage()));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ui.splitter->addWidget(&scene);
	scene.setMinimumHeight(150);


	updateCrossSection();
	automaticCheck();
}
CrossSection::~CrossSection() {
	window = nullptr;
	mainwindow->removeCrossSection(this);
}
void CrossSection::setWindow(QMdiSubWindow* win) {
	window = win;
}
QMdiSubWindow* CrossSection::getWindow() {
	return window;
}
void CrossSection::closeWindow() {
	if(window) {
		window->close();
		window = nullptr;
	}
}
QByteArray CrossSection::saveSplitter() {
	return ui.splitter->saveState();
}
void CrossSection::restoreSplitter(QByteArray state) {
	ui.splitter->restoreState(state);
}
void CrossSection::syncUI() {
	ui.automaticcrossbounds->setChecked(automaticcrossbounds);
	ui.x1->setValue(x1);
	ui.x2->setValue(x2);
	ui.y1->setValue(y1);
	ui.y2->setValue(y2);
	automaticCheck();
}

double CrossSection::projectOntoCrossSection(double px, double py) {
	//If this is slow, replace with arithmetic version. That won't be hard.
	//The projection will map (x1,y1) to 0.0 (i.e., 0 on the x-axis of the graph)
	QVector2D wall(x2-x1, y2-y1); //The object point is being projected onto the wall. I can't think of good names for these!
	QVector2D object(px-x1,py-y1);
	double dotp = QVector2D::dotProduct(wall,object);  // = |wall|*|object|*cosT
	return dotp/wall.length();                         //the value we're after is |object|*cosT
}
std::vector<double> CrossSection::projectObservationSet() {
	//The projection will map (x1,y1) to 0.0 (i.e., 0 on thhe x-axis of the graph)
	std::vector<double> projected;
	if(observationset) {
		std::vector<Point> pts = observationset->getPoints();
		for(auto pt=pts.begin(); pt!=pts.end(); ++pt) {
			projected.push_back(projectOntoCrossSection(pt->x,pt->y));
		}
	}
	return projected;
}
void CrossSection::updateCrossSection() {
	if(observationset && observationset->isValid()) {
		std::vector<double> ymod;
		int size = observationset->getPoints().size();
		for(int i=0; i!=size; ++i) {
			ymod.push_back(observationset->getOffsetGravityEffect(i));
		}
        std::vector<double> xmod = projectObservationSet();
		if(xmod.size()!=ymod.size()) logline("CrossSection::updateCrossSection(): xmod and ymod are different sizes.");
		ui.profilegraph->setModeledPoints(xmod, ymod);
		ui.profilegraph->setEmpiricalPoints(observationset->getEmpGravityVals());
		if(ui.automaticcrossbounds->isChecked()) {
			xmin = 0.0;
			xmax = *std::max_element(xmod.begin(), xmod.end());
			ui.profilegraph->setXMinMax(xmin,xmax);
			//ui.profilegraph->setYMinMax(*std::min_element(ymod.begin(), ymod.end()),*std::max_element(ymod.begin(), ymod.end())); //this could be improved by using min_max_element
		} else {
			xmin = 0.0;
			xmax = projectOntoCrossSection(x2,y2);
			ui.profilegraph->setXMinMax(xmin, xmax);
			//for now, have y still be automatic
			//ui.profilegraph->setYMinMax(*std::min_element(ymod.begin(), ymod.end()),*std::max_element(ymod.begin(), ymod.end())); //this could be improved by using min_max_element
		}
		if(automaticyaxis) {
			double yemphigh = -1000000000;
			double yemplow = 1000000000;
			auto yemp = observationset->getEmpGravityVals();
			for(auto ye=yemp.begin(); ye!=yemp.end(); ++ye) {
				if(!isNoData(*ye)) {
					yemphigh = std::max(yemphigh, *ye);
					yemplow = std::min(yemplow, *ye);
				}
			}
			ylow =  std::min(yemplow, *std::min_element(ymod.begin(), ymod.end()));//could be improved slightly by using min_max_element
			yhigh = std::max(yemphigh, *std::max_element(ymod.begin(), ymod.end()));
			ytick0 = 0.0; //when the y-axis is automatic, have the friendly tick0 of 0.0
			double range = yhigh-ylow;
			double exp = ceil(log10(range*.6));
			ytickinterval = pow(10,exp-1);
		} else { 

		}
		ui.profilegraph->setYMinMax(ylow, yhigh);
		ui.profilegraph->setYTick0(ytick0);
		ui.profilegraph->setYTickInterval(ytickinterval);
		//ui.ylow->blockSignals(true);  ui.ylow->setText(QString::number(Units::siToDisplayGrav(ylow)));   ui.ylow->blockSignals(false); 
		//ui.yhigh->blockSignals(true); ui.yhigh->setText(QString::number(Units::siToDisplayGrav(yhigh))); ui.yhigh->blockSignals(false);
		//updateCrossSectionItems();
		if(mainwindow) mainwindow->setSaved(false);
	} else {
		if(ui.automaticcrossbounds->isChecked()) {
			xmin = 0.0;
			xmax = 1.0;
		} else {
			xmin = 0.0;
			xmax = projectOntoCrossSection(x2,y2);
		}
		ui.profilegraph->setModeledPoints(std::vector<double>(), std::vector<double>());
		ui.profilegraph->setEmpiricalPoints(std::vector<double>());
		ui.profilegraph->setXMinMax(xmin, xmax);
		pushUnitUpdate();
		//updateCrossSectionItems();
		if(mainwindow) mainwindow->setSaved(false);
	}
	scene.update();
}
void CrossSection::pushUnitUpdate() {
	ui.x1->blockSignals(true); ui.y1->blockSignals(true); ui.x2->blockSignals(true); ui.y2->blockSignals(true);
	ui.x1->setValue(Units::siToDisplayDistance(x1));
	ui.y1->setValue(Units::siToDisplayDistance(y1));
	ui.x2->setValue(Units::siToDisplayDistance(x2));
	ui.y2->setValue(Units::siToDisplayDistance(y2));
	ui.x1->blockSignals(false); ui.y1->blockSignals(false); ui.x2->blockSignals(false); ui.y2->blockSignals(false);
}

void CrossSection::updateObservationSetCombo() {
	if(mainwindow) {
		ui.obscombo->blockSignals(true);
		ui.obscombo->clear();
		ui.obscombo->addItem("[Select Observation Set]");
		std::vector<ObservationSet*> osets = mainwindow->observationSets();
		for(auto obs=osets.begin(); obs!=osets.end(); ++obs) {
			ui.obscombo->addItem((*obs)->getName());
			if((*obs)==observationset) {
				ui.obscombo->setCurrentIndex(ui.obscombo->count()-1);
			}
		}
		ui.obscombo->blockSignals(false);
	} else {
		logline("CrossSection::updateObservationSetCombo() without mainwindow pointer.");
	}
}
void CrossSection::clearObservationSet(ObservationSet* os) {
	updateObservationSetCombo();
	if(os==observationset) {
		observationset = nullptr;
		ui.profilegraph->setObservationSet(nullptr);
		ui.obscombo->setCurrentIndex(0);

		std::vector<double> empty;
		ui.profilegraph->setModeledPoints(empty, empty);
		ui.profilegraph->setEmpiricalPoints(empty);
		//updateCrossSectionItems();
	}
	updateObservationSetCombo();
}
void CrossSection::setObservationSet(int i) {
	if(i==0) {
		observationset = nullptr;
		ui.profilegraph->setObservationSet(nullptr);
		ui.obscombo->setCurrentIndex(0);

		std::vector<double> empty;
		ui.profilegraph->setModeledPoints(empty, empty);
		ui.profilegraph->setEmpiricalPoints(empty);
	} else if(i-1<mainwindow->observationSets().size()) {
		observationset = mainwindow->observationSets()[i-1];
		ui.profilegraph->setObservationSet(observationset);
		auto pts = observationset->getPoints();
	}
	ui.obscombo->blockSignals(true);
	ui.obscombo->setCurrentIndex(i);
	ui.obscombo->blockSignals(false);
	automaticCheck();
	updateCrossSection();
}
ObservationSet* CrossSection::getObservationSet() {
	return observationset;
}
int CrossSection::getObservationSetIndex() {
	if(mainwindow) {
		auto observationsets = mainwindow->observationSets();
		int i = 0;
		for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
			if(*os==observationset) {
				return i;
			}
			++i;
		}
	}
	return -1;
}

void CrossSection::setX1(double d) {//triggered only when spinbox changes
	x1 = Units::displayToSiDistance(d);
	updateCrossSection();
}
void CrossSection::setY1(double d) {//triggered only when spinbox changes
	y1 = Units::displayToSiDistance(d);
	updateCrossSection();
}
void CrossSection::setX2(double d) {//triggered only when spinbox changes
	x2 = Units::displayToSiDistance(d);
	updateCrossSection();
}
void CrossSection::setY2(double d) {//triggered only when spinbox changes
	y2 = Units::displayToSiDistance(d);
	updateCrossSection();
}

void CrossSection::automaticCheck() {
	automaticcrossbounds = ui.automaticcrossbounds->isChecked();
	ui.x1->setEnabled(!ui.automaticcrossbounds->isChecked());
	ui.y1->setEnabled(!ui.automaticcrossbounds->isChecked());
	ui.x2->setEnabled(!ui.automaticcrossbounds->isChecked());
	ui.y2->setEnabled(!ui.automaticcrossbounds->isChecked());
	if(ui.automaticcrossbounds->isChecked() && observationset) {
		automaticCrossBoundaries(); //sets x1,y2,x2,y2 automatically once
	}
	updateCrossSection();
}
void CrossSection::automaticCrossBoundaries() {
	if(observationset) {
		//for now, just use the first and last points of an observation set as the boundary points. A linear regression could also be used. Neither approach would play nice with sets that are not naturally "transect-ish."
		std::vector<Point> pts = observationset->getPoints();
		if(observationset->isValid()) {
			x1 = pts[0].x;
			y1 = pts[0].y;
			x2 = pts[pts.size()-1].x;
			y2 = pts[pts.size()-1].y;
		}
		pushUnitUpdate();
	}
}
void CrossSection::automaticYBoundaries() {
	updateCrossSection();
}

QList<double> CrossSection::getMargins() {
	return QList<double>() << leftmargin<< rightmargin<< bottommargin<< topmargin;
}
bool CrossSection::getAutomaticCrossBounds() {
	return automaticcrossbounds;
}
double CrossSection::getX1() {
	return x1;
}
double CrossSection::getX2() {
	return x2;
}
double CrossSection::getY1() {
	return y1;
}
double CrossSection::getY2() {
	return y2;
}
double CrossSection::getYLow() {
	return ylow;
}
double CrossSection::getYHigh() {
	return yhigh;
}
double CrossSection::getYTick0() {
	return ytick0;
}
double CrossSection::getYTickInterval() {
	return ytickinterval;
}
bool CrossSection::isYAxisAutomatic() {
	return automaticyaxis;
}
double CrossSection::getXMin() {
	return xmin;
}
double CrossSection::getXMax() {
	return xmax;
}
double CrossSection::getRealZOffset() {
	return realzoffset;
}
double CrossSection::getZTickInterval() {
	return ui.profilegraph->getXTickInterval();
}

void CrossSection::setAutomaticCrossBounds(bool b) {
	automaticcrossbounds = b;
	updateCrossSection();
}
void CrossSection::setYLow(double d) {
	ylow = d;
	updateCrossSection();
}
void CrossSection::setYHigh(double d) {
	yhigh = d;
	updateCrossSection();
}
void CrossSection::setYTick0(double d) {
	ytick0 = d;
	updateCrossSection();
}
void CrossSection::setYTickInterval(double d) {
	ytickinterval = d;
	updateCrossSection();
}
void CrossSection::setYAxisAutomatic(bool b) {
	automaticyaxis = b;
	updateCrossSection();
}
void CrossSection::setXMin(double d) {
	xmin = d;
	updateCrossSection();
}
void CrossSection::setXMax(double d) {
	xmax = d;
	updateCrossSection();
}
void CrossSection::setRealZOffset(double rzo) {
	realzoffset = rzo;
	updateCrossSection();
}

void CrossSection::scrollRealZOffset(int px) {
	double realtopixel = (xmax-xmin)/(scene.width()-leftmargin-rightmargin);
	realzoffset += px*realtopixel;
	updateCrossSection();
}

void CrossSection::updateMassItemFromPosition(int i, QPointF pt) {
	MassItem* mi = nullptr;
	if(mainwindow && i<mainwindow->massItems().size()) {
		mi = mainwindow->massItems()[i];
	}
	if(mi) {
		if(SphereItem* si = dynamic_cast<SphereItem*>(mi)) {
			double originalmix = si->getCenter().x;
			double originalmiy = si->getCenter().y;
			QPointF originalproj = getCrossSectionPoint(projectOntoCrossSection(originalmix, originalmiy), 0.0);//z doesn't matter here. Just put in 0.0
			auto originalprojrealxz = getRealXZfromCrossSectionPoint(originalproj);
			double originalprojrealx = x1+((originalprojrealxz.first-xmin)/(xmax-xmin))*(x2-x1);
			double originalprojrealy = y1+((originalprojrealxz.first-xmin)/(xmax-xmin))*(y2-y1);

			double xdif = originalprojrealx-originalmix;//This is the vector from the MassItem's original, real position, to the real position of its projected point.
			double ydif = originalprojrealy-originalmiy;//

			auto realxz = getRealXZfromCrossSectionPoint(pt);
			double realx = x1+((realxz.first-xmin)/(xmax-xmin))*(x2-x1);//these are the real x and y coords of the drag point ON THE CURRENT CROSS SECTION
			double realy = y1+((realxz.first-xmin)/(xmax-xmin))*(y2-y1);//
			si->setCenter(Point(realx-xdif, realy-ydif, realxz.second));//subtract the difference vector from the drag point's real position to get the MassItem's new real postion.
		} else if(CylinderItem* ci = dynamic_cast<CylinderItem*>(mi)) {
			double originalmix = ci->getCenter().x;
			double originalmiy = ci->getCenter().y;
			QPointF originalproj = getCrossSectionPoint(projectOntoCrossSection(originalmix, originalmiy), 0.0);//z doesn't matter here. Just put in 0.0
			auto originalprojrealxz = getRealXZfromCrossSectionPoint(originalproj);
			double originalprojrealx = x1+((originalprojrealxz.first-xmin)/(xmax-xmin))*(x2-x1);
			double originalprojrealy = y1+((originalprojrealxz.first-xmin)/(xmax-xmin))*(y2-y1);

			double xdif = originalprojrealx-originalmix;//This is the vector from the MassItem's original, real position, to the real position of its projected point.
			double ydif = originalprojrealy-originalmiy;//

			auto realxz = getRealXZfromCrossSectionPoint(pt);
			double realx = x1+((realxz.first-xmin)/(xmax-xmin))*(x2-x1);//these are the real x and y coords of the drag point ON THE CURRENT CROSS SECTION
			double realy = y1+((realxz.first-xmin)/(xmax-xmin))*(y2-y1);//
			ci->setCenter(Point(realx-xdif, realy-ydif, realxz.second));//subtract the difference vector from the drag point's real position to get the MassItem's new real postion.
		} else if(CloudItem* cli = dynamic_cast<CloudItem*>(mi)) {
			double originalmix = cli->getCenter().x;
			double originalmiy = cli->getCenter().y;
			QPointF originalproj = getCrossSectionPoint(projectOntoCrossSection(originalmix, originalmiy), 0.0);//z doesn't matter here. Just put in 0.0
			auto originalprojrealxz = getRealXZfromCrossSectionPoint(originalproj);
			double originalprojrealx = x1+((originalprojrealxz.first-xmin)/(xmax-xmin))*(x2-x1);
			double originalprojrealy = y1+((originalprojrealxz.first-xmin)/(xmax-xmin))*(y2-y1);

			double xdif = originalprojrealx-originalmix;//This is the vector from the MassItem's original, real position, to the real position of its projected point.
			double ydif = originalprojrealy-originalmiy;//

			auto realxz = getRealXZfromCrossSectionPoint(pt);
			double realx = x1+((realxz.first-xmin)/(xmax-xmin))*(x2-x1);//these are the real x and y coords of the drag point ON THE CURRENT CROSS SECTION
			double realy = y1+((realxz.first-xmin)/(xmax-xmin))*(y2-y1);//
			cli->setCenter(Point(realx-xdif, realy-ydif, realxz.second));//subtract the difference vector from the drag point's real position to get the MassItem's new real postion.
		} else if(PolygonPrismItem* ppi = dynamic_cast<PolygonPrismItem*>(mi)) {
			double originalmix = ppi->getCenter().x;
			double originalmiy = ppi->getCenter().y;
			QPointF originalproj = getCrossSectionPoint(projectOntoCrossSection(originalmix, originalmiy), 0.0);//z doesn't matter here. Just put in 0.0
			auto originalprojrealxz = getRealXZfromCrossSectionPoint(originalproj);
			double originalprojrealx = x1+((originalprojrealxz.first-xmin)/(xmax-xmin))*(x2-x1);
			double originalprojrealy = y1+((originalprojrealxz.first-xmin)/(xmax-xmin))*(y2-y1);

			double xdif = originalprojrealx-originalmix;//This is the vector from the MassItem's original, real position, to the real position of its projected point.
			double ydif = originalprojrealy-originalmiy;//

			auto realxz = getRealXZfromCrossSectionPoint(pt);
			double realx = x1+((realxz.first-xmin)/(xmax-xmin))*(x2-x1);//these are the real x and y coords of the drag point ON THE CURRENT CROSS SECTION
			double realy = y1+((realxz.first-xmin)/(xmax-xmin))*(y2-y1);//
			ppi->setCenter(Point(realx-xdif, realy-ydif, realxz.second));//subtract the difference vector from the drag point's real position to get the MassItem's new real postion.
		} else {
			logline("CrossSection::updateMassItemFromPosition( ) couldn't cast the massitem.");
		}
	} else {
		logline("CrossSection::updateMassItemFromPosition( ) could not match a crosssectionitem to corresponding massitem.");
	}
}
void CrossSection::updateMassItemFromRadius(int i, double r) {
	double rad = r;
	MassItem* mi = nullptr;
	if(mainwindow && i<mainwindow->massItems().size()) {
		mi = mainwindow->massItems()[i];
	}
	if(mi) {
		double realtopixel = (xmax-xmin)/(scene.width()-leftmargin-rightmargin);
		if(SphereItem* si = dynamic_cast<SphereItem*>(mi)) {
			si->setRadius(realtopixel*rad);
		} else if(CylinderItem* ci = dynamic_cast<CylinderItem*>(mi)) {
			ci->setRadius(realtopixel*rad);
        } /*else if(CloudItem* cli = dynamic_cast<CloudItem*>(mi)) {

		} else if(PolygonPrismItem* ppi = dynamic_cast<PolygonPrismItem*>(mi)) {

        }*/ else {
			logline("CrossSection::updateMassItemFromPosition( ) couldn't cast the massitem.");
		}
	} else {
		logline("CrossSection::updateMassItemFromPosition( ) could not match a crosssectionitem to corresponding massitem.");
	}
	//updateCrossSection();
}

void CrossSection::updateVertexFromPostion(PolygonPrismItem* ppi, int vindex, QPointF pt) {
	//MassItem* mi = getMassItemFromCrossSectionItem(ci);
	if(ppi) {
		//if(PolygonPrismItem* ppi = dynamic_cast<PolygonPrismItem*>(mi)) { //This should ALWAYS be a successful cast, because CrossSectionScene checks the Role of the CrossSectionItem
			//The following assumes that the crosssection is angled to look *down-azimuth*. Thus, the vertex is translated along to the crosssection x-axis.
			auto realxz = getRealXZfromCrossSectionPoint(pt);
			double realcenterz = ppi->getCenter().z;
			double realcenterx = projectOntoCrossSection(ppi->getCenter().x, ppi->getCenter().y);
			//vertices are defined relative to the center of the ppi, so the new vertex position is
			ppi->setVertex(vindex, realxz.first-realcenterx, realxz.second-realcenterz);
		//}
	}
}

QPointF CrossSection::getCrossSectionPoint(double realx, double realz) {
	double gx = (realx-xmin)/(xmax-xmin)*(scene.width()-leftmargin-rightmargin)+leftmargin;
	double pixeltoreal = (scene.width()-leftmargin-rightmargin)/(xmax-xmin);
	double gy = topmargin+(-(realz-realzoffset)*pixeltoreal);
	return QPointF(gx,gy);
}
std::pair<double, double> CrossSection::getRealXZfromCrossSectionPoint(QPointF pt) {
	double realx = (pt.x()-leftmargin)/(scene.width()-rightmargin-leftmargin)*(xmax-xmin)+xmin;
	double realtopixel = (xmax-xmin)/(scene.width()-leftmargin-rightmargin);
	double realz = -((pt.y()-topmargin)*realtopixel) + realzoffset;
	return std::make_pair(realx, realz);
}
double CrossSection::pixelToReal(double d) {//pixel input, returns Realworld distance.
	return d*(xmax-xmin)/(scene.width()-leftmargin-rightmargin);
}
double CrossSection::realToPixel(double d) {//real-world distancce input, returns pixel distance.
	return d*(scene.width()-leftmargin-rightmargin)/(xmax-xmin);
}

QVector<QLineF> CrossSection::linesFromPoints(std::vector<double> nx, std::vector<double> ny) {
	QVector<QLineF> lns;
	if(nx.size()>1) {
        for(uint i=0; i!=nx.size()-1; ++i) {
			lns.push_back(QLineF(getCrossSectionPoint(nx[i],ny[i]), getCrossSectionPoint(nx[i+1],ny[i+1])));
		}
	}
	return lns;
}

void CrossSection::customCrossSectionMenuSlot(const QPoint& p) {
	menupoint = p;
    QMenu cmenu("Menu", &scene);
	vertexitems.clear();
	vertexvis.clear();
	edgeitems.clear();
	edgevis.clear();

	std::vector<int> vertexdistances = scene.checkForVertex(p, vertexitems, vertexvis);
	bool vertexoptions = vertexdistances.empty()?false:((*(std::min_element(vertexdistances.begin(), vertexdistances.end()))) < 5.0);
	std::vector<int> edgedistances = scene.checkForEdge(p, edgeitems, edgevis);
	bool edgeoptions = edgedistances.empty()?false:((*(std::min_element(edgedistances.begin(), edgedistances.end()))) < 10.0);

	if(vertexoptions) {
		QSignalMapper* removemapper = new QSignalMapper(this);
		QSignalMapper* leftmapper = new QSignalMapper(this);
		QSignalMapper* rightmapper = new QSignalMapper(this);
		QList<QAction*> actions;
        for(uint i=0; i!=vertexdistances.size(); ++i) {
			actions.append(new QAction("Remove this vertex (\""+vertexitems[i]->getName()+"\")",this));
		}
        for(uint i=0; i!=vertexdistances.size(); ++i) {
			actions.append(new QAction("Send this vertex far left (\""+vertexitems[i]->getName()+"\")",this));
		}
        for(uint i=0; i!=vertexdistances.size(); ++i) {
			actions.append(new QAction("Send this vertex far right (\""+vertexitems[i]->getName()+"\")",this));
		}
        for(uint i=0; i!=vertexdistances.size(); ++i) {
			if(vertexdistances[i]<5) {
				cmenu.addAction(actions[i]);
				//Qt::UniqueConnection, because for some reason triggered() or mapped(int) was firing twice when 2+ vertices were in question. stack exchange suggests it might be a Qt4.8 bug (http://stackoverflow.com/questions/10473719/why-does-this-slot-get-called-twice)
				connect(actions[i], SIGNAL(triggered()), removemapper, SLOT(map()),Qt::UniqueConnection);
				removemapper->setMapping(actions[i], i);
				connect(removemapper, SIGNAL(mapped(int)), this, SLOT(removeThisVertexSlot(int)),Qt::UniqueConnection);
			}
		}
        for(uint i=0; i!=vertexdistances.size(); ++i) {
			if(vertexdistances[i]<5) {
				cmenu.addAction(actions[i+vertexdistances.size()]);
				//Qt::UniqueConnection, because for some reason triggered() or mapped(int) was firing twice when 2+ vertices were in question. stack exchange suggests it might be a Qt4.8 bug (http://stackoverflow.com/questions/10473719/why-does-this-slot-get-called-twice)
				connect(actions[i+vertexdistances.size()], SIGNAL(triggered()), leftmapper, SLOT(map()),Qt::UniqueConnection);
				leftmapper->setMapping(actions[i+vertexdistances.size()], i);
				connect(leftmapper, SIGNAL(mapped(int)), this, SLOT(sendThisVertexLeftSlot(int)),Qt::UniqueConnection);
			}
		}
        for(uint i=0; i!=vertexdistances.size(); ++i) {
			if(vertexdistances[i]<5) {
				cmenu.addAction(actions[i+2*vertexdistances.size()]);
				//Qt::UniqueConnection, because for some reason triggered() or mapped(int) was firing twice when 2+ vertices were in question. stack exchange suggests it might be a Qt4.8 bug (http://stackoverflow.com/questions/10473719/why-does-this-slot-get-called-twice)
				connect(actions[i+2*vertexdistances.size()], SIGNAL(triggered()), rightmapper, SLOT(map()),Qt::UniqueConnection);
				rightmapper->setMapping(actions[i+2*vertexdistances.size()], i);
				connect(rightmapper, SIGNAL(mapped(int)), this, SLOT(sendThisVertexRightSlot(int)),Qt::UniqueConnection);
			}
		}
	} else if(edgeoptions) {
		QSignalMapper* mapper = new QSignalMapper(this);
		QList<QAction*> actions;
        for(uint i=0; i!=edgedistances.size(); ++i) {
			actions.append(new QAction("Add vertex here (\""+edgeitems[i]->getName()+"\")", this));
		}
        for(uint i=0; i!=edgedistances.size(); ++i) {
			if(edgedistances[i]<10) {
				cmenu.addAction(actions[i]);
				//Qt::UniqueConnection, because for some reason triggered() or mapped(int) was firing twice when 2+ edges were in question. stack exchange suggests it might be a Qt4.8 bug (http://stackoverflow.com/questions/10473719/why-does-this-slot-get-called-twice)
				connect(actions[i], SIGNAL(triggered()), mapper, SLOT(map()),Qt::UniqueConnection);
				mapper->setMapping(actions[i], i);
				connect(mapper, SIGNAL(mapped(int)), this, SLOT(addVertexHereSlot(int)),Qt::UniqueConnection);
			}
		}
	}
	cmenu.addSeparator();
	cmenu.addAction(savecrosssection);
	cmenu.exec(QCursor::pos());
}

void CrossSection::sendThisVertexLeftSlot(int i) {
	PolygonPrismItem* ppi = vertexitems[i];
	Point originalvertex = ppi->getVertices()[vertexvis[i]];
	ppi->setVertex(vertexvis[i], originalvertex.x-10000, originalvertex.y);
	//if(grabbeditem) {
	//	Point originalvertex = grabbeditem->getVertices()[grabbedvindex];
	//	grabbeditem->setVertex(grabbedvindex, originalvertex.x-10000, originalvertex.y);
	//}
}
void CrossSection::sendThisVertexRightSlot(int i) {
	PolygonPrismItem* ppi = vertexitems[i];
	Point originalvertex = ppi->getVertices()[vertexvis[i]];
	ppi->setVertex(vertexvis[i], originalvertex.x+10000, originalvertex.y);
	//if(grabbeditem) {
	//	Point originalvertex = grabbeditem->getVertices()[grabbedvindex];
	//	grabbeditem->setVertex(grabbedvindex, originalvertex.x+10000, originalvertex.y);
	//}
}
void CrossSection::removeThisVertexSlot(int i) {
	PolygonPrismItem* ppi = vertexitems[i];
	ppi->removeVertex(vertexvis[i]);
}
void CrossSection::addVertexHereSlot(int i) {
	PolygonPrismItem* ppi = edgeitems[i];
	//The following assumes that the crosssection is angled to look *down-azimuth*. Thus, the vertex is translated along to the crosssection x-axis.
	auto realxz = getRealXZfromCrossSectionPoint(menupoint);
	double realcenterz = ppi->getCenter().z;
	double realcenterx = projectOntoCrossSection(ppi->getCenter().x, ppi->getCenter().y);
	//vertices are defined relative to the center of the ppi, so the new vertex position is
	ppi->insertVertex(edgevis[i], realxz.first-realcenterx, realxz.second-realcenterz);
}

void CrossSection::saveCrossSectionImage() {
	scene.setUiFlag(false);
	QString png = "*.png";
	QString jpg = "*.jpg";
	QString tif = "*.tif";
	QFileDialog dialog(this,"Save Cross Section");
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilters(QStringList()<<jpg<<png<<tif);
	dialog.setLabelText(QFileDialog::Accept,"Save");
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	if( dialog.exec() ) { 
		QString fileName = dialog.selectedFiles().first();
		QString filter = dialog.selectedNameFilter();
		if(filter==png) {
			if(!fileName.endsWith(".png"))
				fileName.append(".png");
		} else if(filter==jpg) {
			if(!fileName.endsWith(".jpg"))
				fileName.append(".jpg");
		} else if(filter==tif) {
			if(!fileName.endsWith(".tif"))
				fileName.append(".tif");
		}
		if(!fileName.isEmpty()) {
			QPixmap pixmap(scene.width(), scene.height());
			QPainter p(&pixmap);
			scene.render(&p);
			if(pixmap.toImage().save(fileName, 0, 100)) { //0 tells Qt to guess the format based on the suffix, 100 tells Qt to save at 100% quality

			} else {
				logline("Unable to save Cross Section to " +fileName);
			}
		}
	}
}

void CrossSection::customProfileMenuSlot(const QPoint& p) {
	menupoint = p;
	QMenu cmenu("Menu", ui.profilegraph);
	int eindex = -1;
	if(ui.profilegraph->closestEmpiricalPoint(p, eindex) < 6) {
		cmenu.addAction(anchorhere);
		selectedeindex = eindex;
	}
	cmenu.addSeparator();
	cmenu.addAction(saveprofile);
	cmenu.exec(QCursor::pos());
}
void CrossSection::anchorProfileHere() {
    mainwindow->setOffsetType(MainWindow::Anchor);
    mainwindow->setOffsetAnchor(observationset, selectedeindex);
}
void CrossSection::saveProfileImage() {
	QString png = "*.png";
	QString jpg = "*.jpg";
	QString tif = "*.tif";
	QFileDialog dialog(this,"Save Profile");
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilters(QStringList()<<jpg<<tif);
	dialog.setLabelText(QFileDialog::Accept,"Save");
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	if( dialog.exec() ) { 
		QString fileName = dialog.selectedFiles().first();
		QString filter = dialog.selectedNameFilter();
		if(filter==png) {
			if(!fileName.endsWith(".png"))
				fileName.append(".png");
		} else if(filter==jpg) {
			if(!fileName.endsWith(".jpg"))
				fileName.append(".jpg");
		} else if(filter==tif) {
			if(!fileName.endsWith(".tif"))
				fileName.append(".tif");
		}
		if(!fileName.isEmpty()) {
			QPixmap pixmap(ui.profilegraph->width(), ui.profilegraph->height());
			QPainter p(&pixmap);
			ui.profilegraph->render(&p);
			if(pixmap.toImage().save(fileName, 0, 100)) { //0 tells Qt to guess the format based on the suffix, 100 tells Qt to save at 100% quality

			} else {
				logline("Unable to save Profile to " + fileName);
			}
		}
	}
}
void CrossSection::openYAxisDialog() {
	YAxisUI* yaxiswindow = new YAxisUI(this);//probably not actually necessary to pass this pointer to the UI.
	if(yaxiswindow->exec() == QDialog::Accepted) {
		if(yaxiswindow->ui.automatic->isChecked()) {
			automaticyaxis = true;
		} else if(yaxiswindow->ui.manual->isChecked()) {
			automaticyaxis = false;
			//The below text-to-doubles will have already been validated by yaxisui
			double newylow = Units::displayToSiGrav(yaxiswindow->ui.ymin->text().toDouble());
			double newyhigh = Units::displayToSiGrav(yaxiswindow->ui.ymax->text().toDouble());
			double newytick0 = Units::displayToSiGrav(yaxiswindow->ui.tick0->text().toDouble());
			double newytickinterval = Units::displayToSiGrav(yaxiswindow->ui.tickinterval->text().toDouble());
			if(newylow<newyhigh) {
				ylow = newylow;
				yhigh = newyhigh;
			}
			if(newytick0>0) {
				ytick0 = newytick0;
			}
			if((yhigh-ylow)/newytickinterval<1000) { //if the user picked a tick interval that would cause a ludicrous amount of ticks, set the tick interval to something reasonable.
				ytickinterval = newytickinterval;
			} else {
				double exp = ceil(log10((yhigh-ylow)*.55));
				ytickinterval = Units::displayToSiDistance(pow(10,exp-1));
			}
		}
		updateCrossSection();
	} else {

	}
}
