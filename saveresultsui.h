#ifndef SAVERESULTSUI_H
#define SAVERESULTSUI_H

#include <QDialog>
#include <QFileDialog>
#include "ui_saveresultsui.h"
#include "observationset.h"

class SaveResultsUI : public QDialog {
	Q_OBJECT

	ObservationSet* observationset;

	QString previewtext;

public:
	SaveResultsUI(QWidget* parent, ObservationSet* os);

	Ui::SaveResultsUIClass ui;
public Q_SLOTS:
	void updatePreview();
	void saveSlot();
};

#endif // SAVERESULTSUI_H
