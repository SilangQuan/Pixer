#ifndef CANVAS_H
#define CANVAS_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QColor>
#include <QPen>
#include <QWidget>
#include <QBrush>
#include <QTimer>
#include <QMessageBox>
#include <QUndoStack>
#include <QUndoView>
#include <qmath.h>

#include "common.h"
#include "selectionrect.h"
#include "layermanager.h"
#include "imageprocessor.h"
#include "commands.h"


class Canvas : public QWidget
{
	Q_OBJECT

public:
	Canvas(QWidget  *parent = 0);
	~Canvas();
	QSize sizeHint() const;
	QImage getMergedImage();
	void paint(int x, int j, QColor c);
    void paint(const QPoint &p, QColor c);
	void crop();
    void erasePixel(const QPoint &p);
    QColor strawColor(const QPoint &p);
	void initWithFile(QString &path);
	float getScaleFactor() const;
	QString getResolution() const;
	QSize getScaledSize() const;
	void save(QString& path);
	QColor getColor(int x, int y);
	void fillSelection();
	void cancelSelection();
	void copySelection();
	void pasteSelection();
	void deleteSelection();
	bool isCreate();
	void extendImage(int deltaX, int deltaY, QImage* image);
	QUndoStack *getUndoStack();

	void setOperationType(OPERATIONTYPE t);

	OPERATIONTYPE operationType;

private:
	float scaleFactorMax = 32.0;
	float scaleFactorMin = 0.125;
	int borderOffset;
	bool isBlackStart;
	QImage m_surface;
	QImage m_clipboard;
	QImage m_prediction;
	QPixmap m_pixmap;
	QPixmap m_gridPixmap;
	int m_width;
	int m_height;
	COLORMODE m_mode;
	double m_scaleFactor;
	QColor foreColor;
	QColor backColor;

	bool isCreated;
	bool isInitialised;
	QSize initialSize;
	SelectionRect selectionRect;
	SelectionRect cropRect;
	LayerManager* layerManager;

	QPoint moveStartPoint;
	QPoint moveStartAnchor;
	//Judge screenPoint p in image or not
    bool isContained(const QPoint &p);
	//Map screen point to m_image point
    QPoint mapToPixmap(const QPoint &p);
	//Stroke when use slection tool.
	void paintSelectionBorder(QPainter &painter, QRect rect);
	
	QTimer *repaintTimer;

	QUndoStack *undoStack;

	PaintCommand *paintCommand;
	EraserCommand *eraserCommand;

protected:
	void paintEvent(QPaintEvent* e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	
public slots:
	void setConfig(int w, int h, COLORMODE m);
	void save(QString&path, int width, int height);
	void setScaleFactor(float factor);
	void zoomIn();
	void zoomOut();
	void setForeColor(QColor c);
	void setBackColor(QColor c);
	void contentToPixmap(QPixmap &p);
	void sealingLayer();
	void flipHorizontalLayer();
	void flipVerticalLayer();
	void grayScaleLayer();
	void sepiaLayer();
	void sepiaGreenLayer();
	void sepiaBlueLayer();
	void updateCanvasAndThumb();

	void undo();
	void redo();

private slots:
	void updateSelectionBorder();

signals:
	void colorStrawed(QColor c);
	void scaleFactorChanged(float factor); 
	void canvasChanged();
	void canvasInitialised();
};

#endif // CANVAS_H
