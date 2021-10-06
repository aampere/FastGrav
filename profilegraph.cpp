#include "profilegraph.h"
#include "observationset.h"
#include "global.h"
#include "units.h"
#include <QtDebug>
#include <QMouseEvent>

ProfileGraph::ProfileGraph( QWidget* parent)
:QWidget(parent), observationset(nullptr),
leftmargin(100), rightmargin(20), bottommargin(30), topmargin(25)
{
	xmin = 0;
	xmax = 1;
	ymin = 0;
	ymax = 100;
	xtickinterval = 10.0;
	ytickinterval = 20.0;
	setMouseTracking(true);
}
ProfileGraph::~ProfileGraph() {

}
void ProfileGraph::setObservationSet(ObservationSet* os) {
	observationset = os;
}
void ProfileGraph::mouseMoveEvent(QMouseEvent * event) {
	mousepos = event->pos();
	fillInfoBox(mousepos);
	update();  //QWdiget::update()
}

void ProfileGraph::setEmpiricalPoints(std::vector<double> ny) {
	yemp = ny;
	update(); //QWdiget::update()
}
void ProfileGraph::setModeledPoints(std::vector<double> nx, std::vector<double> ny) {
	xmod = nx;
	ymod = ny;
	update(); //QWdiget::update()
}
void ProfileGraph::setXMinMax(double a, double b) {
	xmin = a;
	xmax = b;
	double xmindisplay = Units::siToDisplayDistance(xmin);
	double xmaxdisplay = Units::siToDisplayDistance(xmax);
	double range = xmaxdisplay-xmindisplay;
	double exp = ceil(log10(range*.55));
	xtickinterval = Units::displayToSiDistance(pow(10,exp-1));
	update(); //QWdiget::update()
}
void ProfileGraph::setYMinMax(double a, double b) {
	ymin = a;
	ymax = b;
	//double range = ymax-ymin;
	//double exp = ceil(log10(range*.7));
	//ytickinterval = pow(10,exp-1);
	update(); //QWdiget::update()
}
void ProfileGraph::setYTick0(double d) {
	ytick0 = d;
}
void ProfileGraph::setYTickInterval(double d) {
	ytickinterval = d;
}

