#include "thumbnailcanvas.h"

ThumbnailCanvas::ThumbnailCanvas(bool needFit, QWidget *parent)
: QWidget(parent), m_gridPixmap(1000, 1000), needFitCanvas(needFit)
{
	//QPixmap pixmap("images\\sample.jpg");
	//m_pixmap = pixmap.scaledToWidth(200);
	QImage gridImage(1000, 1000, QImage::Format_RGB32);
	QRgb grey = QColor(204, 204, 204).rgb();
	QRgb white = QColor(255, 255, 255).rgb();
	for (int i = 0; i < 1000; i++)
	for (int j = 0; j < 1000; j++)
	{
		int tmpX = i % 10;
		int tmpY = j % 10;
		if (tmpX < 5)
		{
			gridImage.setPixel(i, j, tmpY < 5 ? grey : white);
		}
		else
		{
			gridImage.setPixel(i, j, tmpY < 5 ? white : grey);
		}
	}

	m_gridPixmap = QPixmap::fromImage(gridImage);
}

ThumbnailCanvas::~ThumbnailCanvas()
{

}

void ThumbnailCanvas::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
	
	//paint grid
	if (m_pixmap.isNull())
	{
		return;
	}

	QPainter painter(this);
	QSize pixSize = m_pixmap.size();
	QPoint topleft;

	QBrush brush;
	brush.setTexture(m_gridPixmap);

	QPixmap scaledPix = m_pixmap.scaled(event->rect().size(), Qt::KeepAspectRatio);

	if (event->rect().width() > event->rect().height())
	{
		topleft.setX((event->rect().width() - scaledPix.width()) / 2);
	}
	else
	{
		topleft.setY((event->rect().height() - scaledPix.height()) / 2);
	}

	painter.fillRect(topleft.x(), topleft.y(), scaledPix.width(), scaledPix.height(), brush);
	if (needFitCanvas)
	{
		painter.drawPixmap(topleft, scaledPix);
	}
	else{
		qDebug() << m_pixmap.size();
		painter.drawPixmap(topleft, m_pixmap);
	}

}

void ThumbnailCanvas::mousePressEvent(QMouseEvent *event)
{

}

void ThumbnailCanvas::setPixmap(QPixmap p)
{
	m_pixmap = p;
}