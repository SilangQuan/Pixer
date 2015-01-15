#include "canvas.h"

Canvas::Canvas(QWidget *parent)
: QWidget(parent), m_gridPixmap(2000,2000)
{
	isCreated = false;
	isInitialised = false;
	isBlackStart = false;
	m_scaleFactor = 1.0;
	operationType = NORMAL;
	borderOffset = 0;
	layerManager = LayerManager::getInstance();
	undoStack = new QUndoStack(this);
	undoStack->setUndoLimit(20);
	setMouseTracking(true);
	
	QImage gridImage(2000, 2000, QImage::Format_RGB32);
	QRgb grey = QColor(204, 204, 204).rgb();
	QRgb white = QColor(255, 255, 255).rgb();
	for (int i = 0; i < 2000; i++)
		for (int j = 0; j < 2000; j++)
		{
			int tmpX = i % 20;
			int tmpY = j % 20;
			if (tmpX < 10)
			{
				gridImage.setPixel(i, j, tmpY < 10 ? grey : white);
			}
			else
			{
				gridImage.setPixel(i, j, tmpY < 10 ? white : grey);
			}
		}

	m_gridPixmap = QPixmap::fromImage(gridImage);

	foreColor = QColor(255, 255, 255);
	backColor = foreColor;

	moveStartPoint = QPoint(-1, -1);

	repaintTimer = new QTimer();
	repaintTimer->setInterval(350);
	connect(repaintTimer, SIGNAL(timeout()), this, SLOT(updateSelectionBorder()));

}

Canvas::~Canvas()
{

}

QImage Canvas::getMergedImage()
{
	QImage image(m_width, m_height, QImage::Format_ARGB32);
	image.fill(qRgba(0, 0, 0, 0));
	QList<LayerItem>* layerList = layerManager->getpLayerList();

	QPainter mergePainter(&image);

	for (QList<LayerItem>::iterator it = --layerList->end(); it != --layerList->begin(); it--)
	{
		if (it->isShow)
		{
			mergePainter.drawImage(it->anchor.rx(), it->anchor.ry(), it->thumbnail);
		}
	}
	return image;
}


void Canvas::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);

	if (m_surface.isNull())
	{
		return;
	}
	else
	{
		m_surface = this->getMergedImage();
		m_pixmap = QPixmap::fromImage(m_surface);
	}

	QPainter painter(this);
	QBrush brush;
	brush.setTexture(m_gridPixmap);
	//painter.setBrush(&brush);

	if (isCreated)
	{
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QSize pixSize = m_pixmap.size();

		//For canvas's size not change when window's size change.
		if (!isInitialised)
		{
			initialSize = event->rect().size();	
			m_scaleFactor = 1.0 * initialSize.width() / m_pixmap.width();
		    emit scaleFactorChanged(m_scaleFactor);
			isInitialised = true;
			pixSize.scale(initialSize, Qt::KeepAspectRatio);
		}
		else
		{
			pixSize.scale(m_scaleFactor * QSize(m_width, m_height), Qt::KeepAspectRatio);
		}
		
		this->setMinimumSize(pixSize);

		QPoint topleft;
		topleft.setX((this->width() - pixSize.width()) / 2);
		topleft.setY((this->height() - pixSize.height()) / 2);
		//painter.fillRect(rect(), brush);
		painter.fillRect(topleft.x(),topleft.y(), pixSize.width(),pixSize.height(), brush);
		//painter.drawPixmap(topleft, m_pixmap);

		painter.drawPixmap(topleft, m_pixmap.scaled(pixSize, Qt::KeepAspectRatio));
		painter.drawPixmap(topleft, QPixmap::fromImage(m_prediction).scaled(pixSize, Qt::KeepAspectRatio));

		if (operationType == CROP)
		{
			QPen pen;
			pen.setBrush(Qt::red);
		//	pen.setStyle(Qt::DashLine);
			pen.setWidth(1);
			painter.setPen(pen);
			QRect rectToDraw;
			//start point in the left to the end point.
			if (cropRect.startPoint().x() < cropRect.endPoint().x())
			{
				if (cropRect.startPoint().y() < cropRect.endPoint().y())
				{
					//start point in the top to the end point.
					rectToDraw = QRect(topleft.x() + cropRect.startPoint().x() * m_scaleFactor, topleft.y() + cropRect.startPoint().y() * m_scaleFactor, cropRect.width() * m_scaleFactor, cropRect.height() * m_scaleFactor);
				}
				else{
					//start point in the bottom to the end point.
					rectToDraw = QRect(topleft.x() + cropRect.startPoint().x() * m_scaleFactor, topleft.y() + cropRect.endPoint().y() * m_scaleFactor, cropRect.width() * m_scaleFactor, cropRect.height() * m_scaleFactor);
				}
			}
			else
			{
				if (cropRect.startPoint().y() > cropRect.endPoint().y())
				{
					rectToDraw = QRect(topleft.x() + cropRect.endPoint().x() * m_scaleFactor, topleft.y() + cropRect.endPoint().y() * m_scaleFactor, cropRect.width() * m_scaleFactor, cropRect.height() * m_scaleFactor);
				}
				else{
					rectToDraw = QRect(topleft.x() + cropRect.endPoint().x() * m_scaleFactor, topleft.y() + cropRect.startPoint().y() * m_scaleFactor, cropRect.width() * m_scaleFactor, cropRect.height() * m_scaleFactor);
				}
			}
			painter.drawRect(rectToDraw);
		}

		if (operationType == SELECTION1 || operationType == BUCKET)
		{
			QRect rectToDraw;
			//start point in the left to the end point.
			if (selectionRect.startPoint().x() < selectionRect.endPoint().x())
			{
				if (selectionRect.startPoint().y() < selectionRect.endPoint().y())
				{
					//start point in the top to the end point.
					rectToDraw = QRect(topleft.x() + selectionRect.startPoint().x() * m_scaleFactor, topleft.y() + selectionRect.startPoint().y() * m_scaleFactor, selectionRect.width() * m_scaleFactor, selectionRect.height() * m_scaleFactor);
				}
				else{
					//start point in the bottom to the end point.
					rectToDraw = QRect(topleft.x() + selectionRect.startPoint().x() * m_scaleFactor, topleft.y() + selectionRect.endPoint().y() * m_scaleFactor, selectionRect.width() * m_scaleFactor, selectionRect.height() * m_scaleFactor);
				}
			}
			else
			{
				if (selectionRect.startPoint().y() > selectionRect.endPoint().y())
				{
					rectToDraw = QRect(topleft.x() + selectionRect.endPoint().x() * m_scaleFactor, topleft.y() + selectionRect.endPoint().y() * m_scaleFactor, selectionRect.width() * m_scaleFactor, selectionRect.height() * m_scaleFactor);
				}
				else{
					rectToDraw = QRect(topleft.x() + selectionRect.endPoint().x() * m_scaleFactor, topleft.y() + selectionRect.startPoint().y() * m_scaleFactor, selectionRect.width() * m_scaleFactor, selectionRect.height() * m_scaleFactor);
				}
			}
			paintSelectionBorder(painter, rectToDraw);
		}
	}
}

