#ifndef EDITPOLYGONPRISMITEMUI_H
#define EDITPOLYGONPRISMITEMUI_H

#include <QDialog>
#include "ui_editpolygonprismitemui.h"
class PolygonPrismItem;
class MainWindow;

class EditPolygonPrismItemUI : public QDialog {
	Q_OBJECT

	PolygonPrismItem* ppi;
	MainWindow* mainwindow;
	QColor color;

public:
	EditPolygonPrismItemUI(QWidget* parent, PolygonPrismItem* ppi, MainWindow* mw);

	Ui::EditPolygonPrismUIClass ui;
public Q_SLOTS:
	void validateCell(int i, int j);
	void addVertex();
	void deleteRows();

	void selectColorSlot();
	void snapAzimuthSlot();				//Pressing the ... button by the azimuth field triggers this.
	void applySlot();

};

#endif // EDITPOLYGONPRISMITEMUI_H
