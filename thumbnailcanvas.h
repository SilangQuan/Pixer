#ifndef THUMBNAILCANVAS_H
#define THUMBNAILCANVAS_H

#include <QImage>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QColor>
#include <QPen>
#include <QWidget>
#include <QBrush>

class ThumbnailCanvas : public QWidget
{
	Q_OBJECT

public:
	ThumbnailCanvas(bool needCanvas, QWidget *parent = 0);
	~ThumbnailCanvas();
	void setPixmap(QPixmap pixmap);

private:
	QPixmap m_pixmap;
	QPixmap m_gridPixmap;
	bool needFitCanvas;

protected:
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent *event);
	
};

#endif // THUMBNAILCANVAS_H
