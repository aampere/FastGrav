#include "observationsettable.h"
#include "observationset.h"

ObservationSetTable::ObservationSetTable(QWidget *parent)
: QWidget(parent), os(nullptr)
{
	ui.setupUi(this);
}
ObservationSetTable::~ObservationSetTable() {
	if(os) {
		os->setTable(nullptr, nullptr);
	}
}

void ObservationSetTable::setTitleLabel(QString tl) {
	ui.titlelabel->setText(tl);
}

void ObservationSetTable::clearContents() {
	ui.table->clearContents();
}
void ObservationSetTable::setColumnHeaders(QStringList headers) {
	ui.table->setHorizontalHeaderLabels(headers);
}
void ObservationSetTable::setRowColumnCount(int r, int c) {
	ui.table->setRowCount(r);
	ui.table->setColumnCount(c);
}
void ObservationSetTable::setItem(int r, int c, QTableWidgetItem* twi) {
	ui.table->setItem(r,c,twi);
}

void ObservationSetTable::setObservationSet(ObservationSet* o) {
	os = o;
}