void Canvas::setConfig(int w, int h, COLORMODE m)
{
	m_width = w;
	m_height = h;
	m_mode = m;
	m_surface = QImage(w, h, QImage::Format_ARGB32);
	m_clipboard = QImage(w, h, QImage::Format_ARGB32);
	m_prediction = QImage(w, h, QImage::Format_ARGB32);

	QColor transparent(0, 0, 0, 0);
	m_prediction.fill(transparent);
	m_surface.fill(transparent);
	m_clipboard.fill(transparent);

	m_pixmap = QPixmap::fromImage(m_surface);

	isCreated = true;

	this->update();

	//Set layerSize in LayerManager.
	layerManager->setLayerSize(QSize(w, h));
	layerManager->addNewLayer();

	//Update thumbnail canvas.
	emit canvasChanged();
	emit canvasInitialised();
}

void Canvas::initWithFile(QString &path)
{
	m_surface = QImage(path);
	m_width = m_surface.width();
	m_height = m_surface.height();
	m_clipboard = QImage(m_width, m_height, QImage::Format_ARGB32);
	m_prediction = QImage(m_width, m_height, QImage::Format_ARGB32);

	QColor transparent(0, 0, 0, 0);
	m_prediction.fill(transparent);
	m_clipboard.fill(transparent);
	m_pixmap = QPixmap::fromImage(m_surface);

	layerManager->setLayerSize(QSize(m_width, m_height));
	layerManager->addNewLayer();

	*(layerManager->getSelectedLayer()) = m_surface;
	isCreated = true;
	this->update();
	emit canvasChanged();
	emit canvasInitialised();
}

QSize Canvas::sizeHint() const
{
	return m_pixmap.size();
}

void Canvas::zoomIn()
{
	m_scaleFactor *= 0.9;
	m_scaleFactor = m_scaleFactor < scaleFactorMin ? scaleFactorMin : m_scaleFactor;
	emit scaleFactorChanged(m_scaleFactor);
	this->update();
}

void Canvas::zoomOut()
{
	m_scaleFactor *= 1.1;
	m_scaleFactor = m_scaleFactor > scaleFactorMax ? scaleFactorMax : m_scaleFactor;
	emit scaleFactorChanged(m_scaleFactor);
	this->update();
}

void Canvas::paint(int x, int y, QColor c)
{
	LayerItem *item = layerManager->getSelectItem();
	item->thumbnail.setPixel(x - item->anchor.rx(), y - item->anchor.ry(), c.rgba());
	this->update();
}

void Canvas::paint(const QPoint &p, QColor c)
{
	LayerItem *item = layerManager->getSelectItem();
	item->thumbnail.setPixel(p.x() - item->anchor.rx(), p.y() - item->anchor.ry(), c.rgba());
	this->update();
}

