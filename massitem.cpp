#include "massitem.h"
#include "mainwindow.h"

MassItem::MassItem(MainWindow* mw)
: mainwindow(mw), constG(6.67408e-11), pi(3.1415926535897932384626433832795),
  density(0.0), active(true), visible(true), color(170,170,170), listwidgetitem(nullptr) {
	
} 

MassItem::~MassItem()  {

}

void MassItem::effectChanged() {
	if(mainwindow) {
		mainwindow->massItemHasChanged(this);//mainwindow then tells all ObservationSet*s to update, as well as all CrossSection*s
	}
}

void MassItem::setActive(bool a) {
	active = a;
	if(listwidgetitem)
		listwidgetitem->setText(getFullLabel());
	effectChanged();
}
bool MassItem::isActive() {
	return active;
}
void MassItem::setVisible(bool v) {
	visible = v;
	effectChanged(); //effect didn't eally change, but this will force update throughout program
}
bool MassItem::isVisible() {
	return visible;
}

QString MassItem::getName() {
	return name;
}
double MassItem::getDensity() {
	return density;
}
QString MassItem::getType() {
	return type;
}
void MassItem::setColor(QColor col) {
	color = col;
	if(mainwindow)
		mainwindow->massItemNameHasChanged(this);
}
QColor MassItem::getColor() {
	return color;
}

void MassItem::setName(QString n) {
	name = n;
	if(listwidgetitem)
		listwidgetitem->setText(getFullLabel());
	if(mainwindow)
		mainwindow->massItemNameHasChanged(this);//mainwindow tells all crossSections to redraw, which fixes all names. A system like this might be better for the listwidgetitem mechanic... :|
}
void MassItem::setDensity(double d) {
	density = d;
	effectChanged();
}

QString MassItem::getFullLabel() {
	QString activemark = isActive()?QString((QChar) 0x2611):QString((QChar) 0x2610);//open/checked checkbox
	return activemark + " " +getName();
}
QListWidgetItem* MassItem::listWidgetItem() {
	return listwidgetitem;
}
void MassItem::setListWidgetItem(QListWidgetItem* lwi) {
	listwidgetitem = lwi;
}
