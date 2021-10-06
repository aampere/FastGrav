#include "mainwindow.h"
#include "massitem.h"
#include "editpolygonprismitemui.h"
#include "editcylinderitemui.h"
#include "editclouditemui.h"
#include "offsetui.h"
#include <QFileDialog>
#include <QCloseEvent>
#include <QShortcut>
#include <QDir>
#include <QtDebug>
#include <qmessagebox.h>
#include "massindexitemdelegate.h"
#include "units.h"
#include "logconsole.h"
#include "global.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    massindexmodel(massitems,0), colordialog(new QColorDialog()),
    toolbar("Toolbar", this),saved(true)
{
	//ui.setupUi(this);
	setCentralWidget(&workspace);
	setWindowTitle("FastGrav!");
    setWindowIcon(QIcon(":/smoothicons2.ico"));

	//Units
	Units::setGravityDisplayUnit(Units::MGal);

	setDockNestingEnabled(true);
	//mass index below
	massindex.setModel(&massindexmodel);
	massindex.setItemDelegate(new MassIndexItemDelegate());
	QDockWidget *massdockwidget = new QDockWidget("Mass Index", this);
	massdockwidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	massdockwidget->setWidget(&massindex);
	addDockWidget(Qt::LeftDockWidgetArea, massdockwidget);
	//massindex.setDragDropMode(QAbstractItemView::DragDrop);
	//massindex.setDefaultDropAction(Qt::MoveAction);
	massindex.setViewMode(QListView::ListMode);
	massindex.setContextMenuPolicy(Qt::ActionsContextMenu);
	QAction* edititemaction = new QAction("Edit...", &massindex);
	connect(edititemaction, SIGNAL(triggered()), this, SLOT(editMassItemSlot()));
	connect(&massindex, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(editMassItemSlot()));
	QAction* removeitemaction = new QAction("Remove", &massindex);
	connect(removeitemaction, SIGNAL(triggered()), this, SLOT(removeMassItemSlot()));
    QAction* moveitemupaction = new QAction("Move item up", &massindex);
    connect(moveitemupaction, SIGNAL(triggered()), this, SLOT(sendMassItemUpSlot()));
    QAction* moveitemdownaction = new QAction("Move item down", &massindex);
    connect(moveitemdownaction, SIGNAL(triggered()), this, SLOT(sendMassItemDownSlot()));
    QAction* duplicateitemaction = new QAction("Duplicate item", &massindex);
    connect(duplicateitemaction, SIGNAL(triggered()), this, SLOT(duplicateMassItemSlot()));
	massindex.addAction(edititemaction);
	massindex.addAction(removeitemaction);
    massindex.addAction(moveitemupaction);
    massindex.addAction(moveitemdownaction);
    massindex.addAction(duplicateitemaction);
	//mass index above

	//observation set index below
	QDockWidget *obsdockwidget = new QDockWidget("Observation Set Index", this);
	obsdockwidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	obsdockwidget->setWidget(&obssetindex);
	addDockWidget(Qt::LeftDockWidgetArea, obsdockwidget);
	obssetindex.setDragDropMode(QAbstractItemView::DragDrop);
	obssetindex.setDefaultDropAction(Qt::MoveAction);
	obssetindex.setContextMenuPolicy(Qt::ActionsContextMenu);
	QAction* reloadobsaction = new QAction("Reload", &obssetindex);
	connect(reloadobsaction, SIGNAL(triggered()), this, SLOT(reloadObservationSetSlot()));
	QAction* removeobsaction = new QAction("Remove", &obssetindex);
	connect(removeobsaction, SIGNAL(triggered()), this, SLOT(removeObservationSetSlot()));
	QAction* openobservationsettable = new QAction("Open table", &obssetindex);
	connect(openobservationsettable, SIGNAL(triggered()), this, SLOT(openObservationSetTable()));
	QAction* calculategravity = new QAction("Recalculate gravity effects", &obssetindex);
	connect(calculategravity, SIGNAL(triggered()), this, SLOT(calculateObservationSetGravityEffects()));
	QAction* outputresultsaction = new QAction("Save results as...", &obssetindex);
	connect(outputresultsaction, SIGNAL(triggered()), this, SLOT(outputGravityResultsFromIndex()));
	obssetindex.addAction(reloadobsaction);
	obssetindex.addAction(removeobsaction);
	obssetindex.addAction(openobservationsettable);
	//obssetindex.addAction(calculategravity);
	obssetindex.addAction(outputresultsaction);
	//observation set index above

	//log console below
	QTextEdit* console = new QTextEdit("FastGrav. Beginning log.\n");
	console->setReadOnly(true);
	console->setFont(QFont("Courier"));
	console->setTabStopWidth(65);
	console->setWordWrapMode(QTextOption::NoWrap);
	QDockWidget *consoledockwidget = new QDockWidget("Log", this);
	consoledockwidget->setWidget(console);
	addDockWidget(Qt::BottomDockWidgetArea, consoledockwidget);
	consoledockwidget->setVisible(false);
	LogConsole::globalconsole = console;
	//log console above

	QMenu *fileMenu = new QMenu("&File",this);
	fileMenu->addAction("&New Project", this, SLOT(newSlot()),QKeySequence(QKeySequence::New));
	fileMenu->addAction("&Save Project", this, SLOT(saveSlot()),QKeySequence(QKeySequence::Save));
	fileMenu->addAction("S&ave Project as...", this, SLOT(saveAsSlot()),QKeySequence(QKeySequence::SaveAs));
	fileMenu->addAction("&Open Project", this, SLOT(openSlot()),QKeySequence(QKeySequence::Open));
	fileMenu->addAction("&Quit", this, SLOT(close()),QKeySequence(QKeySequence::Quit));
	menuBar()->addMenu(fileMenu);

	QMenu *modelMenu = new QMenu("&Model",this);
	modelMenu->addAction("&Offset options...", this, SLOT(openOffsetDialog()));
	modelMenu->addAction("&Display units...", this, SLOT(openUnitsDialog()));
	menuBar()->addMenu(modelMenu);

	QMenu *addMenu = new QMenu("&Add",this); 
	addMenu->addAction("&Mass Item", this,SLOT(createMassItemSlot()));
	addMenu->addAction("&Observation Set", this,SLOT(createObservationSetFileSlot()));
	addMenu->addAction("&Cross Section", this,SLOT(addCrossSectionSlot()));
	menuBar()->addMenu(addMenu);

	QMenu *helpMenu = new QMenu("&Help", this);
	helpMenu->addAction("&Help/About", this,SLOT(helpSlot()));
	menuBar()->addMenu(helpMenu);

	offsettype = BestFit;
	manualgravityoffset = 0.0;
	gravityoffset = 0.0; 
	
	toolbar.addAction(QIcon(":/new"),"New Project",this, SLOT(newSlot()));
	toolbar.addAction(QIcon(":/save"),"Save Project",this, SLOT(saveSlot()));
	toolbar.addAction(QIcon(":/open"),"Open Project",this, SLOT(openSlot()));
	toolbar.addSeparator();
	toolbar.addAction(QIcon(":/units"),"Display Units",this, SLOT(openUnitsDialog()));
	toolbar.addAction(QIcon(":/offset"),"Offset Options",this, SLOT(openOffsetDialog()));
	toolbar.addSeparator();
	toolbar.addAction(QIcon(":/crosssection"),"Add Cross Section/Profile",this, SLOT(addCrossSectionSlot()));
	toolbar.addAction(QIcon(":/observationset"),"Add Observation Set",this, SLOT(createObservationSetFileSlot()));
	toolbar.addSeparator();
	toolbar.addAction(QIcon(":/sphere24.1"),"Add Sphere",this, SLOT(createSphereItemSlot()));
	toolbar.addAction(QIcon(":/cylinder24.1"),"Add Cylinder",this, SLOT(createCylinderItemSlot()));
	toolbar.addAction(QIcon(":/prism24.1"),"Add Polygon Prism",this, SLOT(createPolygonPrismItemSlot()));
	toolbar.addAction(QIcon(":/cloud24.1"),"Add Point Cloud",this, SLOT(createCloudItemSlot()));
	addToolBar(Qt::LeftToolBarArea, &toolbar);
	//toolbar.setDo

	//Settings setup
	workspace.setObjectName("workspace");
	massdockwidget->setObjectName("massdockwidget");
	obsdockwidget->setObjectName("obsdockwidget");
	consoledockwidget->setObjectName("consoledockwidget");
	toolbar.setObjectName("toolbar");

	setSaved(true);
	setCurrentProjectFile("");
	calculateGravityOffset();
}
MainWindow::~MainWindow()
{

}
void MainWindow::closeEvent(QCloseEvent *event){
	if(!saved) {
		QMessageBox msgBox(this);
		msgBox.setModal(true);
		msgBox.setText("You have unsaved work.");
		msgBox.setInformativeText("Do you want to save your project before exiting?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		switch (ret) {
		case QMessageBox::Save:
		    saveSlot();
		    event->accept();
		    break;
		case QMessageBox::Discard:
		    event->accept();
		    break;
		case QMessageBox::Cancel:
		    event->ignore();
		    break;
		default:
		    event->ignore();
		    break;
		}
	} else {
		event->accept();
	}
}