void Canvas::erasePixel(const QPoint &p)
{
	QColor c(0, 0, 0, 0);
	LayerManager* instance = LayerManager::getInstance();
	QImage* selectedImage = instance->getSelectedLayer();
	selectedImage->setPixel(p.x(), p.y(), c.rgba());
	this->update();
}

QColor Canvas::strawColor(const QPoint &p)
{
	return QColor::fromRgba(m_surface.pixel(p));
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
	int selectedIndex = layerManager->getSelectedLayerIndex();
	if ((event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton) && isContained((event->pos())))
	{
		switch (operationType)
		{
		case NORMAL:
			break;
		case MOVE:
			if (event->button() == Qt::LeftButton)
			{
                moveStartPoint = mapToPixmap(event->pos());
				moveStartAnchor = layerManager->getSelectItem()->anchor;
			}
			break;
		case PEN:
			if (!layerManager->getpLayerList()->at(selectedIndex).isShow)
			{
				QMessageBox msgBox(QMessageBox::Warning, tr("Warning!"),
					"Layer selected is not visible.", 0, this);
				msgBox.exec();
			}
			else {
				paintCommand = new PaintCommand(layerManager->getSelectedLayer());
				if (event->button() == Qt::LeftButton)
				{
					paint(mapToPixmap(event->pos()), foreColor);
				}
				else{
					paint(mapToPixmap(event->pos()), backColor);
				}
				m_prediction.fill(qRgba(0, 0, 0, 0));
			}
			
			break;
		case ERASER:
			if (!layerManager->getpLayerList()->at(selectedIndex).isShow)
			{
				QMessageBox msgBox(QMessageBox::Warning, tr("Warning!"),
					"Layer selected is not visible.", 0, this);
				msgBox.exec();
			}
			else
			{
				if (event->button() == Qt::LeftButton)
				{
					eraserCommand = new EraserCommand(layerManager->getSelectedLayer());
					erasePixel(mapToPixmap(event->pos()));
					m_prediction.fill(qRgba(0, 0, 0, 0));
				}
			}
			break;
		case SELECTION1:
			{
				QPoint pixelPosition = mapToPixmap(event->pos());
				 //selectionRect.setStart(event->pos());
				 selectionRect.setStart(QPoint( pixelPosition.rx(), pixelPosition.ry()));
				 selectionRect.setEnd(QPoint(pixelPosition.rx(), pixelPosition.ry()));
				break;
			}
		case BUCKET:
		{
				QPoint pixelPosition = mapToPixmap(event->pos());
				if (selectionRect.contains(pixelPosition))
				{
					fillSelection();
				}
				break;
		}
		case CROP:
			{
				 QPoint pixelPosition = mapToPixmap(event->pos());
				 //selectionRect.setStart(event->pos());
				cropRect.setStart(QPoint(pixelPosition.rx(), pixelPosition.ry()));
				cropRect.setEnd(QPoint(pixelPosition.rx(), pixelPosition.ry()));
				 break;
			}
			break;
			
		case STRAW:
			{
				QColor c = strawColor(mapToPixmap(event->pos()));
				if (c.alpha() == 0)
				{
					return;
				}
			    else
				{
					 emit(colorStrawed(c));
				}
				 break;
			}
		
		case DODGE:
			if (!layerManager->getpLayerList()->at(selectedIndex).isShow)
			{
				QMessageBox msgBox(QMessageBox::Warning, tr("Warning!"),
					"Layer selected is not visible.", 0, this);
				msgBox.exec();
			}
			else {
				if (event->button() == Qt::LeftButton)
				{
					QImage *image = layerManager->getSelectedLayer();
					DodgeCommand *dodgeCommand = new DodgeCommand(image);

					QColor originColor = QColor::fromRgba(image->pixel(mapToPixmap(event->pos())));

					qDebug() << "originColor" << originColor<<endl;
					QColor newColor(originColor.red() + 20 > 255 ? 255 : originColor.red() + 20, originColor.green() + 20 > 255 ? 255 : originColor.green() + 20, originColor.blue() + 20 > 255 ? 255 : originColor.blue() + 20, originColor.alpha());
					image->setPixel(mapToPixmap(event->pos()), newColor.rgba());
					m_prediction.fill(qRgba(0, 0, 0, 0));
					this->update();
					dodgeCommand->setNewImage(image);
					undoStack->push(dodgeCommand);
				}
			}

			break;

		case DEEPEN:
			if (!layerManager->getpLayerList()->at(selectedIndex).isShow)
			{
				QMessageBox msgBox(QMessageBox::Warning, tr("Warning!"),
					"Layer selected is not visible.", 0, this);
				msgBox.exec();
			}
			else {
				if (event->button() == Qt::LeftButton)
				{
					QImage *image = layerManager->getSelectedLayer();
					DeepenCommand *deepenCommand = new DeepenCommand(image);
					QColor originColor = QColor::fromRgba(image->pixel(mapToPixmap(event->pos())));

					qDebug() << "originColor" << originColor << endl;
					QColor newColor(originColor.red() - 20 > 0 ? originColor.red() - 20 : 0, originColor.green() - 20 > 0 ? originColor.green() - 20 : 0, originColor.blue() - 20 > 0 ? originColor.blue() - 20 : 0, originColor.alpha());
					image->setPixel(mapToPixmap(event->pos()), newColor.rgba());
					m_prediction.fill(qRgba(0, 0, 0, 0));
					this->update();

					deepenCommand->setNewImage(image);
					undoStack->push(deepenCommand);
				}
			}

			break;
		}
		
		emit canvasChanged();
	}

}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
	switch (operationType)
	{
	case NORMAL:
		break;
	case MOVE:
		if (isContained(event->pos()))
		{
			if (event->buttons() & Qt::LeftButton)
			{
				//qDebug() << "Delta" << mapToPixmap(event->pos()).rx() - moveStartPoint.rx() << mapToPixmap(event->pos()).ry() - moveStartPoint.ry();
				int deltaX = mapToPixmap(event->pos()).rx() - moveStartPoint.rx();
				int deltaY = mapToPixmap(event->pos()).ry() - moveStartPoint.ry();
				LayerItem *item = layerManager->getSelectItem();
				item->anchor.setX(moveStartAnchor.rx() + deltaX);
				item->anchor.setY(moveStartAnchor.ry() + deltaY);
				qDebug() << "MOVE" << deltaX << deltaY;
				this->update();
			}
		}
		break;
	case PEN:
		if (isContained(event->pos()))
		{
			if (event->buttons() & Qt::LeftButton)
			{
				paint(mapToPixmap(event->pos()), foreColor);
			}
			else if (event->buttons() & Qt::RightButton){
				paint(mapToPixmap(event->pos()), backColor);
			}
		
			m_prediction.fill(qRgba(0, 0, 0, 0));
			if (!(event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton))
			{
				m_prediction.setPixel(mapToPixmap(event->pos()), qRgba(255, 255, 255, 128));
				this->update();
			}
            else{
                m_prediction.fill(qRgba(0, 0, 0, 0));
                this->update();
            }
		}

		break;
	case ERASER:
		if (isContained(event->pos()))
		{
			if (event->buttons() & Qt::LeftButton)
			{
				erasePixel(mapToPixmap(event->pos()));
			}
			m_prediction.fill(qRgba(0, 0, 0, 0));
			if (!(event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton))
			{
				m_prediction.setPixel(mapToPixmap(event->pos()), qRgba(0, 0, 0, 128));
				this->update();
			}
		}
        else{
            m_prediction.fill(qRgba(0, 0, 0, 0));
            this->update();
        }
		
		break;
	case STRAW:
		break;
	case ZOOMOUT:

		break;

	case DEEPEN:
	case DODGE:
		m_prediction.fill(qRgba(0, 0, 0, 0));
		if (!(event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton))
		{
			m_prediction.setPixel(mapToPixmap(event->pos()), qRgba(255, 255, 255, 128));
			this->update();
        }
        else{
            m_prediction.fill(qRgba(0, 0, 0, 0));
            this->update();
        }
		break;

	case SELECTION1:
		if (event->buttons() & Qt::LeftButton)
		{
			if (isContained(event->pos()))
			{
				QPoint pixelPosition = mapToPixmap(event->pos());
				if (pixelPosition.rx() < selectionRect.startPoint().x() && pixelPosition.ry() < selectionRect.startPoint().y())
				{
					selectionRect.setEnd(QPoint((pixelPosition.rx()), (pixelPosition.ry())));
				}
				else if (pixelPosition.rx() < selectionRect.startPoint().x() && pixelPosition.ry() > selectionRect.startPoint().y()){
					selectionRect.setEnd(QPoint((pixelPosition.rx()), (pixelPosition.ry() + 1)));
				}
				else if (pixelPosition.rx() > selectionRect.startPoint().x() && pixelPosition.ry() < selectionRect.startPoint().y())
				{
					selectionRect.setEnd(QPoint((pixelPosition.rx() + 1), (pixelPosition.ry())));
				}
				else
				{
					selectionRect.setEnd(QPoint((pixelPosition.rx() + 1), (pixelPosition.ry() + 1)));
				}

				update();
			}
		}
		break;
	case CROP:
		if (event->buttons() & Qt::LeftButton)
		{
			if (isContained(event->pos()))
			{
				QPoint pixelPosition = mapToPixmap(event->pos());
				if (pixelPosition.rx() < cropRect.startPoint().x() && pixelPosition.ry() < cropRect.startPoint().y())
				{
					cropRect.setEnd(QPoint((pixelPosition.rx()), (pixelPosition.ry())));
				}
				else if (pixelPosition.rx() < cropRect.startPoint().x() && pixelPosition.ry() > cropRect.startPoint().y()){
					cropRect.setEnd(QPoint((pixelPosition.rx()), (pixelPosition.ry() + 1)));
				}
				else if (pixelPosition.rx() > cropRect.startPoint().x() && pixelPosition.ry() < cropRect.startPoint().y())
				{
					cropRect.setEnd(QPoint((pixelPosition.rx() + 1), (pixelPosition.ry())));
				}
				else
				{
					cropRect.setEnd(QPoint((pixelPosition.rx() + 1), (pixelPosition.ry() + 1)));
				}
				update();
			}
		}
	default:
		break;
	}
}



