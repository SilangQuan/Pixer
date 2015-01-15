#-------------------------------------------------
#
# Project created by QtCreator 2014-11-03T11:13:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pixer
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/canvas.cpp \
    src/commands.cpp \
    src/exportdlg.cpp \
    src/historypanel.cpp \
    src/imageprocessor.cpp \
    src/layeritemdelegate.cpp \
    src/layermanager.cpp \
    src/layerpanel.cpp \
    src/layertablemodel.cpp \
    src/layertableview.cpp \
    src/newfiledlg.cpp \
    src/palettepanel.cpp \
    src/palettesquare.cpp \
    src/swatches.cpp \
    src/thumbnailcanvas.cpp \
    src/thumbnailpanel.cpp

HEADERS  += src/mainwindow.h \
    src/canvas.h \
    src/commands.h \
    src/common.h \
    src/exportdlg.h \
    src/historypanel.h \
    src/imageprocessor.h \
    src/layeritemdelegate.h \
    src/layermanager.h \
    src/layerpanel.h \
    src/layertablemodel.h \
    src/layertableview.h \
    src/newfiledlg.h \
    src/palettepanel.h \
    src/palettesquare.h \
    src/resource.h \
    src/selectionrect.h \
    src/swatches.h \
    src/thumbnailcanvas.h \
    src/thumbnailpanel.h

OTHER_FILES +=

RESOURCES += \
    resources.qrc

RC_FILE += Pixer.rc