std::vector<MassItem*> MainWindow::massItems() {
	return massitems;
}
std::vector<ObservationSet*> MainWindow::observationSets() {
	return observationsets;
}
double MainWindow::getTotalGravityEffect(Point p) {
	std::vector<double> results;
	for(auto mi=massitems.begin(); mi!=massitems.end(); ++mi) {
		results.push_back((*mi)->getGravityEffect(p));
	}
	//I'm not sure if this is necessary, but below I sort the individual gravity effects, and sum them up from lowest to highest.
	//This could be slightly more precise than summing them in arbitrary order, if a many very very small doubles are added to a large double.
	std::sort(results.begin(), results.end());
	double result = 0.0;
	for(auto r=results.begin(); r!=results.end(); ++r) {
		result += *r;
	}
	return result;
}

void MainWindow::clearApplication() {
	for(auto c=crosssections.begin(); c!=crosssections.end(); ++c) {
		(*c)->closeWindow();
	}
	crosssections.clear();

	for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
		(*os)->closeTable();
		delete (*os);
	}
	observationsets.clear();
	listitemobsmap.clear();
	obssetindex.clear();

	massindexmodel.removeRows(0, massindexmodel.rowCount());
	for(auto mi=massitems.begin(); mi!=massitems.end(); ++mi) {
		delete (*mi);
	}
	massitems.clear();

	setSaved(true);setCurrentProjectFile("");
}
void MainWindow::setCurrentProjectFile(QString f) {
	currentprojectfile = f;
	if(currentprojectfile=="") {
		//I don't think I really need to do anything here. The current directory is fine, whatever it is, because the currentprojectfile being "" is enough to know there is no project.
	} else {
		if(! QDir::setCurrent(QFileInfo(currentprojectfile).absoluteDir().absolutePath()) )
			logline("QDir failed to change the current directory.");
	}
	updateMainWindowTitle();
}
void MainWindow::setSaved(bool s) {
	saved = s;
	updateMainWindowTitle();
}
void MainWindow::updateMainWindowTitle() {
	setWindowTitle(QString("FastGrav: %0%1").arg(saved?"":"*").arg((currentprojectfile=="")?"New Project":currentprojectfile));
}