void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
		switch (operationType)
		{
		case NORMAL:
			break;
		case MOVE:
			if (event->button() == Qt::LeftButton)
			{
					 //qDebug() << "Delta" << mapToPixmap(event->pos()).rx() - moveStartPoint.rx() << mapToPixmap(event->pos()).ry() - moveStartPoint.ry();
					 int deltaX = mapToPixmap(event->pos()).rx() - moveStartPoint.rx();
					 int deltaY = mapToPixmap(event->pos()).ry() - moveStartPoint.ry();
					 qDebug() << "Delta" << deltaX << deltaY;
					 
					 LayerItem *item = layerManager->getSelectItem();
					 int relativeDeltaX = moveStartAnchor.rx() + deltaX;
					 int relativeDeltaY = moveStartAnchor.ry() + deltaY;
					 int extendWidth = item->thumbnail.width() - m_width - qAbs(item->anchor.rx());
					 int extendHeight = item->thumbnail.height() - m_height - qAbs(item->anchor.ry());

					 if (deltaX > 0 && deltaY > 0)
					 {
						 if (relativeDeltaX > 0 || relativeDeltaY > 0)
						 {
							 extendImage(relativeDeltaX >0 ? relativeDeltaX : 0, relativeDeltaY > 0 ? relativeDeltaY : 0, layerManager->getSelectedLayer());
						 }
					 }
					 if (deltaX < 0 && deltaY < 0)
					 {

						 if (extendWidth<0 || extendHeight<0)
						 {
							 qDebug()<<"Extend2: "<<extendWidth << extendHeight;
							 extendImage(extendWidth >0 ? 0 : extendWidth, extendHeight > 0 ? 0:extendHeight , layerManager->getSelectedLayer());
						 }
					 }
					 if (deltaX>0 && deltaY<0)
					 {
						 if (relativeDeltaX > 0 || extendHeight<0)
						 {
							 extendImage(relativeDeltaX >0 ? relativeDeltaX : 0, extendHeight > 0 ? 0 : extendHeight, layerManager->getSelectedLayer());
						}
					 }
					 if (deltaX < 0 && deltaY>0)
					 {
						 if (extendWidth < 0 || relativeDeltaY>0)
						 {
							 extendImage(extendWidth >0 ? 0 : extendWidth, relativeDeltaY > 0 ? relativeDeltaY : 0,  layerManager->getSelectedLayer());
						 }
					 }
					 if ( 0 == deltaX && deltaY>0)
					 {
						 if (relativeDeltaY>0)
						 {
							 extendImage(0, relativeDeltaY > 0 ? relativeDeltaY : 0, layerManager->getSelectedLayer());
						 }
					 }
					 if (0 == deltaX && deltaY < 0)
					 {
						 if (extendHeight<0)
						 {
							 extendImage(0, extendHeight > 0 ? 0 : extendHeight , layerManager->getSelectedLayer());
						 }
					 }
					 if (deltaX >0 && 0 == deltaY)
					 {
						 if (relativeDeltaX > 0)
						 {
							 extendImage(relativeDeltaX > 0 ? relativeDeltaX : 0, 0, layerManager->getSelectedLayer());
						 }
					 }
					 if (deltaX < 0 && 0 == deltaY)
					 {
						 if (extendWidth<0)
						 {
							 extendImage(extendWidth > 0 ? 0 : extendWidth, 0, layerManager->getSelectedLayer());
						 }
					 }

					 item->anchor = QPoint(relativeDeltaX >0 ? 0 : relativeDeltaX, relativeDeltaY > 0? 0 :relativeDeltaY);

					 break;
			}
		case PEN:
			if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
			{
				paintCommand->setNewImage(layerManager->getSelectedLayer());
				undoStack->push(paintCommand);
			}
			break;
		case ERASER:
			if (event->button() == Qt::LeftButton)
			{
				eraserCommand->setNewImage(layerManager->getSelectedLayer());
				undoStack->push(eraserCommand);
			}
			break;
		case STRAW:
			break;
		case ZOOMOUT:
			if (event->button() == Qt::LeftButton)
			{
				zoomOut();
			}
			break;
		case ZOOMIN:
			if (event->button() == Qt::LeftButton)
			{
				zoomIn();
			}
			break;

		case SELECTION1:
			if (event->button() == Qt::LeftButton && isContained(event->pos()))
			{
				QPoint pixelPosition = mapToPixmap(event->pos());
				selectionRect.setEnd(QPoint((pixelPosition.rx() + 1), (pixelPosition.ry() + 1)));
				update();
			}
			break;
		default:
			break;
		}
		emit canvasChanged();
	
}