void ProfileGraph::paintEvent(QPaintEvent *event) {

	QImage graph = QImage(width(),height(),QImage::Format_ARGB32_Premultiplied);
	graph.fill(Qt::white);
	QPainter p(&graph);

	int h = height();
	int w = width();
	
	QFont font = p.font();
	font.setPixelSize(12);
	p.setFont(font);
	QFontMetrics fm = p.fontMetrics();

	QPen axispen(QBrush(Qt::black), 2);
	QPen tickpen(QBrush(Qt::black), 1);
	QPen emppen(QBrush(Qt::darkGray), 1);
	QPen modpen(QBrush(Qt::blue), 1);

	//draw emp data
	p.setPen(emppen);
	p.drawLines(linesFromPoints(xmod, yemp));
    for(uint i=0; i!=xmod.size(); ++i) {
		if(!isNoData(yemp[i])){
			QPointF center(getGraphPoint(xmod[i], yemp[i]));
			p.fillRect(QRect(center.x()-1, center.y()-1, 3, 3), Qt::black);
		}
	}
	//draw mod data
	p.setPen(modpen);
	p.drawLines(linesFromPoints(xmod, ymod));
    for(uint i=0; i!=xmod.size(); ++i) {
		QPointF center(getGraphPoint(xmod[i], ymod[i]));
		p.fillRect(QRect(center.x()-1, center.y()-1, 3, 3), Qt::black);
	}
	//draw "masking" rectangles
	p.fillRect(QRectF(0,0,leftmargin, h), Qt::white);
	p.fillRect(QRectF(0,h-bottommargin,w, bottommargin), Qt::white);
	p.fillRect(QRectF(w-rightmargin,0,rightmargin, h), Qt::white);
	// draw 3 axis lines
	p.setPen(axispen);
	p.drawLine(leftmargin,topmargin, leftmargin, h-bottommargin);
	p.drawLine(leftmargin, h-bottommargin, w-rightmargin, h-bottommargin);
	p.drawLine(w-rightmargin, h-bottommargin, w-rightmargin, topmargin);
	//draw axis ticks and tick Labels
	p.setPen(tickpen);
	for(int i=0; i<=(xmax-xmin)/xtickinterval; ++i) {
		QPointF first = getGraphPoint(xmin+i*xtickinterval, ymin);
		p.drawLine(first, QPointF(first.x(), first.y()-4));
		QString tag = QString::number(Units::siToDisplayDistance(xmin+i*xtickinterval));
		int tagw = fm.width(tag);
		p.drawText(QRect(first.x()-tagw/2+1, first.y()+3, tagw, 12), Qt::AlignCenter, tag);
	}
	double ytickmod = fmod(ymin-ytick0, ytickinterval);
	for(int i=-1; ymin-ytickmod+i*ytickinterval<=ymax; ++i) {
		double ytickval = ymin-ytickmod+i*ytickinterval;
		if(ytickval>=ymin && ytickval<=ymax) {
			QPointF first = getGraphPoint(xmin, ytickval);
			p.drawLine(first, QPointF(first.x()+4, first.y()));
			p.drawText(QRect(0, first.y()-7, leftmargin-3, 12),Qt::AlignRight, QString::number(Units::siToDisplayGrav(ytickval)));
		}
	}
	//draw axis labels
	p.drawText(5,5,   50,12,Qt::AlignLeft, Units::gravSuffix());
	p.drawText(leftmargin-45,h-17,50,12,Qt::AlignLeft, Units::distanceSuffix());

	//QFontMetrics fontmetrics=p.fontMetrics();
	//int fw = fontmetrics.width("Aaron Price");
	//int fh = fontmetrics.height();
	//p.fillRect(mousepos.x(), mousepos.y(), fw,fh,Qt::red );
	//p.drawText(QRect(mousepos.x(), mousepos.y(), fw,fh), "Aaron Price");

	QPainter q(this);
	q.drawImage(0,0,graph);
	if(underMouse()) {
		q.drawImage(0,0,infobox);
	}
}
void ProfileGraph::fillInfoBox(QPoint p) {
	QImage newinfobox(width(),height(),QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&newinfobox);
	QFont font = painter.font();
	font.setPixelSize(12);
	painter.setFont(font);
	QFontMetrics fm = painter.fontMetrics();

	QPointF realpt = getRealXYFromGraphPoint(p);
	if(p.x()>leftmargin && p.x()<width()-rightmargin && p.y()>topmargin-15 && p.y()<height()-bottommargin) {
		QStringList lines = QStringList() << QString("graph x:%1 y:%2").arg(Units::siToDisplayDistance(realpt.x())).arg(Units::siToDisplayGrav(realpt.y()));
		int ei=0;
		int mi=0;
		if(observationset) {
			double empdist = closestEmpiricalPoint(p,ei);
			double moddist = closestModeledPoint(p,mi);
			if(empdist<6 || moddist<6) {
				lines << QString("Data at (%1, %2, %3): ");
				Point realgeopoint;
				if(empdist<6) {
					realgeopoint = observationset->getPoints()[ei];
					lines << QString("  Emp:%1").arg(Units::siToDisplayGrav(observationset->getEmpGravityVals()[ei]));
					QPointF center(getGraphPoint(xmod[ei], yemp[ei]));
					painter.fillRect(center.x()-1,center.y()-1,3,3, Qt::red);
				}
				if(moddist<6) {
					realgeopoint = observationset->getPoints()[mi];
					lines << QString("  Mod:%1").arg(Units::siToDisplayGrav(observationset->getOffsetGravityEffect(mi)));
					QPointF center(getGraphPoint(xmod[mi], ymod[mi]));
					painter.fillRect(center.x()-1,center.y()-1,3,3, Qt::red);
				}
				lines[1] = lines[1].arg(Units::siToDisplayDistance(realgeopoint.x)).arg(Units::siToDisplayDistance(realgeopoint.y)).arg(Units::siToDisplayDistance(realgeopoint.z));
			}
		}
		int h = (fm.height()+1)*lines.size();
		int w = 0;
		for(int i=0; i!=lines.size(); ++i) {
            w = std::max(w, fm.width(lines[i]))+1;
		}
		int xoffset = (p.x()>width()-w-20)?(-w-14):14;
		int yoffset = (p.y()>height()-h-2)?(-h):0;
		QRect box(p+QPoint(xoffset-1,yoffset),QSize(w+1,h));
	
		painter.setPen(Qt::black);
		painter.setBrush(QColor(253,255,211));
		painter.drawRect(box);
		for(int i=0; i!=lines.size(); ++i) {
            painter.drawText(QRect(p+QPoint(xoffset,yoffset+i*(fm.height()+1)),QSize(w+1,fm.height()+1)),lines[i]);
		}
	}

	infobox = newinfobox;
	update();
}
QList<QPointF> ProfileGraph::getEmpiricalGraphPoints() {
	QList<QPointF> eps;
    for(uint i=0; i!=xmod.size(); ++i) {
		if(!isNoData(yemp[i])){
			eps.append(getGraphPoint(xmod[i], yemp[i]));
		}
	}
	return eps;
}
QList<QPointF> ProfileGraph::getModeledGraphPoints() {
	QList<QPointF> mps;
    for(uint i=0; i!=xmod.size(); ++i) {
		if(!isNoData(ymod[i])){
			mps.append(getGraphPoint(xmod[i], ymod[i]));
		}
	}
	return mps;
}
double ProfileGraph::closestEmpiricalPoint(QPoint p, int& ei) {
	if(observationset) {
		auto eps = getEmpiricalGraphPoints();
		int j=0;
		auto evals = observationset->getEmpGravityVals();
		int bestindex = 0;
		double bestdist = 100000000;
        for(uint i=0; i!=evals.size(); ++i) {
			if(!isNoData(evals[i])) {
				double dist = sqrt(pow(p.x()-eps[j].x(),2) + pow(p.y()-eps[j].y(),2));
				if(dist<bestdist) {
					bestindex = i;
					bestdist = dist;
				}
				++j;
			}
		}
		ei = bestindex;
		return bestdist;
	} else {
		return 10000000;
	}
}
double ProfileGraph::closestModeledPoint(QPoint p, int& mi) {
	auto mps = getModeledGraphPoints();
	int bestindex = 0;
	double bestdist = 100000000;
	for(int i=0; i!=mps.size(); ++i) {
		double dist = sqrt(pow(p.x()-mps[i].x(),2) + pow(p.y()-mps[i].y(),2));
		if(dist<bestdist) {
			bestindex = i;
			bestdist = dist;
		}
	}
	mi = bestindex;
	return bestdist;
}
QVector<QLineF> ProfileGraph::linesFromPoints(std::vector<double> nx, std::vector<double> ny) {
	QVector<QLineF> lns;
	std::vector<double> ndx;//nx with nodata y's removed
	std::vector<double> ndy;//ny with nodata y's removed
    for(uint i=0; i!=nx.size(); ++i) {
		if(!isNoData(ny[i])) {
			ndx.push_back(nx[i]);
			ndy.push_back(ny[i]);
		}
	}
	if(ndx.size()>1) {
        for(uint i=0; i!=ndx.size()-1; ++i) {
			lns.push_back(QLineF(getGraphPoint(ndx[i],ndy[i]), getGraphPoint(ndx[i+1],ndy[i+1])));
		}
	}
	return lns;
}

QPointF ProfileGraph::getGraphPoint(double nx, double ny) {
	double gx = (nx-xmin)/(xmax-xmin)*(width()-leftmargin-rightmargin)+leftmargin;
	double gy = height() - bottommargin - ((ny-ymin)/(ymax-ymin)*(height()-topmargin-bottommargin));
	return QPointF(gx,gy);
}
QPointF ProfileGraph::getRealXYFromGraphPoint(QPoint p) {
	double xfrac = (double)(p.x()-leftmargin)/(double)(width()-leftmargin-rightmargin);
	double yfrac = (double)(height()-p.y()-bottommargin)/(double)(height()-bottommargin-topmargin);
	return QPointF(xmin+xfrac*(xmax-xmin), ymin+yfrac*(ymax-ymin));
}

double ProfileGraph::getXTickInterval() {
	return xtickinterval;
}