void MainWindow::newSlot() {
	if(!saved) {
		QMessageBox msgBox(this);
		msgBox.setText("You have unsaved work.");
		msgBox.setInformativeText("Do you want to save your changes before starting a new project?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		switch (ret) {
		case QMessageBox::Save:
		    saveSlot();
		    clearApplication();
		    break;
		case QMessageBox::Discard:
		    clearApplication();
		    break;
		case QMessageBox::Cancel:
		    // Cancel was clicked
		    break;
		default:
		    // should never be reached
		    break;
		}
	} else {
		clearApplication();
	}
}
void MainWindow::saveSlot() {
	if(currentprojectfile=="") {
		//The user has not loaded a project, and it trying to save an all new project, ratehr than overwriting an existing one.
		saveAsSlot();
	} else {
		//The user has already saved a file, so overwrite the project to that file.
		saveToFile(currentprojectfile);
	}
}
void MainWindow::saveAsSlot() {
	QFileDialog dialog(this,"Save Project As...");
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setLabelText(QFileDialog::Accept,"Save");
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setNameFilter("Project files (*.gravproj)");
	if( dialog.exec() ) { 
		QString fileName = dialog.selectedFiles().first();
		if(!fileName.endsWith(".gravproj")) fileName = fileName+".gravproj";
		saveToFile(fileName);
	}
}
void MainWindow::saveToFile(QString fname) {
	QFile file(fname);
	if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		//Write "header"
		out << "HEADER\n";
		out << "displayunits\t" << Units::displaygravunit << "\t" << Units::displaydistanceunit << "\t" << Units::displaydensityunit << "\t" << Units::displaymassunit << "\n";
        uint i=0;
		for(i=0; i<observationsets.size(); ++i) {
			if(anchorset==observationsets[i]) {
				break;
			}
		}
		out << "offset\t" << offsettype << "\t" << i << "\t" << anchorindex << "\t" << manualgravityoffset << "\n";
		//Now write the MassItems one-by-one. This is enough to recreate them; no auxiliary files are needed here (except for DEM-bounded solids, or other 3D bodies)
		out << "MASS\n";
		for(auto mi=massitems.begin(); mi!=massitems.end(); ++mi) {
			if(SphereItem* si = dynamic_cast<SphereItem*>(*mi)) {
				out<<"SphereItem\t"
					<< si->getName() << "\t"
					<< ((si->isActive())?"true":"false") << "\t"
					<< ((si->isVisible())?"true":"false") << "\t"
					<< si->getColor().name() << "\t"
					<< QString::number(si->getDensity(),'g',12) << "\t"
					<< QString::number(si->getCenter().x,'g',12) << "\t" 
					<< QString::number(si->getCenter().y,'g',12) << "\t"
					<< QString::number(si->getCenter().z,'g',12) << "\t"
					<< QString::number(si->getRadius(),'g',12) << "\n";
			} else if(CylinderItem* ci = dynamic_cast<CylinderItem*>(*mi)) {
				out<<"CylinderItem\t"
					<< ci->getName()<< "\t"
					<< ((ci->isActive())?"true":"false") << "\t"
					<< ((ci->isVisible())?"true":"false") << "\t"
					<< ci->getColor().name() << "\t"
					<< QString::number(ci->getDensity(),'g',12) << "\t"
					<< QString::number(ci->getCenter().x,'g',12) << "\t"
					<< QString::number(ci->getCenter().y,'g',12) << "\t"
					<< QString::number(ci->getCenter().z,'g',12) << "\t"
					<< QString::number(ci->getAzimuth(),'g',12) << "\t"
					<< QString::number(ci->getRadius(),'g',12) << "\n";
			} else if(CloudItem* cli = dynamic_cast<CloudItem*>(*mi)) {
				out<<"CloudItem\t"
					<< cli->getName()<< "\t"
					<< ((cli->isActive())?"true":"false") << "\t"
					<< ((cli->isVisible())?"true":"false") << "\t"
					<< cli->getColor().name() << "\t"
					<< cli->getSource() << "\t"
					<< cli->getDistanceUnit() << "\t"
					<< QString::number(cli->getPointmass(),'g',12) << "\t"
					<< QString::number(cli->getCenter().x,'g',12) << "\t"
					<< QString::number(cli->getCenter().y,'g',12) << "\t"
					<< QString::number(cli->getCenter().z,'g',12) << "\n";
			} else if(PolygonPrismItem* ppi = dynamic_cast<PolygonPrismItem*>(*mi)) {
				out<<"PolygonPrismItem\t"
					<< ppi->getName()<< "\t"
					<< ((ppi->isActive())?"true":"false") << "\t"
					<< ((ppi->isVisible())?"true":"false") << "\t"
					<< ppi->getColor().name() << "\t"
					<< QString::number(ppi->getDensity(),'g',12) << "\t"
					<< QString::number(ppi->getCenter().x,'g',12) << "\t"
					<< QString::number(ppi->getCenter().y,'g',12) << "\t"
					<< QString::number(ppi->getCenter().z,'g',12) << "\t"
					<< QString::number(ppi->getAzimuth(),'g',12) << "\n";
				auto vertices = ppi->getVertices();
                for(uint i=0; i!=vertices.size(); ++i) {
					out <<"vertex\t"<< QString::number(vertices[i].x,'g',12) << "\t"<< QString::number(vertices[i].y,'g',12)<<"\n";
				}
				out << "endvertices\n";
			}
			//else if cast other item...
			//else if cast other item...
			//else if cast other item...
		}
		//Now write the ObservationSets one-by-one
		out << "OBSERVATION SETS\n";
		for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
			out<<"ObservationSet\t"
				<<(*os)->getName() << "\t"
				<<(*os)->getSourceFile() << "\t"
				<<(*os)->gravityu << "\t"
				<<(*os)->distanceu << "\n";
		}
		//Now write the CrossSections one-by-one
		out << "CROSS SECTIONS\n";
		for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
			out<<"CrossSection\t"
				<<(*cs)->getObservationSetIndex() << "\t"
				<<((*cs)->getAutomaticCrossBounds()?"true":"false") << "\t"
				<<(*cs)->getX1() << "\t"
				<<(*cs)->getX2() << "\t"
				<<(*cs)->getY1() << "\t"
				<<(*cs)->getY2() << "\t"
				<<((*cs)->isYAxisAutomatic()?"true":"false") << "\t"
				<<(*cs)->getYLow() << "\t"
				<<(*cs)->getYHigh() << "\t"
				<<(*cs)->getYTick0() << "\t"
				<<(*cs)->getYTickInterval() << "\t"
				<<(*cs)->getXMin() << "\t"
				<<(*cs)->getXMax() << "\t"
				<<(*cs)->getRealZOffset() << "\n";
		}
		out << "END\n";
		file.close();
		setSaved(true);
		setCurrentProjectFile(fname);
		QSettings mwsettings(QDir::current().filePath(QFileInfo(currentprojectfile).baseName()+".ini"), QSettings::IniFormat); //Recall that .ini is Windows-specific, I think
		mwsettings.setValue("mwsize", size());
		mwsettings.setValue("mwpos", pos());
		mwsettings.setValue("mwstate", saveState());
        for(uint i=0; i!=crosssections.size(); ++i) {
			mwsettings.setValue(QString("crosssection%1geometry").arg(i),crosssections[i]->getWindow()->saveGeometry());
			mwsettings.setValue(QString("crosssection%1splitterstate").arg(i),crosssections[i]->saveSplitter());
		}
	} else {
		logline(QString("Could not save project file to %1").arg(fname));
	}
}
void MainWindow::openSlot() {
	QFileDialog dialog(this,"Open Project");
	if(!saved) {
		QMessageBox msgBox(this);
		msgBox.setText("You have unsaved work.");
		msgBox.setInformativeText("Do you want to save your work before opening another project?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		switch (ret) {
		case QMessageBox::Save:
			saveSlot();
			clearApplication();
			dialog.setFileMode(QFileDialog::ExistingFile);
			dialog.setNameFilter("Project files (*.gravproj)");
			if( dialog.exec() ) { 
				QString fileName = dialog.selectedFiles().first();
				openFromFile(fileName);
			}
			break;
		case QMessageBox::Discard:
			clearApplication();
			dialog.setFileMode(QFileDialog::ExistingFile);
			dialog.setNameFilter("Project files (*.gravproj)");
			if( dialog.exec() ) { 
				QString fileName = dialog.selectedFiles().first();
				openFromFile(fileName);
			}
			break;
		case QMessageBox::Cancel:
			// Cancel was clicked
			break;
		default:
			// should never be reached
			break;
		}
	} else {
		clearApplication();
		dialog.setFileMode(QFileDialog::ExistingFile);
			dialog.setNameFilter("Project files (*.gravproj)");
		if( dialog.exec() ) { 
			QString fileName = dialog.selectedFiles().first();
			openFromFile(fileName);
		}
	}
}
void MainWindow::openFromFile(QString fname) {
	QFile file(fname);
	if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		setCurrentProjectFile(fname);
		QSettings mwsettings(QDir::current().filePath(QFileInfo(currentprojectfile).baseName()+".ini"), QSettings::IniFormat);
		if(QFile::exists(QDir::current().filePath(QFileInfo(currentprojectfile).baseName()+".ini"))){ //Just for restoring the mainwindow settings, check if the settings file exists, because if it doesn't, the mainwindow restores very strangely. 
			if(mwsettings.contains("mwsize")) {
				resize(mwsettings.value("mwsize").toSize());
			}
			if(mwsettings.contains("mwpos")) {
				move(mwsettings.value("mwpos").toPoint());
			}
			if(mwsettings.contains("mwstate")) {
				restoreState(mwsettings.value("mwstate").toByteArray());
			}
		}
		QTextStream in(&file);
		bool ok;
		bool okx;
		bool oky;
		bool okz;
		int section = 0;
		//Offset preperation
		Offsettype otype = BestFit;
		int anchorsetpre = 0;
		int anchorindexpre = 0;
		double manualgravitypre = 0.0;
		//
		//0 haven't started reading lines yet
		//1 HEADER keyword has been read; we are in the header section
		//2 MASS keyword has been read; we are in the mass section
		//3 OBSERVATION SETS keyword has been read; we are in the observation set section
		//4 CROSS SECTIONS keyword has been read; we are in the cross section section
		logline(QString("Reading project from %1 ...").arg(fname));
		while(!in.atEnd()) {
			QString line=in.readLine();
			logline(line);
			if(section==0) {
				if(line=="HEADER") {
					section = 1;
				} else {
					logline("\tWarning: missing HEADER section");
					section = 1;
				}
			} else if(section==1) {
				if(line=="MASS") {
					section = 2;
				} else {
					//process -line- as a header line
					QStringList parts = line.split("\t");
                    if(parts[0] == "displayunits") {
                        //I condensed this block to satisfy Mac warnings about uninitialization.
						int gravityunitint = parts[1].toInt(&ok);
                        if(ok) Units::displaygravunit = static_cast<Units::GravityUnit>(gravityunitint);
						int distanceunitint = parts[2].toInt(&ok);
                        if(ok) Units::displaydistanceunit = static_cast<Units::DistanceUnit>(distanceunitint);
						int densityunitint = parts[3].toInt(&ok);
                        if(ok) Units::displaydensityunit = static_cast<Units::DensityUnit>(densityunitint);
						int massunitint = parts[4].toInt(&ok);
                        if(ok) Units::displaymassunit = static_cast<Units::MassUnit>(massunitint);
					} else if(parts[0] == "offset") {
						int ot = parts[1].toInt(&ok);
						if(ok) otype = static_cast<Offsettype>(ot);
						int as = parts[2].toInt(&ok);
						if(ok) anchorsetpre = as;
						int ai = parts[3].toInt(&ok);
						if(ok) anchorindexpre = ai;
						double man = parts[4].toDouble(&ok);
						if(ok) manualgravitypre = man;
					}
				}
			} else if(section==2) {
				if(line=="OBSERVATION SETS") {
					section = 3;
				} else {
					//process -line- as a mass line
					QStringList parts = line.split("\t");
					if(!parts.empty()) {
						if(parts[0]=="SphereItem") {
							if(parts.size()==10) {
								SphereItem* si = new SphereItem(this);
								si->setName(parts[1]);
								si->setActive((parts[2]=="true")?true:false);
								si->setVisible((parts[3]=="true")?true:false);
								QColor col = QColor(parts[4]);
								if(col.isValid()) si->setColor(col);
								else {logline("\tSphereItem "+si->getName()+": Color could not be parsed."); si->setColor(QColor(170,170,170));}
								double val = parts[5].toDouble(&ok);
								if(ok) si->setDensity(val);
								else { logline("\tSphereItem "+si->getName()+": Density could not be converted to double."); si->setDensity(0.0);}
								double xval = parts[6].toDouble(&okx);
								double yval = parts[7].toDouble(&oky);
								double zval = parts[8].toDouble(&okz);
								if(okx&&oky&&okz) si->setCenter(Point(xval,yval,zval));
								else { logline("\tSphereItem "+si->getName()+": Center coordinate could not be converted to doubles."); si->setCenter(Point(0,0,0));}
								val = parts[9].toDouble(&ok);
								if(ok) si->setRadius(val);
								else { logline("\tSphereItem "+si->getName()+": Radius could not be converted to double."); si->setRadius(0.0);}
								addMassItem(si);
							} else {
								logline("\tSphereItem must have 9 parameters. This item could not be added.");
							}
						} else if(parts[0]=="CylinderItem") {
							if(parts.size() == 11) {
								CylinderItem* ci = new CylinderItem(this);
								ci->setName(parts[1]);
								ci->setActive((parts[2]=="true")?true:false);
								ci->setVisible((parts[3]=="true")?true:false);
								QColor col = QColor(parts[4]);
								if(col.isValid()) ci->setColor(col);
								else { logline("\tCylinderItem "+ci->getName()+": Color could not be parsed."); ci->setColor(QColor(170,170,170));}
								double val = parts[5].toDouble(&ok);
								if(ok) ci->setDensity(val);
								else { logline("\tCylinderItem "+ci->getName()+" Density could not be converted to double."); ci->setDensity(0.0);}
								double xval = parts[6].toDouble(&okx);
								double yval = parts[7].toDouble(&oky);
								double zval = parts[8].toDouble(&okz);
								if(okx&&oky&&okz) ci->setCenter(Point(xval,yval,zval));
								else { logline("\tCylinderItem "+ci->getName()+ "Center coordinate could not be converted to doubles."); ci->setCenter(Point(0,0,0));}
								val = parts[9].toDouble(&ok);
								if(ok) ci->setAzimuth(val);
								else { logline("\tCylinderItem "+ci->getName()+": Azimuth could not be converted to double."); ci->setAzimuth(0.0);}
								val = parts[10].toDouble(&ok);
								if(ok) ci->setRadius(val);
								else { logline("\tCylinderItem "+ci->getName()+": Radius could not be converted to double."); ci->setRadius(0.0);}
								addMassItem(ci);
							} else {
								logline("\tCylinderItem must have 10 parameters. This item could not be added.");
							}
						} else if(parts[0]=="CloudItem") {
							if(parts.size() == 11) {
								CloudItem* cli = new CloudItem(this);
								cli->setName(parts[1]);
								cli->setActive((parts[2]=="true")?true:false);
								cli->setVisible((parts[3]=="true")?true:false);
								QColor col = QColor(parts[4]);
								if(col.isValid()) cli->setColor(col);
								else { logline("\tCloudItem "+cli->getName()+": Color could not be parsed."); cli->setColor(QColor(170,170,170));}
								int ival = parts[6].toInt(&ok);
								if(ok) cli->setDistanceUnit(static_cast<Units::DistanceUnit>(ival));
								else { logline("\tCloudItem "+cli->getName()+": Distance unit could not be parsed.");}
								cli->setSource(parts[5]);
								double val = parts[7].toDouble(&ok);
								if(ok) cli->setPointmass(val);
								else { logline("\tCloudItem "+cli->getName()+": Point mass could not be converted to double."); cli->setDensity(0.0);}
								double xval = parts[8].toDouble(&okx);
								double yval = parts[9].toDouble(&oky);
								double zval = parts[10].toDouble(&okz);
								if(okx&&oky&&okz) cli->setCenter(Point(xval,yval,zval));
								else { logline("\tCloudItem "+cli->getName()+": Center coordinate could not be converted to doubles."); cli->setCenter(Point(0,0,0));}
								addMassItem(cli);
							} else {
								logline("\tCloudItem must have 10 parameters. This item could not be added.");
							}
						} else if(parts[0]=="PolygonPrismItem") {
							if(parts.size() == 10) {
								PolygonPrismItem* ppi = new PolygonPrismItem(this);
								ppi->setName(parts[1]);
								ppi->setActive((parts[2]=="true")?true:false);
								ppi->setVisible((parts[3]=="true")?true:false);
								QColor col = QColor(parts[4]);
								if(col.isValid()) ppi->setColor(col);
								else { logline("\tPolygonPrismItem "+ppi->getName()+": Color could not be parsed."); ppi->setColor(QColor(170,170,170));}
								double val = parts[5].toDouble(&ok);
								if(ok) ppi->setDensity(val);
								else { logline("\tPolygonPrismItem "+ppi->getName()+": Density could not be converted to double."); ppi->setDensity(0.0);}
								double xval = parts[6].toDouble(&okx);
								double yval = parts[7].toDouble(&oky);
								double zval = parts[8].toDouble(&okz);
								if(okx&&oky&&okz) ppi->setCenter(Point(xval,yval,zval));
								else { logline("\tPolygonPrismItem "+ppi->getName()+": Center coordinate could not be converted to doubles."); ppi->setCenter(Point(0,0,0));}
								val = parts[9].toDouble(&ok);
								if(ok) ppi->setAzimuth(val);
								else { logline("\tPolygonPrismItem "+ppi->getName()+": Azimuth could not be converted to double."); ppi->setAzimuth(0.0);}
								//now that we've added the standard parameters, we need to interpret new lines
								std::vector<Point> vs;
								QString extraline = in.readLine();
								while(extraline!="endvertices") {
									QStringList extraparts = extraline.split("\t");
									if(extraparts.size()==3) {
										if(extraparts[0]=="vertex") {
											xval = extraparts[1].toDouble(&okx);
											yval = extraparts[2].toDouble(&oky);
											if(okx&&oky) vs.push_back(Point(xval,yval));
											else logline("\tPolygonPrismItem "+ppi->getName()+": Vertex could not be converted to doubles.");
										} else logline("\tPolygonPrismItem "+ppi->getName()+": vertex keyword expected. Don't forget that your vertex list should end with a single line that says \"endvertices\".");
									} else {
										logline("\tPolygonPrismItem "+ppi->getName()+ ":Vertex could not be read. Vertices must be formatted like this: \"vertex[tab]x[tab]z\".  Don't forget that your vertex list should end with a single line that says \"endvertices\".");
									}
									extraline = in.readLine();
								}
								ppi->setVertices(vs);
								addMassItem(ppi);
							} else {
								logline("PolygonPrismItem must have 9 parameters. This item could not be added.");
							}
						} /* else if(.... it's another type of mass item....

						*/
					}
				}
			} else if(section==3) {
				if(line=="CROSS SECTIONS") {
					section = 4;
				} else {
					//process -line as a observations set line
					QStringList parts = line.split("\t");
					if(!parts.empty()) {
						if(parts[0]=="ObservationSet") {
							if(parts.size()==5) {
								Units::GravityUnit gravityunit;
								Units::DistanceUnit distanceunit;
								bool badunits = false;
								int gravityunitint = parts[3].toInt(&ok);
								if(ok) gravityunit = static_cast<Units::GravityUnit>(gravityunitint); else badunits = true;
								int distanceunitint = parts[4].toInt(&ok);
								if(ok) distanceunit = static_cast<Units::DistanceUnit>(distanceunitint); else badunits = true;
								if(!badunits) {
									ObservationSet* os = new ObservationSet(parts[2], gravityunit, distanceunit,this);
									if(os->isValid()) {
										addObservationSet(os);
									} else logline("\tObservation set could not be loaded from "+parts[2]);
								} else {
									logline("\tImproper units in observation set " + parts[2]);
								}
							} else {
								logline("\tObservationSet must have 4 parameters. This Observation Set could not be added.");
							}
						}
					}
				}
			} else if(section==4) {
				if(line=="END") {
					break;
				} else {
					//process -line as a cross section line
					QStringList parts = line.split("\t");
					if(!parts.empty()) {
						if(parts[0]=="CrossSection") {
							if(parts.size()==15) {
								CrossSection* cs = addCrossSectionSlot();
								int ind = parts[1].toInt(&ok);
								if(ok) cs->setObservationSet(ind+1); else logline("CrossSection: The ObservationSet index could not be read as an int.");
								cs->setAutomaticCrossBounds((parts[2]=="true")?true:false);
								double val = parts[3].toDouble(&ok);
								if(ok) cs->setX1(val); else logline("CrossSection: x1 could not be converted to double.");
								val = parts[4].toDouble(&ok);
								if(ok) cs->setX2(val); else logline("CrossSection: x2 could not be converted to double.");
								val = parts[5].toDouble(&ok);
								if(ok) cs->setY1(val); else logline("CrossSection: y1 could not be converted to double.");
								val = parts[6].toDouble(&ok);
								if(ok) cs->setY2(val); else logline("CrossSection: y2 could not be converted to double.");
								cs->setYAxisAutomatic((parts[7]=="true")?true:false);
								val = parts[8].toDouble(&ok);
								if(ok) cs->setYLow(val); else logline("CrossSection: ylow could not be converted to double.");
								val = parts[9].toDouble(&ok);
								if(ok) cs->setYHigh(val); else logline("CrossSection: yhigh could not be converted to double.");
								val = parts[10].toDouble(&ok);
								if(ok) cs->setYTick0(val); else logline("CrossSection: ytick0 could not be converted to double.");
								val = parts[11].toDouble(&ok);
								if(ok) cs->setYTickInterval(val); else logline("CrossSection: ytickinterval could not be converted to double.");
								val = parts[12].toDouble(&ok);
								if(ok) cs->setXMin(val); else logline("CrossSection: xmin could not be converted to double.");
								val = parts[13].toDouble(&ok);
								if(ok) cs->setXMax(val); else logline("CrossSection: xmax could not be converted to double.");
								val = parts[14].toDouble(&ok);
								if(ok) cs->setRealZOffset(val); else logline("CrossSection: realzoffset could not be converted to double.");
								cs->syncUI();
								cs->updateCrossSection();
							} else {
								logline("CrossSection must have 14 parameters. This cross section could not be added.");
							}
						}
					}
				}
			}
			offsettype = otype;
			if(anchorsetpre<observationsets.size() && anchorindexpre<observationsets[anchorsetpre]->getPoints().size()) {
				anchorset = observationsets[anchorsetpre];
				anchorindex = anchorindexpre;
			}
			manualgravityoffset = manualgravitypre;
		}
		file.close();
		calculateGravityOffset();
		for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
			(*cs)->syncUI();
			(*cs)->updateCrossSection();
		}
		setSaved(true);
		setCurrentProjectFile(fname);
		if(QFile::exists(QDir::current().filePath(QFileInfo(currentprojectfile).baseName()+".ini"))) {
            for(uint i=0; i!=crosssections.size(); ++i) {
                if(mwsettings.contains(QString("crosssection%1geometry").arg(i))) {
                    crosssections[i]->getWindow()->restoreGeometry(mwsettings.value(QString("crosssection%1geometry").arg(i)).toByteArray());
				}
				if(mwsettings.contains(QString("crosssection%1splitterstate").arg(i))) {
                    crosssections[i]->restoreSplitter(mwsettings.value(QString("crosssection%1splitterstate").arg(i)).toByteArray());
				}
			}
		}
	} else {
		logline("Could not open project file "+fname);
	}
}
void MainWindow::selectColorSlot() {
	if(colordialog && colordialog->exec()) {
		selectedcolor->operator=(colordialog->selectedColor());
		QPixmap col(16,16);
		col.fill(*selectedcolor);
		if(selectedcolorbutton)
			selectedcolorbutton->setIcon(QIcon(col));
	}
}
QString MainWindow::getCurrentProjectFile() {
	return currentprojectfile;
}

