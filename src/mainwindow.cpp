#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), canvas()
{
    applicationDir = QDir(QApplication::applicationDirPath());

#if defined(Q_OS_WIN)
    if (applicationDir.dirName().toLower() == "debug"
            || applicationDir.dirName().toLower() == "release"
            || applicationDir.dirName().toLower() == "bin")
			//applicationDir.cdUp();
#elif defined(Q_OS_MAC)
    if (applicationDir.dirName() == "MacOS") {
        applicationDir.cdUp();
        applicationDir.cdUp();
        applicationDir.cdUp();
    }
#endif

	qDebug() << applicationDir.absolutePath();
    QIcon c(applicationDir.absolutePath() + "/Resources/images/pixer_icon.ico");

    QPixmap p(applicationDir.absolutePath() + "/Resources/images/pixer_icon.ico");

	this->setWindowIcon(c);

	QDesktopWidget* desktopWidget = QApplication::desktop();
	screenRect = desktopWidget->screenGeometry();
	setFocusPolicy(Qt::StrongFocus);

	this->setStyleSheet("background-color: #333337; color: #f1f1f1;");
	//this->setStyleSheet("QAbstractScrollArea::corner {background: #535353;}");
	this->setMinimumWidth(1024);
	this->setMinimumHeight(768);
	this->setWindowTitle(tr("Pixer"));

	zoomLabel = new QLabel;
	zoomLabel->setMinimumWidth(30);
	zoomLabel->setStyleSheet("background-color: #535353;");
	resolutionLabel = new QLabel;
	resolutionLabel->setMinimumWidth(50);
	resolutionLabel->setStyleSheet("background-color: #535353;");
	statusBar()->addWidget(zoomLabel);
	statusBar()->addWidget(resolutionLabel);
	statusBar()->setStyleSheet(QString("QStatusBar{background-color: #535353;}\
									   	QStatusBar::item{border:0px}\
									   	QSizeGrip{background-color: #535353;}"));

	canvas = new Canvas();
	//canvas->setMinimumSize(2024, 1768);
	canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	scrollArea = new QScrollArea();
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setWidgetResizable(true);
	scrollArea->setAlignment(Qt::AlignHCenter);
	
	statusBar()->showMessage("Ok");
	
	scrollArea->setWidget(canvas);
	this->setCentralWidget(scrollArea);
	
	menuBar()->setStyleSheet(" QMenuBar{background-color: #535353; padding-left: 5px;}\
							QMenuBar::item {background-color: #535353; padding:2px; margin:6px 10px 0px 0px;} \
						   	QMenuBar::item:selected {background: #3e3e40;} \
							QMenuBar::item:pressed {background: #1b1b1c;}\
							");
	createActions();
	createMenus();

	toolActionList = new QList<QAction*>();
	toolActionList->append(moveAct);
	toolActionList->append(penAct);
	toolActionList->append(eraserAct);
	toolActionList->append(selectionAct);
	toolActionList->append(bucketAct);
	toolActionList->append(cropAct);
	toolActionList->append(strawAct);
	toolActionList->append(dodgeAct);
	toolActionList->append(zoomAct);
	
	effectActionList = new QList<QAction*>();
	effectActionList->append(sealingLayerAct);
	effectActionList->append(flipHorizontalLayerAct);
	effectActionList->append(flipVerticalLayerAct);
	effectActionList->append(grayScaleAct);
	effectActionList->append(sepiaAct);
	effectActionList->append(sepiaBlueAct);
	effectActionList->append(sepiaGreenAct);

	disableActions();

	toolbar = new QToolBar();
	this->addToolBar(Qt::LeftToolBarArea, toolbar);
	toolbar->setStyleSheet("color:black");
	for (int i = 0; i < toolActionList->size(); ++i) {
		toolbar->addAction(toolActionList->at(i));
	}
	toolbar->addSeparator();
	toolbar->setOrientation(Qt::Vertical);


	foreSwatches = new Swatches();
	foreSwatches->setMinimumSize(30, 30);
	toolbar->addWidget(foreSwatches);

	backSwatches = new Swatches();
	backSwatches->setMinimumSize(30, 30);
	toolbar->addSeparator();
	toolbar->addWidget(backSwatches);

	connect(foreSwatches, SIGNAL(colorSelected(QColor)), canvas, SLOT(setForeColor(QColor)));
	connect(canvas, SIGNAL(colorStrawed(QColor)), foreSwatches, SLOT(changeColor(QColor)));
	connect(canvas, SIGNAL(colorStrawed(QColor)), canvas, SLOT(setForeColor(QColor)));
	connect(backSwatches, SIGNAL(colorSelected(QColor)), canvas, SLOT(setBackColor(QColor)));
	connect(canvas, SIGNAL(scaleFactorChanged(float)), this, SLOT(updateStatueBar(float)));
	backSwatches->changeColor(QColor(0, 0, 0));

	thumbnailPanel = new ThumbnailPanel(this);
	thumbnailPanel->setWindowTitle(tr("Navigator"));
	thumbnailPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea, thumbnailPanel);

	connect(thumbnailPanel, SIGNAL(thumbZoom(float)), canvas, SLOT(setScaleFactor(float)));
	connect(thumbnailPanel, SIGNAL(thumbZoom(float)), this, SLOT(updateStatueBar(float)));
	connect(canvas, SIGNAL(scaleFactorChanged(float)), thumbnailPanel, SLOT(updateZoomLabel(float)));
	connect(canvas, SIGNAL(scaleFactorChanged(float)), thumbnailPanel, SLOT(updateZoomSlider(float)));

	palettePanel = new PalettePanel(this);
	palettePanel->setWindowTitle(tr("Palette"));
	palettePanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea, palettePanel);
    palettePanel->loadPalette(QString(applicationDir.absolutePath() +"/Resources/palette/default.gpl"));
	connect(palettePanel, SIGNAL(changeForecolorSignal(QColor)), foreSwatches, SLOT(changeColor(QColor)));

	layerPanel = new LayerPanel(this);
	layerPanel->setWindowTitle(tr("Layer"));
	layerPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea, layerPanel);
	
	connect(canvas, SIGNAL(canvasChanged()), this, SLOT(updateThumbnail()));
	connect(canvas, SIGNAL(canvasChanged()), layerPanel, SLOT(update()));

	connect(canvas, SIGNAL(canvasInitialised()), this, SLOT(enableActions()));
	
	historyPanel = new HistoryPanel(canvas->getUndoStack(), this);
	historyPanel->setWindowTitle(tr("History"));
	historyPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->addDockWidget(Qt::LeftDockWidgetArea, historyPanel);
	historyPanel->hide();
	connect(historyPanel, SIGNAL(historyTriggered()), canvas, SLOT(updateCanvasAndThumb()));

    //