bool Canvas::isContained(const QPoint &p)
{
	QSize s = QSize(m_scaleFactor * m_width, m_scaleFactor *m_height);

	if ((s.height() > this->rect().height()) && (s.width() > this->rect().width()))
	{
		return true;
	}

	QPoint topleft;
	topleft.setX((this->width() - s.width()) / 2);
	topleft.setY((this->height() - s.height()) / 2);

	QRect rect(topleft, s);
	return rect.contains(p);
}

void Canvas::setForeColor(QColor c)
{
	this->foreColor = c;
}

void Canvas::setBackColor(QColor c)
{
	this->backColor = c;
}

QPoint Canvas::mapToPixmap(const QPoint &screenPoint)
{
	QSize pixmapSize = QSize(m_scaleFactor * m_width, m_scaleFactor *m_height);

	//Get the position of screenPoint to the pixmap in show.  
	QPoint tmpPos;
	if (pixmapSize.width() > this->width() && pixmapSize.height() > this->height())
	{
		qDebug() << "Case 1";
		tmpPos.setX(pixmapSize.width() - (this->width() - screenPoint.x()));
		tmpPos.setY(pixmapSize.height() - (this->height() - screenPoint.y()));
	}
	else if (pixmapSize.width() < this->width() && pixmapSize.height() > this->height())
	{
		qDebug() << "Case 2";
		tmpPos.setX(screenPoint.x() - (this->width() - pixmapSize.width()) / 2);
		tmpPos.setY(pixmapSize.height() - (this->height() - screenPoint.y()));
	}
	else if (pixmapSize.width() > this->width() && pixmapSize.height() < this->height())
	{
		qDebug() << "Case3";
		tmpPos.setX(pixmapSize.width() - (this->width() - screenPoint.x()));
		tmpPos.setY(screenPoint.y() - (this->height() - pixmapSize.height()) / 2);
	}
	else{
		QPoint topleft;
		topleft.setX((this->width() - pixmapSize.width()) / 2);
		topleft.setY((this->height() - pixmapSize.height()) / 2);
		tmpPos.setX(screenPoint.x() - topleft.x());
		tmpPos.setY(screenPoint.y() - topleft.y());
	}
	//return the position to the real pixmap.*/  
	return QPoint(tmpPos.x() / m_scaleFactor, tmpPos.y() / m_scaleFactor);
}

