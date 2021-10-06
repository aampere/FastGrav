#ifndef EDITCLOUDITEMUI_H
#define EDITCLOUDITEMUI_H

#include <QDialog>
#include "ui_editclouditemui.h"
class MainWindow;
class CloudItem;

class EditCloudItemUI : public QDialog {
	Q_OBJECT

	CloudItem* cli;
	MainWindow* mainwindow;
	QColor color;

public:
	EditCloudItemUI(QWidget* parent, CloudItem* c, MainWindow* mw);

	Ui::EditCloudUIClass ui;
public Q_SLOTS:
	void selectColorSlot();
	void setSourceSlot();
	void applySlot();

};

#endif