CrossSection* MainWindow::addCrossSectionSlot() {
	CrossSection* cross = new CrossSection(this);
	crosssections.push_back(cross);
	QMdiSubWindow* sub = workspace.addSubWindow(cross);
	cross->setWindow(sub);
	QRect frame = sub->geometry();
	frame.setHeight(600);
	sub->setGeometry(frame);
	sub->show();
	return cross;
}
void MainWindow::removeCrossSection(CrossSection* cs) {
	for(auto c=crosssections.begin(); c!=crosssections.end(); ++c) {
		if(cs == *c) {
			crosssections.erase(c);
			setSaved(false);
			break;
		}
	}
}

void MainWindow::massItemHasChanged(MassItem* mi) {
    for(uint i=0; i!=massitems.size(); ++i) {
		if(mi==massitems[i]) {
			//for all ObservationSet*s, recalculate that MassItem*'s effect
			for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
				(*os)->calculateGravityEffect(i); //this also calls ObservationSet::updateTable()
			}
			calculateGravityOffset();
			//update all CrossSections*s
			for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
				(*cs)->updateCrossSection();
			}
			break;
		}
	}
	setSaved(false);
}
void MainWindow::massItemNameHasChanged(MassItem* mi) {
    for(uint i=0; i!=massitems.size(); ++i) {
		if(mi==massitems[i]) {
			//update all CrossSections*s
			for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
				(*cs)->updateCrossSection();
			}
			break;
		}
	}
	setSaved(false);
}
void MainWindow::pushUiGravityUpdate() {
	for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
		(*cs)->updateCrossSection();
		(*cs)->syncUI();
	}
	for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
		(*os)->updateTable();
	}
}