#if defined(Q_OS_WIN)
    palettePanel->setStyleSheet("background-color: #393939;");
    historyPanel->setStyleSheet("background-color: #393939;");
    layerPanel->setStyleSheet("background-color: #393939;");
    thumbnailPanel->setStyleSheet("background-color: #393939; padding: 0px;");
#elif defined(Q_OS_MAC)
    palettePanel->setStyleSheet("background-color: #393939; color:#000000");
    historyPanel->setStyleSheet("background-color: #393939; color:#000000");
    layerPanel->setStyleSheet("background-color: #393939; color:#000000");
    thumbnailPanel->setStyleSheet("background-color: #393939; color:#000000;padding: 0px;");
    toolbar->setStyleSheet("QToolBar{background: #393939; border: 0px solid gray;} QToolbutton{background:black} ");
    //toolbar->setStyleSheet("QToolBar::tab{color:gray} ");
    //toolbar->setStyleSheet("QToolBar::tab:selected{color:white} ");

#endif
	LayerManager* pManager = LayerManager::getInstance();
	connect(pManager, SIGNAL(updateCanvas()), canvas, SLOT(update()));
	//pManager->setCanvasObserver(canvas);
}

MainWindow::~MainWindow()
{
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About Pixer"),
		tr("<p><b>Pixer</b> is an Pixel painter.</p>"
		"<p>Contact: silangquan@gmail.com</p>"));
}

void MainWindow::newFile()
{
	NewFileDlg *newFileDlg;
	newFileDlg = new NewFileDlg();
	connect(newFileDlg, SIGNAL(configConfirmed(int, int, COLORMODE)), \
		canvas, SLOT(setConfig(int, int, COLORMODE)));
	newFileDlg->exec();
	updateStatueBar(1.0);
	resolutionLabel->setText(canvas->getResolution());
}

