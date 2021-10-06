#-------------------------------------------------
#
# Project created by QtCreator 2016-11-26T22:13:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FastGrav
TEMPLATE = app

RC_ICONS = icons\smoothicons2.ico

SOURCES += main.cpp \
    clouditem.cpp \
    crosssection.cpp \
    crosssectionscene.cpp \
    cylinderitem.cpp \
    editclouditemui.cpp \
    editcylinderitemui.cpp \
    editpolygonprismitemui.cpp \
    global.cpp \
    logconsole.cpp \
    mainwindow.cpp \
    massindexitemdelegate.cpp \
    massindexmodel.cpp \
    massitem.cpp \
    observationset.cpp \
    observationsettable.cpp \
    offsetui.cpp \
    point.cpp \
    polygonprismitem.cpp \
    profilegraph.cpp \
    saveresultsui.cpp \
    sphereitem.cpp \
    units.cpp \
    yaxisui.cpp

HEADERS  += \
    clouditem.h \
    crosssection.h \
    crosssectionscene.h \
    cylinderitem.h \
    editclouditemui.h \
    editcylinderitemui.h \
    editpolygonprismitemui.h \
    global.h \
    logconsole.h \
    mainwindow.h \
    massindexitemdelegate.h \
    massindexmodel.h \
    massitem.h \
    observationset.h \
    observationsettable.h \
    offsetui.h \
    point.h \
    polygonprismitem.h \
    profilegraph.h \
    saveresultsui.h \
    sphereitem.h \
    units.h \
    yaxisui.h

FORMS += \
    createmassitemui.ui \
    crosssectionui.ui \
    editclouditemui.ui \
    editcylinderitemui.ui \
    editpolygonprismitemui.ui \
    editsphereitemui.ui \
    infoui.ui \
    observationsettableui.ui \
    observationsetunitsui.ui \
    offsetui.ui \
    saveresultsui.ui \
    unitsui.ui \
    yaxisui.ui

RESOURCES = icons\fastgrav.qrc