void MainWindow::addMassItem(MassItem* mi) {
	massitems.push_back(mi);
	//QListWidgetItem* listitem = new QListWidgetItem(mi->getFullLabel(),&massindex);
	//mi->setListWidgetItem(listitem);
	massindexmodel.insertRow(massindexmodel.rowCount());
	massindexmodel.setData(massindexmodel.index(massindexmodel.rowCount()-1,0,QModelIndex()), mi->getName());
	//Room must be added to each ObservationSet*'s gravityeffects vector
	for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
		(*os)->addMassItemGravityEffect(massitems.size()-1);
    }
	setSaved(false);
}
void MainWindow::createMassItemSlot() {
	QDialog* createMassWindow = new QDialog(this);
	createMassWindow->setModal(true);
	createmassui.setupUi(createMassWindow);
	createMassWindow->show();
	connect(createmassui.createSphere, SIGNAL(clicked()), this, SLOT(createSphereItemSlot()));
	connect(createmassui.createSphere, SIGNAL(clicked()), createMassWindow, SLOT(close()));//need to delete as well? not just close? Or make it non-pointer....
	connect(createmassui.createCylinder, SIGNAL(clicked()), this, SLOT(createCylinderItemSlot()));
	connect(createmassui.createCylinder, SIGNAL(clicked()), createMassWindow, SLOT(close()));//need to delete as well? not just close? Or make it non-pointer....
	connect(createmassui.createCloud, SIGNAL(clicked()), this, SLOT(createCloudItemSlot()));
	connect(createmassui.createCloud, SIGNAL(clicked()), createMassWindow, SLOT(close()));//need to delete as well? not just close? Or make it non-pointer....
	connect(createmassui.createPolygonPrism, SIGNAL(clicked()), this, SLOT(createPolygonPrismItemSlot()));
	connect(createmassui.createPolygonPrism, SIGNAL(clicked()), createMassWindow, SLOT(close()));//need to delete as well? not just close? Or make it non-pointer....
	//Brings up a form that asks what type of mass you want to create. (Point, cylinder, cube, slab, DEM-sandwich, etc)
	//When the type of mass is selected, the appropriate mass edit window is brought up next.
	//	That window is the exact same window that will be available when editing MassItems already in the index.
}
void MainWindow::createSphereItemSlot() {
	SphereItem* sphereitem = new SphereItem(this);
	addMassItem(sphereitem);
	editSphereItemSlot(sphereitem);
}
void MainWindow::createCylinderItemSlot() {
	CylinderItem* cylinderitem = new CylinderItem(this);
	addMassItem(cylinderitem);
	editCylinderItemSlot(cylinderitem);
}
void MainWindow::createCloudItemSlot() {
	CloudItem* clouditem = new CloudItem(this);
	addMassItem(clouditem);
	editCloudItemSlot(clouditem);
}
void MainWindow::createPolygonPrismItemSlot() {
	PolygonPrismItem* polygonprismitem = new PolygonPrismItem(this);
	addMassItem(polygonprismitem);
	editPolygonPrismItemSlot(polygonprismitem);
}
void MainWindow::editMassItemSlot() {
	//QListWidgetItem* lwi = massindex.currentItem();
	QModelIndex selected = massindex.currentIndex();
	if(selected.isValid()) {
		MassItem* mi = massitems[selected.row()];
		if(SphereItem* si = dynamic_cast<SphereItem*>(mi)) {
			editSphereItemSlot(si);
		}
		if(CylinderItem* si = dynamic_cast<CylinderItem*>(mi)) {
			editCylinderItemSlot(si);
		}
		if(CloudItem* cli = dynamic_cast<CloudItem*>(mi)) {
			editCloudItemSlot(cli);
		}
		if(PolygonPrismItem* ppi = dynamic_cast<PolygonPrismItem*>(mi)) {
			editPolygonPrismItemSlot(ppi);
		}
	}
}
void MainWindow::editSphereItemSlot(SphereItem* si) {
	QDialog* editSphereWindow = new QDialog(this);
	editSphereWindow->setModal(true);
	editsphereui.setupUi(editSphereWindow);
	connect(editsphereui.applybutton, SIGNAL(clicked()), editSphereWindow, SLOT(accept()));
	connect(editsphereui.cancelbutton, SIGNAL(clicked()), editSphereWindow, SLOT(reject()));//need to delete as well? not just close? Or make it non-pointer....
	editsphereui.densityu->setText(Units::densitySuffix()); editsphereui.xu->setText(Units::distanceSuffix()); editsphereui.yu->setText(Units::distanceSuffix()); editsphereui.zu->setText(Units::distanceSuffix()); editsphereui.radiusu->setText(Units::distanceSuffix());
	editsphereui.name->setText(si->getName());
	editsphereui.active->setChecked(si->isActive());
	editsphereui.visible->setChecked(si->isVisible());
		QPixmap coloricon(16,16); coloricon.fill(si->getColor());
		editsphereui.color->setIcon(QIcon(coloricon));
		selectedcolorbutton = editsphereui.color;
		QColor newcol = si->getColor();
		selectedcolor = &newcol;
		colordialog->setCurrentColor(si->getColor());
		connect(editsphereui.color, SIGNAL(clicked()), this, SLOT(selectColorSlot()));
	editsphereui.density->setValue(Units::siToDisplayDensity(si->getDensity()));
	editsphereui.x->setValue(Units::siToDisplayDistance(si->getCenter().x));
	editsphereui.y->setValue(Units::siToDisplayDistance(si->getCenter().y));
	editsphereui.z->setValue(Units::siToDisplayDistance(si->getCenter().z));
	editsphereui.radius->setValue(Units::siToDisplayDistance(si->getRadius()));
	if(editSphereWindow->exec() == QDialog::Accepted) {
		si->setName(editsphereui.name->text());
		si->setActive(editsphereui.active->isChecked());
		si->setVisible(editsphereui.visible->isChecked());
		si->setColor(newcol);
		si->setDensity(Units::displayToSiDensity(editsphereui.density->value()));
		si->setCenter(Point(Units::displayToSiDistance(editsphereui.x->value()),Units::displayToSiDistance(editsphereui.y->value()),Units::displayToSiDistance(editsphereui.z->value())));
		si->setRadius(Units::displayToSiDistance(editsphereui.radius->value()));
	} else {
		
	}
}
void MainWindow::editCylinderItemSlot(CylinderItem* ci) {
	EditCylinderItemUI* editwindow = new EditCylinderItemUI(this, ci, this);//first this is the parent, second this is to give direct acccess to MainWindow*
	editwindow->setModal(true);
	editwindow->exec();
}
void MainWindow::editCloudItemSlot(CloudItem* cli) {
	EditCloudItemUI* editwindow = new EditCloudItemUI(this, cli, this); //first this is the parent, second this is to give direct acccess to MainWindow*
	editwindow->setModal(true);
	editwindow->exec();
}
void MainWindow::editPolygonPrismItemSlot(PolygonPrismItem* ppi) {
	//See editpolygonprismui.cpp
	EditPolygonPrismItemUI* editwindow = new EditPolygonPrismItemUI(this, ppi, this);//first this is the parent, second this is to give direct acccess to MainWindow*
	editwindow->setModal(true);
	editwindow->exec();
}
void MainWindow::removeMassItemSlot() {
	QModelIndex selected = massindex.currentIndex();
	if(selected.isValid()) {
        //MassItem* mi = massitems[selected.row()];
		massitems.erase(massitems.begin()+selected.row());
		massindexmodel.removeRow(selected.row());
		for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
			(*os)->removeMassItemGravityEffect(selected.row());
		}
		calculateGravityOffset();
		for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
			(*cs)->updateCrossSection();
		}
	}
	setSaved(false);
}
void MainWindow::sendMassItemUpSlot() {
	QModelIndex selected = massindex.currentIndex();
	if(selected.isValid()) {
		MassItem* mi = massitems[selected.row()];
		if(selected.row()==0) {
			return; //This item can't be moved up. It's on top.
		}
		MassItem* swapmi = massitems[selected.row()-1];
		massitems[selected.row()-1] = mi;
		massitems[selected.row()] = swapmi;
		for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
			(*os)->calculateGravityEffects();
		}
		calculateGravityOffset();
		for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
			(*cs)->updateCrossSection();
		}
	}
	setSaved(false);
}
void MainWindow::sendMassItemDownSlot() {
	QModelIndex selected = massindex.currentIndex();
	if(selected.isValid()) {
		MassItem* mi = massitems[selected.row()];
		if(selected.row()==massitems.size()-1) {
			return; //This item can't be moved up. It's on top.
		}
		MassItem* swapmi = massitems[selected.row()+1];
		massitems[selected.row()+1] = mi;
		massitems[selected.row()] = swapmi;
		for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
			(*os)->calculateGravityEffects();
		}
		calculateGravityOffset();
		for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
			(*cs)->updateCrossSection();
		}
	}
	setSaved(false);
}
void MainWindow::duplicateMassItemSlot() {
    QModelIndex selected = massindex.currentIndex();
    if(selected.isValid()) {
        MassItem* mi = massitems[selected.row()];
        int dupindex = selected.row()+1; //index of the new duplicated item. Put it right under the original item
		MassItem* dupitem = mi->clone();
        massitems.insert(massitems.begin()+dupindex, dupitem);
        massindexmodel.insertRow(dupindex);
        massindexmodel.setData(massindexmodel.index(dupindex,0,QModelIndex()), dupitem->getName());

        for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
            (*os)->addMassItemGravityEffect(dupindex);
            (*os)->calculateGravityEffects();
        }
        calculateGravityOffset();
        for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
            (*cs)->updateCrossSection();
        }
    }
    setSaved(false);
}

