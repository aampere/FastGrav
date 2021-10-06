#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMdiArea>
#include <QDockWidget>
#include <QListWidget>
#include <QListView>
#include <QFileDialog>
#include <QLineEdit>
#include <QColorDialog>
#include <QToolBar>
#include "ui_createmassitemui.h"
#include "ui_editsphereitemui.h"
#include "ui_editcylinderitemui.h"
#include "ui_editclouditemui.h"
#include "ui_editpolygonprismitemui.h"
#include "ui_unitsui.h"
#include "ui_infoui.h"
#include "saveresultsui.h"
#include <qmdisubwindow.h>
#include <QSettings>
#include "massitem.h"
#include "sphereitem.h"
#include "cylinderitem.h"
#include "clouditem.h"
#include "polygonprismitem.h"
#include "observationset.h"
#include "observationsettable.h"
#include "crosssection.h"
#include "massindexmodel.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QMdiArea workspace;

	QListView massindex;										//Docking list widget with list items corresponding to MassItem*s.
	MassIndexModel massindexmodel;
	//StringListModel massindexmodel;
	std::vector<MassItem*> massitems;							//Master list of MassItem*s.

	QString currentprojectfile;									//the filename of the project that is open. If the Save As function is used, the project will be saved to this file. "" if no project is open (an unsaved new project)
	//QDir currentdirectory;										//Right now this is ALWAYS the directory that contains the project file. Relative paths are with respect to this directory

	QListWidget obssetindex;									//Docking list widget with list items corresponding to ObservationSet*s.
	std::vector<ObservationSet*> observationsets;				//Master list of ObservationSet*s.
	std::map<QListWidgetItem*, ObservationSet*> listitemobsmap;	//Maps a list item to its corresponding ObservationSet*.

	std::vector<CrossSection*> crosssections;					//List of CrossSections, which are contained in QMdiWindows.

	QColorDialog* colordialog;									//Lame way to always a have a Color Dialog. The selected color is stored in selectedcolor
	QColor* selectedcolor;										//stores the colordialog's selected color.
	QPushButton* selectedcolorbutton;							//Pointer to the color button's icon. Very lame implementation.

	QToolBar toolbar;
	
	ObservationSet* anchorset;									//if offsettype is Anchor, this points to the specified observationset for anchoring...
	int anchorindex;											// ...and this is the index for the anchor point in that observationset
	double manualgravityoffset;
	double gravityoffset;										//This is a constant that may be added to all calculated gravity effects for this observationset. It helps the user "line up" their empirical profile with the calculated anomaly.

public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
	void closeEvent(QCloseEvent *event);

	std::vector<MassItem*> massItems();
	std::vector<ObservationSet*> observationSets();
	double getTotalGravityEffect(Point p);		//Gets total gravitational effect of all MassItem*s as observed from Point p

	void clearApplication();					//Removes everything and totally resets the MainWindow
	void setCurrentProjectFile(QString f);
	void setSaved(bool s);
	void updateMainWindowTitle();
	bool saved;

	void saveToFile(QString fname);				//Writes all the info needed to recreate the project into the specified filename.
	void openFromFile(QString fname);			//Parses a saved "Project file" to recreate a project.
	
	enum Offsettype{Manual, Anchor, BestFit};	//Determines how the gravityoffset will be calculated. Manual=user enters an offset. Anchor=user specifies a specific observation point that will always be equal to its corresponding calculated point, BestFit=gravityoffset automatically determined by least squares.
	Offsettype offsettype;

public Q_SLOTS:
	void newSlot();								//Triggered from main menu. Prompts a save, then clears the application completely (effectively beginning a new project)
	void saveSlot();							//Triggered from main menu. Saves the project to the currentprojectfile filename, or calls saveAsSlot() if the project is new and unsaved.
	void saveAsSlot();							//Triggered from main menu. Opens QFileDialog then calls saveToFile( ).
	void openSlot();							//Triggered from main menu. Opens QFileDialog then calls openFromFile( ).
	void selectColorSlot();
	QString getCurrentProjectFile();

	void massItemHasChanged(MassItem* mi);
	void massItemNameHasChanged(MassItem* mi);
	void pushUiGravityUpdate();

	CrossSection* addCrossSectionSlot();
	void removeCrossSection(CrossSection* cs);

	void addMassItem(MassItem* mi);				//Adds an already extant MassItem* to the MassItem* vector, and creates the corresponding QListWidgetItem.
	void createMassItemSlot();					//Triggered from Menu->Add->Mass Item. Opens dialog with options for what type of mass to create (Sphere, Point, slab, etc).
	void createSphereItemSlot();				//Triggered from the Create Mass Item Dialog. Creates a SphereItem*, calls addMassItem( ) on it, then calls editSphereItem( ) on it.
	void createCylinderItemSlot();
	void createCloudItemSlot();
	void createPolygonPrismItemSlot();
	void editMassItemSlot();					//Triggered by right-click->Edit on massindex. Calls the appropriate editor function for the selected MassItem.
	void editSphereItemSlot(SphereItem* si);	//Triggered from right-click in the massindex, or when a SphereItem is created. Opens a dialog that edits the Sphere's parameters.
	void editCylinderItemSlot(CylinderItem* ci);
	void editCloudItemSlot(CloudItem* cli);
	void editPolygonPrismItemSlot(PolygonPrismItem* ppi);
	void removeMassItemSlot();
	void sendMassItemUpSlot();
	void sendMassItemDownSlot();
    void duplicateMassItemSlot();

	void addObservationSet(ObservationSet* os);	//Adds an already extant ObervationSet* to the ObservationSet* vector, and creates the corresponding QListWidgetItem
	void createObservationSetManualSlot();		//Non-functional. It would create an empty Observation set, then open an editor immediately.
	void createObservationSetFileSlot();		//Triggered from Menu->Add->Observation Set->From file. Opens a QFileDialog, creates an ObservationSet from the selected file, and calls addObservationSet( ) on it. 
	void reloadObservationSetSlot();			//Re-reads the source file. Triggered from right-click->Reload from obssetindex
	void removeObservationSetSlot();			//Removes ("deletes") an observation set from the index.  Triggered by right-click->Remove on obssetindex.
	void openObservationSetTable();
	void calculateObservationSetGravityEffects();

	void setOffsetType(Offsettype ot);
	void setOffsetAnchor(ObservationSet* os, int eindex);
	void openOffsetDialog();
	void calculateGravityOffset();
	double getGravityOffset();

	void openUnitsDialog();
	void helpSlot();

	void outputGravityResultsFromIndex();			//Triggered by right-click on observations set. Outputs simple gravity results to gravityresults.txt.
	void saveGravityResultsFor(ObservationSet* os);

private:
	//Ui::mainwindowClass ui;					//ui for the MainWindow
	Ui::CreateMassUIClass createmassui;			//ui for the Add->Mass Item dialog
	Ui::EditSphereUIClass editsphereui;			//ui for the SphereItem edit dialog
	Ui::EditCloudUIClass editcloudui;			//ui for the CloudItem edit dialog
	Ui::UnitsUIClass unitsui;					//ui for the unit selection dialog
	Ui::InfoUIClass infoui;						//ui for the help/about dialog
};

#endif // MAINWINDOW_H
