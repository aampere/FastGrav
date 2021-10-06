#ifndef OBSERVATIONSETTABLE_H
#define OBSERVATIONSETTABLE_H

#include "ui_observationsettableui.h"
class ObservationSet;

class ObservationSetTable : public QWidget
{
	Q_OBJECT

	ObservationSet* os;

public:
	ObservationSetTable(QWidget *parent=0);
	~ObservationSetTable();
	
	void setTitleLabel(QString tl);

	void clearContents();
	void setColumnHeaders(QStringList headers);
	void setRowColumnCount(int r, int c);
	void setItem(int r, int c, QTableWidgetItem* twi);

	void setObservationSet(ObservationSet* o);


private:
	Ui::ObservationSetTableClass ui;
};

#endif // OBSERVATIONSETTABLE_H