float Canvas::getScaleFactor() const
{
	return m_scaleFactor;
}

QSize Canvas::getScaledSize() const
{
	return m_scaleFactor * QSize(m_width, m_width);
}

QString Canvas::getResolution() const
{
	return QString::number(m_width) + " x " + QString::number(m_height);
}

void Canvas::setScaleFactor(float factor)
{
	this->m_scaleFactor = factor;
	this->update();
}

void Canvas::save(QString& path)
{
	QImage mergedImage = this->getMergedImage();

	if (!path.endsWith("png"))
	{
		QImage image(m_width, m_height, QImage::Format_ARGB32);
		image.fill(QColor(Qt::white).rgb());
		QPainter mergePainter(&image);
		mergePainter.drawImage(0, 0, mergedImage);
		image.save(path);
	}
	else{
		mergedImage.save(path);
	}
	
}

void Canvas::save(QString&path, int width, int height)
{
	QImage mergedImage = this->getMergedImage();

	if (!path.endsWith("png"))
	{
		QImage image(m_width, m_height, QImage::Format_ARGB32);
		image.fill(QColor(Qt::white).rgb());
		QPainter mergePainter(&image);
		mergePainter.drawImage(0, 0, mergedImage);
		image.scaledToWidth(width).save(path);
	}
	else{
		mergedImage.scaledToWidth(width).save(path);
	}

}

void Canvas::contentToPixmap(QPixmap &p)
{
	p = m_pixmap;
}