void MainWindow::open()
{
	QString fileName;
	QFileDialog* fd = new QFileDialog(this, tr("Open image File"), ".", tr("Image files(*.jpg *.png)"));
	fd->setFileMode(QFileDialog::ExistingFiles);
	fd->setViewMode(QFileDialog::Detail);

	if (fd->exec() == QDialog::Accepted)
	{
		canvas->initWithFile(fd->selectedFiles().first());
		updateStatueBar(canvas->getScaleFactor());
		resolutionLabel->setText(canvas->getResolution());
	}
}
void MainWindow::save()
{
///	QPixmap pixmap(canvas->size());
	//canvas->render(&pixmap, QPoint(), QRegion(canvas->rect()));
	//pixmap.save("C:/Users/River/Desktop/0.png");
}

void MainWindow::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save As"), "",
		tr("PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp)"));
	if (!fileName.isEmpty())
	{
		 canvas->save(fileName);
	}
	else 
	{
		return;
	}
}

void MainWindow::updateThumbnail()
{
    QPixmap tmp = QPixmap::fromImage(canvas->getMergedImage());
    thumbnailPanel->setThumb(tmp);

}

void MainWindow::createActions()
{
	newAct = new QAction(tr("&New File"), this);
	newAct->setShortcut(tr("Ctrl+N"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

	openAct = new QAction(tr("&Open File"), this);
	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveAct = new QAction(tr("&Save"), this);
	saveAct->setShortcut(tr("Ctrl+S"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAct = new QAction(tr("&Save as"), this);
	saveAsAct->setShortcut(tr("Ctrl+Shift+S"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	exportAct = new QAction(tr("&Export"), this);
	exportAct->setShortcut(tr("Ctrl+Shift+E"));
	connect(exportAct, SIGNAL(triggered()), this, SLOT(exportAs()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
	zoomInAct->setShortcut(tr("Ctrl+-"));
	connect(zoomInAct, SIGNAL(triggered()), canvas, SLOT(zoomIn()));
	//connect(zoomInAct, SIGNAL(triggered()), this, SLOT(updateStatueBar()));

	zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
	zoomOutAct->setShortcut(tr("Ctrl+="));
	//zoomOutAct->setEnabled(false);
	connect(zoomOutAct, SIGNAL(triggered()), canvas, SLOT(zoomOut()));
	//connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(updateStatueBar()));

	normalSizeAct = new QAction(tr("&Normal Size"), this);
	normalSizeAct->setShortcut(tr("Ctrl+S"));
	//normalSizeAct->setEnabled(false);
	//connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

	fitToWindowAct = new QAction(tr("&Fit to Window"), this);
	fitToWindowAct->setEnabled(false);
	fitToWindowAct->setCheckable(true);
	fitToWindowAct->setShortcut(tr("Ctrl+F"));
	//connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));
	
	undoAction = new QAction(tr("&Undo"), this);
	undoAction->setShortcuts(QKeySequence::Undo);
	connect(undoAction, SIGNAL(triggered()), canvas, SLOT(undo()));

	redoAction = new QAction(tr("&Redo"), this);
	redoAction->setShortcuts(QKeySequence::Redo);
	connect(redoAction, SIGNAL(triggered()), canvas, SLOT(redo()));


	loadPaletteAct = new QAction(tr("&Load Palette"), this);
	loadPaletteAct->setEnabled(true);
	connect(loadPaletteAct, SIGNAL(triggered()), this, SLOT(loadPalette()));

	savePaletteAct = new QAction(tr("&Save Palette"), this);
	savePaletteAct->setEnabled(true);
	connect(savePaletteAct, SIGNAL(triggered()), this, SLOT(savePalette()));

	sealingLayerAct = new QAction(tr("&Sealing Layer"), this);
	sealingLayerAct->setEnabled(true);
	connect(sealingLayerAct, SIGNAL(triggered()), canvas, SLOT(sealingLayer()));

	flipHorizontalLayerAct = new QAction(tr("&Flip Layer Horizontal"), this);
	flipHorizontalLayerAct->setEnabled(true);
	connect(flipHorizontalLayerAct, SIGNAL(triggered()), canvas, SLOT(flipHorizontalLayer()));

	flipVerticalLayerAct = new QAction(tr("&Flip Layer Vertical"), this);
	flipVerticalLayerAct->setEnabled(true);
	connect(flipVerticalLayerAct, SIGNAL(triggered()), canvas, SLOT(flipVerticalLayer()));

	grayScaleAct = new QAction(tr("&GrayScale Effect"), this);
	grayScaleAct->setEnabled(true);
	connect(grayScaleAct, SIGNAL(triggered()), canvas, SLOT(grayScaleLayer()));

	//this->setContentsMargins(5, 0, 5, 0);
	sepiaAct = new QAction(tr("&Sepia Effect"), this);
	sepiaAct->setEnabled(true);
	connect(sepiaAct, SIGNAL(triggered()), canvas, SLOT(sepiaLayer()));

	sepiaBlueAct = new QAction(tr("&Sepia Blue Effect"), this);
	sepiaBlueAct->setEnabled(true);
	connect(sepiaBlueAct, SIGNAL(triggered()), canvas, SLOT(sepiaBlueLayer()));

	sepiaGreenAct = new QAction(tr("&Sepia Green Effect"), this);
	sepiaGreenAct->setEnabled(true);
	connect(sepiaGreenAct, SIGNAL(triggered()), canvas, SLOT(sepiaGreenLayer()));

    showToolBarAct = new QAction(tr("&Tool bar"), this);
    showToolBarAct->setEnabled(true);
    showToolBarAct->setCheckable(true);
    showToolBarAct->setChecked(true);
    connect(showToolBarAct, SIGNAL(triggered()), this, SLOT(showToolbar()));

	showThumbAct = new QAction(tr("&Thumbnail"), this);
	showThumbAct->setEnabled(true);
	showThumbAct->setCheckable(true);
	showThumbAct->setChecked(true);
	connect(showThumbAct, SIGNAL(triggered()), this, SLOT(showThumbWindow()));

	showPaletteAct = new QAction(tr("&Palettes"), this);
	showPaletteAct->setEnabled(true);
	showPaletteAct->setCheckable(true);
	showPaletteAct->setChecked(true);
	connect(showPaletteAct, SIGNAL(triggered()), this, SLOT(showPaletteWindow()));

	showLayerAct = new QAction(tr("&Layer"), this);
	showLayerAct->setEnabled(true);
	showLayerAct->setCheckable(true);
	showLayerAct->setChecked(true);
	connect(showLayerAct, SIGNAL(triggered()), this, SLOT(showLayerWindow()));

	showHistoryAct = new QAction(tr("&History"), this);
	showHistoryAct->setEnabled(true);
	showHistoryAct->setCheckable(true);
	showHistoryAct->setChecked(false);
	connect(showHistoryAct, SIGNAL(triggered()), this, SLOT(showHistoryWindow()));

	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	toolbarGroup = new QActionGroup(this);
	
    moveAct = new QAction(QIcon(applicationDir.absolutePath() + "/Resources/images/move.png"), tr("&Move"), toolbarGroup);
	moveAct->setShortcut(tr("M"));
	moveAct->setCheckable(true);
	connect(moveAct, SIGNAL(triggered()), this, SLOT(useMove()));

    cropAct = new QAction(QIcon(applicationDir.absolutePath() + "/Resources/images/crop.png"), tr("&Crop"), toolbarGroup);
	cropAct->setShortcut(tr("C"));
	cropAct->setCheckable(true);
	connect(cropAct, SIGNAL(triggered()), this, SLOT(useCrop()));

    penAct = new QAction(QIcon(applicationDir.absolutePath() + "/Resources/images/pencil.png"), "Pen", toolbarGroup);
	penAct->setShortcut(tr("B"));
	//penAct->setDisabled(true);
	connect(penAct, SIGNAL(triggered()), this, SLOT(usePen()));

    eraserAct = new QAction(QIcon(applicationDir.absolutePath() + "/Resources/images/eraser.png"), "Eraser", toolbarGroup);
	eraserAct->setShortcut(tr("E"));
	eraserAct->setCheckable(true);
	connect(eraserAct, SIGNAL(triggered()), this, SLOT(useEraser()));

    selectionAct = new QAction(QIcon(applicationDir.absolutePath() + "/Resources/images/rectangle_selection.png"), "Selection", toolbarGroup);
	selectionAct->setShortcut(tr("M"));
	selectionAct->setCheckable(true);
	connect(selectionAct, SIGNAL(triggered()), this, SLOT(useRectSelection()));

    bucketAct = new QAction(QIcon(applicationDir.absolutePath() + "/Resources/images/bucket.png"), "Bucket", toolbarGroup);
	bucketAct->setShortcut(tr("G"));
	bucketAct->setCheckable(true);
	connect(bucketAct, SIGNAL(triggered()), this, SLOT(useBucket()));

    strawAct = new QAction(QIcon(applicationDir.absolutePath() + "/Resources/images/eyedropper.png"), "Straw", toolbarGroup);
	strawAct->setShortcut(tr("I"));
	strawAct->setCheckable(true);
	connect(strawAct, SIGNAL(triggered()), this, SLOT(useStraw()));

    dodgeAct = new QAction(QIcon(applicationDir.absolutePath() + "/Resources/images/dodge.png"), "Dodge", toolbarGroup);
	dodgeAct->setShortcut(tr("O"));
	dodgeAct->setCheckable(true);
	connect(dodgeAct, SIGNAL(triggered()), this, SLOT(useDodge()));

    zoomAct = new QAction(QIcon(applicationDir.absolutePath() + "/Resources/images/zoom.png"), "Zoom", toolbarGroup);
	zoomAct->setShortcut(tr("Z"));
	zoomAct->setCheckable(true);
	connect(zoomAct, SIGNAL(triggered()), this, SLOT(useZoom()));
}

void MainWindow::createMenus()
{
	QString ss("QMenu {background-color: #1b1b1c;}\
			   	QMenu::item:selected {\
				background: #3e3e40;\
				  }");


	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addSeparator();
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addAction(exportAct);

	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	fileMenu->setStyleSheet(ss);

	viewMenu = new QMenu(tr("&View"), this);
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addAction(normalSizeAct);
	//viewMenu->addSeparator();
	//viewMenu->addAction(fitToWindowAct);
	viewMenu->setStyleSheet(ss);

	helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(aboutAct);
	helpMenu->setStyleSheet(ss);

	operationMenu = new QMenu(tr("&Operation"), this);
	operationMenu->addAction(undoAction);
	operationMenu->addAction(redoAction);
	operationMenu->addSeparator();
	operationMenu->addAction(loadPaletteAct);
	operationMenu->addAction(savePaletteAct);
	operationMenu->addSeparator();
	operationMenu->addAction(sealingLayerAct);
	operationMenu->addAction(flipHorizontalLayerAct);
	operationMenu->addAction(flipVerticalLayerAct);
	operationMenu->addSeparator();
	operationMenu->addAction(grayScaleAct);
	operationMenu->addAction(sepiaAct);
	operationMenu->addAction(sepiaBlueAct);
	operationMenu->addAction(sepiaGreenAct);
	operationMenu->setStyleSheet(ss);

	windowsMenu = new QMenu(tr("&Windows"), this);
    windowsMenu->addAction(showToolBarAct);
	windowsMenu->addAction(showThumbAct);
	windowsMenu->addAction(showPaletteAct);
	windowsMenu->addAction(showLayerAct);
	windowsMenu->addAction(showHistoryAct);
	windowsMenu->setStyleSheet(ss);

	toolsMenu = new QMenu(tr("&Tools"), this);

	toolsMenu->addAction(moveAct);
	toolsMenu->addAction(penAct);
	toolsMenu->addAction(eraserAct);
	toolsMenu->addAction(selectionAct);
	toolsMenu->addAction(bucketAct);
	toolsMenu->addAction(cropAct);
	toolsMenu->addAction(strawAct);
	toolsMenu->addAction(dodgeAct);
	toolsMenu->addAction(zoomAct);
	toolsMenu->setStyleSheet(ss);

	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(operationMenu);
	menuBar()->addMenu(toolsMenu);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(windowsMenu);
	menuBar()->addMenu(helpMenu);
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
	if (e->modifiers() == Qt::AltModifier)
	{
		int  numDegrees = e->delta() / 8;
		int numSteps = numDegrees / 15;
		if (numSteps == 1)
		{
			canvas->zoomOut();
			this->updateStatueBar(canvas->getScaleFactor());
		}
		if (numSteps == -1)
		{
			canvas->zoomIn();
			this->updateStatueBar(canvas->getScaleFactor());
		}
	}
}

void  MainWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_X)
	{
		exchangeSwatches();
	}

	if (!canvas->isCreate())
	{
		return;
	}
	//Zoom out-> ctrl + +
	if (e->modifiers() & Qt::ControlModifier && e->key() == Qt::Key_Plus)
	{
		canvas->zoomOut();
	}

	//Cancel selection->Ctrl + D
	if (e->modifiers() & Qt::ControlModifier&& e->key() == Qt::Key_D)
	{
		canvas->cancelSelection();
	}

	//Sealing layer->Ctrl + Alt + Shift + E
	if (e->modifiers() & Qt::ControlModifier&&e->modifiers() & Qt::AltModifier&&e->modifiers() & Qt::ShiftModifier&& e->key() == Qt::Key_E)
	{
		canvas->sealingLayer();
	}

	//Create new layer by selection -> Ctrl +J
	if (e->modifiers() & Qt::ControlModifier&& e->key() == Qt::Key_J)
	{
		LayerManager* instance = LayerManager::getInstance();
		if (canvas->operationType == SELECTION1)
		{
			canvas->copySelection();
			instance->addNewLayer();
			canvas->pasteSelection();
			canvas->cancelSelection();
		}
		else{
			instance->duplicateLayer();
		}
	}
	
	if (e->key() == Qt::Key_Delete)
	{
		
		if (canvas->operationType == SELECTION1)
		{	//Fill selection ->Alt+Delete 
			if (e->modifiers() & Qt::AltModifier)
			{
				canvas->fillSelection();
			}
			//Delete selection -> delete
			else{
					canvas->deleteSelection();
			}
		}
	}

	if (e->key() == Qt::Key_Alt)
	{
		if (canvas->operationType == ZOOMOUT)
		{
            QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/zoomin_ico.png");
			QCursor cursor = QCursor(pixmap, -1, -1);
			setCursor(cursor);
			canvas->setOperationType(ZOOMIN);
		}
		if (canvas->operationType == DODGE)
		{
			canvas->setOperationType(DEEPEN);
		}
	}

	if (e->key() == Qt::Key_Enter)
	{
		if (canvas->operationType == CROP)
		{
			canvas->crop();
		}
	}


}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Alt)
	{
		if (canvas->operationType == ZOOMIN)
		{
            QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/zoomout_ico.png");
			QCursor cursor = QCursor(pixmap, -1, -1);
			setCursor(cursor);
			canvas->setOperationType(ZOOMOUT);
		}
	}
	if (canvas->operationType == DEEPEN)
	{
		canvas->setOperationType(DODGE);
	}
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
	if ((e->buttons() == Qt::LeftButton))
	{
	}
}

void MainWindow::useMove()
{
    QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/move_ico.png");
    qDebug()<<"useMove"<<pixmap.width();
	QCursor cursor = QCursor(pixmap, -1, -1);
	canvas->setOperationType(MOVE);
	setCursor(cursor);
}

void MainWindow::usePen()
{
	qDebug() << penAct->isEnabled();
    QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/pen_ico.png");
	QCursor cursor = QCursor(pixmap, -1, -1);
	canvas->setOperationType(PEN);
	setCursor(cursor);
}

void MainWindow::useEraser()
{
    QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/eraser_ico.png");
	QCursor cursor = QCursor(pixmap, -1, -1);
	canvas->setOperationType(ERASER);
	setCursor(cursor);
}

void MainWindow::useStraw()
{
    QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/straw_ico.png");
	QCursor cursor = QCursor(pixmap, -1, -1);
	canvas->setOperationType(STRAW);
	setCursor(cursor);
}

void MainWindow::useDodge()
{
    QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/pen_ico.png");
	QCursor cursor = QCursor(pixmap, -1, -1);
	canvas->setOperationType(DODGE);
	setCursor(cursor);
}

void MainWindow::useBucket()
{
    QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/bucket_ico.png");
	QCursor cursor = QCursor(pixmap, -1, -1);
	canvas->setOperationType(BUCKET);
	setCursor(cursor);
}


void MainWindow::useZoom()
{
    QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/zoomout_ico.png");
	QCursor cursor = QCursor(pixmap, -1, -1);
	canvas->setOperationType(ZOOMOUT);
	setCursor(cursor);
}

void MainWindow::useRectSelection()
{
    QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/rectangle_selection_ico.png");
	QCursor cursor = QCursor(pixmap, -1, -1);
	canvas->setOperationType(SELECTION1);
	setCursor(cursor);
}

void MainWindow::useCrop()
{
    QPixmap pixmap(applicationDir.absolutePath() + "/Resources/images/crop_ico.png");
	QCursor cursor = QCursor(pixmap, -1, -1);
	canvas->setOperationType(CROP);
	setCursor(cursor);
}

void MainWindow::updateStatueBar(float scaleFactor)
{
	zoomLabel->setText(QString::number(scaleFactor * 100) + "%");
}

void MainWindow::showToolbar()
{
    if (toolbar->isVisible())
    {
        toolbar->hide();
    }
    else
    {
        toolbar->show();
    }
}

void MainWindow::showThumbWindow()
{
	if (thumbnailPanel->isVisible())
	{
		thumbnailPanel->hide();
	}
	else
	{
		thumbnailPanel->show();
	}
}

void MainWindow::showPaletteWindow()
{
	if (palettePanel->isVisible())
	{
		palettePanel->hide();
	}
	else
	{
		palettePanel->show();
	}
}

void MainWindow::showLayerWindow()
{
	if (layerPanel->isVisible())
	{
		layerPanel->hide();
	}
	else
	{
		layerPanel->show();
	}
}

void MainWindow::showHistoryWindow()
{
	if (historyPanel->isVisible())
	{
		historyPanel->hide();
	}
	else
	{
		historyPanel->show();
	}
}


void MainWindow::loadPalette()
{
	QString fileName;
    QFileDialog* fd = new QFileDialog(this, tr("Load Palette File"), applicationDir.absolutePath() + "/Resources/palette", tr("Gimp Palette File(*.gpl)"));
	fd->setFileMode(QFileDialog::ExistingFiles);
	fd->setViewMode(QFileDialog::Detail);

	if (fd->exec() == QDialog::Accepted)
	{
		palettePanel->loadPalette(fd->selectedFiles().first());
	}

}

void MainWindow::savePalette()
{
	QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save As"), applicationDir.absolutePath() + "/Resources/palette",
		tr("GPL (*.gpl)"));
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		palettePanel->savePalette(fileName);
	}
}

void MainWindow::exportAs()
{
	ExportDlg *exportDlg;
	exportDlg = new ExportDlg();
	connect(exportDlg, SIGNAL(exportImage(QString &, int, int)), canvas, SLOT(save(QString &, int, int)));
	exportDlg->exec();
}

void MainWindow::exchangeSwatches()
{
	QColor tmp = foreSwatches->Color();
	foreSwatches->changeColor(backSwatches->Color());
	backSwatches->changeColor(tmp);
	foreSwatches->update();
	backSwatches->update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
	//qDebug() << "mOVE";
}

void MainWindow::disableActions()
{
	for (int i = 0; i < toolActionList->size(); ++i) {
		toolActionList->at(i)->setDisabled(true);
	}
	for (int i = 0; i < effectActionList->size(); ++i) {
		effectActionList->at(i)->setDisabled(true);
	}

	zoomInAct->setDisabled(true);
	zoomOutAct->setDisabled(true);
	normalSizeAct->setDisabled(true);
	saveAct->setDisabled(true);
	saveAsAct->setDisabled(true);
	exportAct->setDisabled(true);
}

void MainWindow::enableActions()
{
	for (int i = 0; i < toolActionList->size(); ++i) {
		toolActionList->at(i)->setDisabled(false);
	}
	for (int i = 0; i < effectActionList->size(); ++i) {
		effectActionList->at(i)->setDisabled(false);
	}

	zoomInAct->setDisabled(false);
	zoomOutAct->setDisabled(false);
	normalSizeAct->setDisabled(false);
	saveAct->setDisabled(false);
	saveAsAct->setDisabled(false);
	exportAct->setDisabled(false);
	layerPanel->enableBtns();
}