void MainWindow::addObservationSet(ObservationSet* os) {
	observationsets.push_back(os);
	QListWidgetItem* listitem = new QListWidgetItem(os->getName(),&obssetindex);
	os->setListWidgetItem(listitem);
	listitemobsmap[listitem] = os;
	setSaved(false);
}
void MainWindow::createObservationSetManualSlot() {
	//create empty observation set, then open the observation set edit interface.
	//
	//
}
void MainWindow::createObservationSetFileSlot() {
	QFileDialog dialog(this,"Open Observation Set");
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setLabelText(QFileDialog::Accept,"Open");
	if( dialog.exec() ) { 
		QString fileName = dialog.selectedFiles().first();
		ObservationSet* obs = new ObservationSet(fileName,this);
		if(obs->hasUnits()) {
			addObservationSet(obs);
			for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
				(*cs)->updateObservationSetCombo();
			}
		} else {
			delete obs;
		}
	}
}
void MainWindow::reloadObservationSetSlot() {
	QListWidgetItem* lwi = obssetindex.currentItem();
	auto iter = listitemobsmap.find(lwi);
	if(iter!=listitemobsmap.end()) {
		ObservationSet* os = iter->second;
		os->initializeFromFile(os->getSourceFile());
		os->calculateGravityEffects();
		calculateGravityOffset();
		//update all CrossSections*s
		for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
			(*cs)->updateCrossSection();
		}
		setSaved(false);
	}
}
void MainWindow::removeObservationSetSlot() {
	QListWidgetItem* lwi = obssetindex.currentItem();
	auto iter = listitemobsmap.find(lwi);
	if(iter!=listitemobsmap.end()) {
		ObservationSet* os = iter->second;
		int ret = QMessageBox::Yes;
		if(os == anchorset) {
			QMessageBox msgBox;
			msgBox.setText("Your model\'s offset is currently anchored to this observation set. If you remove this observation set, a best-fit offset will be applied. (You can edit your offset parameters from Model->Offset options)");
			msgBox.setInformativeText("Remove this observation set?");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::Save);
			ret = msgBox.exec();
			if(ret==QMessageBox::Yes) {
				setOffsetType(BestFit);
			}
		}
		if(ret == QMessageBox::Yes) {
			obssetindex.takeItem(obssetindex.currentIndex().row());
			os->closeTable();
			for(auto osi = observationsets.begin(); osi!=observationsets.end(); ++osi) {
				if(*osi==os) {
					observationsets.erase(osi);
					break;
				}
			}
			for(auto cs=crosssections.begin(); cs!=crosssections.end(); ++cs) {
				(*cs)->clearObservationSet(os);
			}
			listitemobsmap.erase(iter);
		}

	}
	setSaved(false);
}
void MainWindow::openObservationSetTable() {
	QListWidgetItem* lwi = obssetindex.currentItem();
	auto iter = listitemobsmap.find(lwi);
	if(iter!=listitemobsmap.end()) {
		ObservationSet* os = iter->second;
		if(os->getTable()==nullptr) {
			ObservationSetTable* ost = new ObservationSetTable(this);
			//QDockWidget* ostdockwidget = new QDockWidget("Observation Set Table", this);
			QMdiSubWindow* sub = workspace.addSubWindow(ost);
			os->setTable(ost, sub);
			ost->setObservationSet(os);
			sub->show();
			//ostdockwidget->setAllowedAreas(Qt::AllDockWidgetAreas);
			//ostdockwidget->setWidget(ost);
			//addDockWidget(Qt::RightDockWidgetArea, ostdockwidget);
		}
	}
}
void MainWindow::calculateObservationSetGravityEffects() {
	QListWidgetItem* lwi = obssetindex.currentItem();
	auto iter = listitemobsmap.find(lwi);
	if(iter!=listitemobsmap.end()) {
		ObservationSet* os = iter->second;
		os->calculateGravityEffects();
	}
}