void Canvas::paintSelectionBorder(QPainter &painter, QRect rect)
{
	int startX = selectionRect.startPoint().x();
	int startY = selectionRect.startPoint().y();

	//Init a transparent QImage.
	QSize scaledSize = m_scaleFactor * QSize(m_width, m_width);
	QImage transparentImage(this->size(), QImage::Format_ARGB32);

	QColor transparent(0, 0, 0, 0);
	QColor black(0, 0, 0);
	QColor white(255, 255, 255);

	for (int i = 0; i < this->size().width(); i++)
		for (int j = 0; j < this->size().height(); j++)
		{
			transparentImage.setPixel(i, j, transparent.rgba());
		}

	bool isDrawBlack = true;
	
	//Draw left&right border.
	for (int i = 0; i < rect.height(); i++)
	{
		if (i <= borderOffset)
		{
			transparentImage.setPixel(rect.left() + rect.width(), rect.top() + i, isBlackStart ? black.rgb() : white.rgb());
			transparentImage.setPixel(rect.left(), rect.top() + i, isBlackStart ? black.rgb() : white.rgb());
			isDrawBlack = !isBlackStart;
		}
		else
		{
			transparentImage.setPixel(rect.left() + rect.width(), rect.top() + i, \
				isDrawBlack ? black.rgb() : white.rgb());
			transparentImage.setPixel(rect.left(), rect.top() + i, \
				isDrawBlack ? black.rgb() : white.rgb());
			if ((i - borderOffset) % 5 == 0)
			{
				isDrawBlack = !isDrawBlack;
			}
		}
	}

	//Draw top&bottom border;
	for (int i = 0; i < rect.width(); i++)
	{
		if (i <= borderOffset)
		{
			transparentImage.setPixel(rect.left() + i, rect.top(), isBlackStart ? black.rgb() : white.rgb());
			transparentImage.setPixel(rect.left() + i, rect.top() + rect.height(), isBlackStart ? black.rgb() : white.rgb());

			isDrawBlack = !isBlackStart;
		}
		else
		{
			transparentImage.setPixel(rect.left() + i, rect.top(), \
				isDrawBlack ? black.rgb() : white.rgb());
			transparentImage.setPixel(rect.left() + i, rect.top() + rect.height(), \
				isDrawBlack ? black.rgb() : white.rgb());
			if ((i - borderOffset) % 5 == 0)
			{
				isDrawBlack = !isDrawBlack;
			}
		}
	}
/*
	//Draw left&right border.
	for (int i = 0; i < selectionRect.height(); i++)
	{
		if (i <= borderOffset)
		{
			transparentImage.setPixel(startX + selectionRect.width(), startY + i, isBlackStart ? black.rgb() : white.rgb());
			transparentImage.setPixel(startX, startY + i, isBlackStart ? black.rgb() : white.rgb());
			isDrawBlack = !isBlackStart;
		}
		else
		{
			transparentImage.setPixel(startX + selectionRect.width(), startY + i, \
				isDrawBlack ? black.rgb() : white.rgb());
			transparentImage.setPixel(startX, startY + i, \
				isDrawBlack ? black.rgb() : white.rgb());
			if ((i - borderOffset) % 5 == 0)
			{
				isDrawBlack = !isDrawBlack;
			}
		}
	}

	//Draw up border;
	for (int i = 0; i < selectionRect.width(); i++)
	{
		if (i <= borderOffset)
		{
			transparentImage.setPixel(startX + i, startY, isBlackStart ? black.rgb() : white.rgb());
			transparentImage.setPixel(startX + i, startY + selectionRect.height(), isBlackStart ? black.rgb() : white.rgb());

			isDrawBlack = !isBlackStart;
		}
		else
		{
			transparentImage.setPixel(startX + i, startY, \
				isDrawBlack ? black.rgb() : white.rgb());
			transparentImage.setPixel(startX + i, startY + selectionRect.height(), \
				isDrawBlack ? black.rgb() : white.rgb());
			if ((i - borderOffset) % 5 == 0)
			{
				isDrawBlack = !isDrawBlack;
			}
		}
	}*/

	painter.drawImage(0, 0, transparentImage);
}

QColor Canvas::getColor(int x, int y)
{
	QImage *selectedImage = layerManager->getSelectedLayer();
	return selectedImage->pixel(x, y);
}


void Canvas::updateSelectionBorder()
{
	borderOffset++;
	if (borderOffset > 4)
	{
		borderOffset = 0;
		isBlackStart = !isBlackStart;
	}
	this->repaint();
}

void Canvas::crop()
{
	if (cropRect.width() == 0 || cropRect.height() == 0)
	{
		return;
	}
	layerManager->crop(QRect(cropRect.startPoint(), cropRect.endPoint()));

	m_width = cropRect.width();
	m_height = cropRect.height();
	layerManager->setLayerSize(QSize(m_width, m_height));
	m_surface = QImage(m_width, m_height, QImage::Format_ARGB32);
	m_clipboard = QImage(m_width, m_height, QImage::Format_ARGB32);
	m_prediction = QImage(m_width, m_height, QImage::Format_ARGB32);
	m_prediction.fill(qRgba(0, 0, 0, 0));
	cropRect.reset();
	this->update();
	emit canvasChanged();
}
	
void Canvas::setOperationType(OPERATIONTYPE t)
{
	if (operationType == SELECTION1 && t != SELECTION1)
	{
		if (t != BUCKET)
		{
			repaintTimer->stop();
			selectionRect.reset();
			this->update();
		}
	}

	if (operationType == CROP && t != CROP)
	{
		cropRect.reset();
		this->update();
	}

	operationType = t;
	if (operationType == SELECTION1)
	{
		repaintTimer->start();
		cropRect.reset();
	}
}

