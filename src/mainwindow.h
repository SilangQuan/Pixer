#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIcon>
#include <QScrollBar>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImageReader>
#include <QDebug>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QStatusBar>
#include <QScrollArea>
#include <QSizePolicy>
#include <QToolBar>
#include <QCursor>
#include <QDir>
#include "canvas.h"
#include "newfiledlg.h"
#include "swatches.h"
#include "thumbnailpanel.h"
#include "palettepanel.h"
#include "layerpanel.h"
#include "layermanager.h"
#include "exportdlg.h"
#include "historypanel.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	void createActions();
	void createMenus();
    QDir applicationDir;
	
	QScrollArea *scrollArea;
	QWidget *centralWidget;
	Canvas *canvas;
	Swatches *foreSwatches;
	Swatches *backSwatches;
	QRect screenRect;

	QMenu *fileMenu;
	QMenu *operationMenu;
	QMenu *viewMenu;
	QMenu *windowsMenu;
	QMenu *helpMenu;
	QMenu *toolsMenu;

	QList<QAction*> *toolActionList;
	QActionGroup *toolbarGroup;
	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *exportAct;
	QAction *printAct;
	QAction *exitAct;
	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *normalSizeAct;
	QAction *fitToWindowAct;
	QAction *aboutAct;
	QAction *aboutQtAct;
	QAction *analyzeAct;
	QAction *cropAct;
	QAction *bucketAct;

	QList<QAction*> *effectActionList;
	QAction *loadPaletteAct;
	QAction *savePaletteAct;
	QAction *sealingLayerAct;
	QAction *flipHorizontalLayerAct;
	QAction *flipVerticalLayerAct;
	QAction *grayScaleAct;
	QAction *sepiaAct;
	QAction *sepiaBlueAct;
	QAction *sepiaGreenAct;

	QAction *penAct;
	QAction *eraserAct;
	QAction *selectionAct;
	QAction *strawAct;
	QAction *zoomAct;
	QAction *moveAct;
	QAction *dodgeAct;

	QAction *showLayerAct;
	QAction *showPaletteAct;
	QAction *showThumbAct;
	QAction *showHistoryAct;
    QAction *showToolBarAct;

	QAction *undoAction;
	QAction *redoAction;

	QToolBar *toolbar;

	ThumbnailPanel *thumbnailPanel;
	PalettePanel *palettePanel;
	LayerPanel *layerPanel;
	HistoryPanel *historyPanel;

	QLabel *zoomLabel;
	QLabel *resolutionLabel;

private slots:
	void open();
	void save();
	void saveAs();
	void newFile();
	void about();
	void useMove();
	void usePen();
	void useEraser();
	void useStraw();
	void useDodge();
	void useZoom();
	void useRectSelection();
	void useCrop();
	void useBucket();
	void exportAs();
	void exchangeSwatches();

	void updateStatueBar(float scaleFactor);
	void updateThumbnail();

	void loadPalette();
	void savePalette();

    void showToolbar();
	void showThumbWindow();
	void showPaletteWindow();
	void showLayerWindow();
	void showHistoryWindow();

	void disableActions();
	void enableActions();

protected:
	void keyPressEvent(QKeyEvent *e);	
	void keyReleaseEvent(QKeyEvent *e);
	void wheelEvent(QWheelEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);

};

#endif // MAINWINDOW_H