void MainWindow::setOffsetType(Offsettype ot) {
	offsettype = ot;
	calculateGravityOffset();
	pushUiGravityUpdate();
	setSaved(false);
}
void MainWindow::setOffsetAnchor(ObservationSet* os, int eindex) {
	anchorset = os;
	anchorindex = eindex;
	calculateGravityOffset();
	pushUiGravityUpdate();
	setSaved(false);
}
void MainWindow::openOffsetDialog() {
	OffsetUI* offsetdialog = new OffsetUI(this);
	if(offsettype==BestFit) {
		offsetdialog->ui.bestfit->setChecked(true);
		offsetdialog->bestfitClickedSlot();
	} else if(offsettype==Anchor) {
		offsetdialog->ui.anchor->setChecked(true);
		offsetdialog->anchorClickedSlot();
	} else if(offsettype==Manual) {
		offsetdialog->ui.manual->setChecked(true);
		offsetdialog->manualClickedSlot();
	}
	offsetdialog->ui.anchorset->clear();
	offsetdialog->ui.anchorset->addItem("Select Observationset");
	int osindex=0;
    for(uint i=0; i!=observationsets.size(); ++i) {
		offsetdialog->ui.anchorset->addItem(observationsets[i]->getName());
		if(observationsets[i]==anchorset) {
			osindex = i+1; //+1 beause of the first dummy item.
		}
	}
	offsetdialog->ui.anchorset->setCurrentIndex(osindex); //this will make the offsetui populate the anchorpoint combobox now.
	int pindex = 0;
	for(int i=0; i!=offsetdialog->ui.anchorpoint->count(); ++i) {
		if(i>0) {
			if(offsetdialog->ui.anchorpoint->itemText(i).split(" ")[0].toInt()==anchorindex) {
				pindex = i;
			}
		}
	}
	offsetdialog->ui.anchorpoint->setCurrentIndex(pindex);
	offsetdialog->ui.manualoffset->setText(QString::number(Units::siToDisplayGrav(manualgravityoffset)));
	offsetdialog->setManualOffsetFromLineEdit();
	offsetdialog->ui.manualoffsetu->setText(Units::gravSuffix());
	if(offsetdialog->exec() == QDialog::Accepted) {
		if(offsetdialog->ui.bestfit->isChecked()) {
			offsettype = BestFit;
		} else if(offsetdialog->ui.anchor->isChecked()) {
			offsettype = Anchor;
			if(offsetdialog->ui.anchorset->currentIndex() > 0) {
				anchorset = observationsets[offsetdialog->ui.anchorset->currentIndex()-1];
				if(offsetdialog->ui.anchorpoint->currentIndex()>0) {
					QString itemtext = offsetdialog->ui.anchorpoint->currentText();
					QStringList parts = itemtext.split(" ");
					QString indexstring = parts[0];
					int index = indexstring.toInt();
					anchorindex = index;
				}
			} else {
				offsettype = BestFit; //this is a "silent" reversion, it would be better to also tell the user that their choice was invalid.
			}
		} else if(offsetdialog->ui.manual->isChecked()) {
			offsettype = Manual;
		}
		bool ok;
		double val = offsetdialog->ui.manualoffset->text().toDouble(&ok);
		if(ok) {
			manualgravityoffset = Units::displayToSiGrav(val); 
		} else {
			logline("Manual gravity offset could not be parsed as a double.");
		}
		setSaved(false);
	}
	calculateGravityOffset();
	pushUiGravityUpdate();
}
void MainWindow::calculateGravityOffset() {
	if(offsettype==Manual) {
		gravityoffset = manualgravityoffset;
	} else if(offsettype==Anchor) {
		if(anchorset && anchorindex>=0 && anchorindex<anchorset->getSize()) {
			auto emp = anchorset->getEmpGravityVals();
			if(isNoData(emp[anchorindex])) {  //if there is nodata at this point
				logline(QString("Cannot set anchor point to Observation Set \"%1\" point %2 because there is no empirical data at this point.\n  Gravity offset set to 0.0.").arg(anchorset->getName()).arg(anchorindex));
				gravityoffset = 0.0;
			} else {
				gravityoffset = emp[anchorindex]-anchorset->getSumGravityEffect(anchorindex);
			}
		}
	} else if(offsettype==BestFit) {
		//Find a least squares best fit. I.e, for the equation:
		//error(d) = sum_i_n( (c_i+d-e_i)^2 )
		// where d is the offset, 
		//       c is the calculated gravity value
		//       e is the empirical gravity value
		//       n is the total number of point *in all observationsets* that have empirical data
		//       i is the i'th point
		//find the value for d that minimizes error.
		//This comes out to d = sum_i_n(e_i-c_i)/n

		//Note: it would be possible to do a linear interpolation to "fill in" empirical data where there is none. Probably not a good idea.
		int n = 0;
		double sum = 0;
		for(auto os=observationsets.begin(); os!=observationsets.end(); ++os) {
			auto emp = (*os)->getEmpGravityVals();
            for(uint i=0; i!=emp.size(); ++i) {
				if(!isNoData(emp[i])) {
					++n;
					sum += emp[i]-(*os)->getSumGravityEffect(i);
				}
			}
		}
		if(n>0) {
			gravityoffset = sum/(double)n;
		} else {
			gravityoffset = 0.0;
		}
	}
}
double MainWindow::getGravityOffset() {
	return gravityoffset;
}