void Canvas::fillSelection()
{
	FillCommand *fiilCommand =new FillCommand(layerManager->getSelectedLayer());
	QPoint start = selectionRect.startPoint();
	QPoint end = selectionRect.endPoint();
	QPoint topLeft = QPoint(start.rx() < end.rx() ? start.rx() : end.rx(), start.ry() < end.ry() ? start.ry() : end.ry());

	for (int i = 0; i < selectionRect.width(); i++)
	for (int j = 0; j < selectionRect.height();j++)
	{
		paint(QPoint(topLeft.rx() + i, topLeft.ry() + j), foreColor);
	}
	fiilCommand->setNewImage(layerManager->getSelectedLayer());
	undoStack->push(fiilCommand);
}

void Canvas::cancelSelection()
{
	repaintTimer->stop();
	selectionRect.reset();
	this->update();
}

void Canvas::copySelection()
{
	m_clipboard.fill(QColor(0, 0, 0, 0));
	QPoint start = selectionRect.startPoint();
	QPoint end = selectionRect.endPoint();
	QPoint topLeft = QPoint(start.rx() < end.rx() ? start.rx() : end.rx(), start.ry() < end.ry() ? start.ry() : end.ry());

	QImage* image = layerManager->getSelectedLayer();

	for (int i = 0; i < selectionRect.width(); i++)
	for (int j = 0; j < selectionRect.height(); j++)
	{
		m_clipboard.setPixel(topLeft.rx() + i, topLeft.ry() + j, image->pixel(topLeft.rx() + i, topLeft.ry() + j));
	}
}

void Canvas::pasteSelection()
{
	QPoint start = selectionRect.startPoint();
	QPoint end = selectionRect.endPoint();
	QPoint topLeft = QPoint(start.rx() < end.rx() ? start.rx() : end.rx(), start.ry() < end.ry() ? start.ry() : end.ry());
	QImage* image = layerManager->getSelectedLayer();

	for (int i = 0; i < selectionRect.width(); i++)
	for (int j = 0; j < selectionRect.height(); j++)
	{
		image->setPixel(topLeft.rx() + i, topLeft.ry() + j, m_clipboard.pixel(topLeft.rx() + i, topLeft.ry() + j));
	}
}


void Canvas::deleteSelection()
{
	QPoint start = selectionRect.startPoint();
	QPoint end = selectionRect.endPoint();
	QPoint topLeft = QPoint(start.rx() < end.rx() ? start.rx() : end.rx(), start.ry() < end.ry() ? start.ry() : end.ry());

	QImage* image = layerManager->getSelectedLayer();

	for (int i = 0; i < selectionRect.width(); i++)
	for (int j = 0; j < selectionRect.height(); j++)
	{
		image->setPixel(topLeft.rx() + i, topLeft.ry() + j, QColor(0, 0, 0, 0).rgba());
	}
}

void Canvas::sealingLayer()
{
	QImage* image = layerManager->getSelectedLayer();
	*image = getMergedImage();
}

bool Canvas::isCreate()
{
	return isCreated;
}



void Canvas::extendImage(int deltaX, int deltaY, QImage* image)
{
	QSize newSize = QSize(image->width() + qAbs(deltaX), image->height() + qAbs(deltaY));
	QImage tmpImage(newSize, QImage::Format_ARGB32);
	tmpImage.fill(qRgba(0, 0, 0, 0));
	QPainter painter(&tmpImage);
	painter.drawImage(deltaX > 0 ? deltaX : 0, deltaY > 0 ? deltaY : 0, *image);

	*image = tmpImage;
}


void Canvas::flipHorizontalLayer()
{
	ImageProcessor::flipHorizontal(layerManager->getSelectedLayer());
	emit canvasChanged();
}

void Canvas::flipVerticalLayer()
{
	ImageProcessor::flipVertical(layerManager->getSelectedLayer());
	emit canvasChanged();
}

void Canvas::grayScaleLayer()
{
	ImageProcessor::grayScale(layerManager->getSelectedLayer());
	emit canvasChanged();
}

void Canvas::sepiaLayer()
{
	ImageProcessor::sepia(layerManager->getSelectedLayer());
	emit canvasChanged();
}

void Canvas::sepiaGreenLayer()
{
	ImageProcessor::sepiaGreen(layerManager->getSelectedLayer());
	emit canvasChanged();
}

void Canvas::sepiaBlueLayer()
{
	ImageProcessor::sepiaBlue(layerManager->getSelectedLayer());
	emit canvasChanged();
}

QUndoStack *Canvas::getUndoStack()
{
	return undoStack;
}

void Canvas::updateCanvasAndThumb()
{
	this->update();
	emit canvasChanged();
}

void Canvas::undo()
{
	undoStack->undo();
	updateCanvasAndThumb();
}

void Canvas::redo()
{
	undoStack->redo();
	updateCanvasAndThumb();
}
