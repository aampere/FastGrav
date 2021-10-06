#ifndef EDITCYLINDERUI_H
#define EDITCYLINDERUI_H

#include <QDialog>
#include "ui_editcylinderitemui.h"
class CylinderItem;
class MainWindow;

class EditCylinderItemUI : public QDialog {
	Q_OBJECT

	CylinderItem* ci;
	MainWindow* mainwindow;
	QColor color;

public:
	EditCylinderItemUI(QWidget* parent, CylinderItem* c, MainWindow* mw);

	Ui::EditCylinderUIClass ui;
public Q_SLOTS:
	void selectColorSlot();
	void snapAzimuthSlot();
	void applySlot();

};

#endif