void MainWindow::openUnitsDialog() {
	QDialog* unitwindow =new QDialog(this);
	unitsui.setupUi(unitwindow);
	connect(unitsui.applybutton, SIGNAL(clicked()), unitwindow, SLOT(accept()));
	connect(unitsui.cancelbutton, SIGNAL(clicked()), unitwindow, SLOT(reject()));//need to delete as well? not just close? Or make it non-pointer....
	unitsui.acceleration->clear();
	unitsui.acceleration->addItem("m/s^2");
	unitsui.acceleration->addItem("mGal");
	unitsui.acceleration->setCurrentIndex(static_cast<int>(Units::displaygravunit));
	unitsui.distance->clear();
	unitsui.distance->addItem("m");
	unitsui.distance->addItem("ft");
	unitsui.distance->setCurrentIndex(static_cast<int>(Units::displaydistanceunit));
	unitsui.density->clear();
	unitsui.density->addItem("kg/m^3");
	unitsui.density->addItem("g/cm^3");
	unitsui.density->setCurrentIndex(static_cast<int>(Units::displaydensityunit));
	unitsui.mass->clear();
	unitsui.mass->addItem("kg");
	unitsui.mass->addItem("lb");
	unitsui.mass->setCurrentIndex(static_cast<int>(Units::displaymassunit));
	if(unitwindow->exec() == QDialog::Accepted) {
		Units::setGravityDisplayUnit(static_cast<Units::GravityUnit>(unitsui.acceleration->currentIndex())); //The index actually lines up exactly with the GravityUnit enum. 0=Ms2 1=MGal
		Units::setDistanceDisplayUnit(static_cast<Units::DistanceUnit>(unitsui.distance->currentIndex())); //The index actually lines up exactly with the DistanceUnit enum. 0=M 1=Ft
		Units::setDensityDisplayUnit(static_cast<Units::DensityUnit>(unitsui.density->currentIndex())); //The index actually lines up exactly with the DensityUnit enum. 0=Kgm3 1=Gcm3
		Units::setMassDisplayUnit(static_cast<Units::MassUnit>(unitsui.mass->currentIndex())); //The index actually lines up exactly with the MassUnit enum. 0=Kg 1=Lb
		pushUiGravityUpdate();
		setSaved(false);
	} else {

	}
}
void MainWindow::helpSlot() {
	QDialog* infowindow = new QDialog(this);
	infoui.setupUi(infowindow);
	infowindow->exec();
}

//Rudimentary gravity calculation output
void MainWindow::outputGravityResultsFromIndex() {
	QListWidgetItem* lwi = obssetindex.currentItem();
	auto iter = listitemobsmap.find(lwi);
	if(iter!=listitemobsmap.end()) {
		saveGravityResultsFor(iter->second);
	}
}
void MainWindow::saveGravityResultsFor(ObservationSet* os) {
	SaveResultsUI* dialog = new SaveResultsUI(this, os);
	dialog->show();
}
