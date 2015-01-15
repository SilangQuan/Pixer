#include "layermanager.h"

LayerManager::LayerManager(QObject *parent)
{
	layerlist.clear();
}


LayerManager::~LayerManager()
{
}


LayerManager *  LayerManager::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new LayerManager();
	}
	return m_pInstance;
}

QList<LayerItem>* LayerManager::getpLayerList()
{
	return &layerlist;
}

void LayerManager::addNewLayer()
{
	LayerItem item;
	item.isShow = true;
	item.layerName = QString("Layer ") + QString::number(layerCounter++);
	item.transparence = 100;
	item.anchor = QPoint(0, 0);
	QImage tmpImage = QImage(layerSize, QImage::Format_ARGB32);
	QColor black(0, 0, 0, 0);
	black.setAlpha(0);

	for (int i = 0; i < layerSize.width(); i++)
		for (int j = 0; j < layerSize.height(); j++)
		{
			tmpImage.setPixel(i, j, black.rgba());
		}

	item.thumbnail = tmpImage;
	QList<LayerItem>::iterator it;

	//Get the index of first selected layer.
	int firstSelectedIndex = 0;
	for (it = layerlist.begin(); it != layerlist.end(); it++)
	{
		if (it->isSelected == true)
		{
			break;
		}
		firstSelectedIndex++;
	}
	
	//Unselect all layers.
	for (it = layerlist.begin(); it != layerlist.end(); it++)
	{
		it->isSelected = false;
	}

	//Select new layer.
	item.isSelected = true;
	layerlist.insert(firstSelectedIndex, item);
	emit(updateModel());
}

void LayerManager::deleteLayer()
{
	if (layerlist.size() == 1)
	{
		return;
	}
	int index = 0;
	QList<LayerItem>::iterator it;
	for (it = layerlist.begin(); it != layerlist.end(); it++)
	{
		index++;
		if (it->isSelected)
		{
			break;
		}
	}

	if (index == layerlist.size())
	{
		layerlist[index - 2].isSelected = true;
	}else
	{
		layerlist[index].isSelected = true;
	}

	layerlist.erase(it);

	emit(updateModel());
	emit(updateCanvas());
}

void LayerManager::changeLayerVisibility(int index)
{
	layerlist[index].isShow = !layerlist[index].isShow;
	emit(updateModel());
	emit(updateCanvas());
}


void LayerManager::clearLayers()
{
	layerlist.clear();
}

void LayerManager::setLayerSize(const QSize &size)
{
	layerSize = size;
}

QImage* LayerManager::getSelectedLayer()
{
	for (QList<LayerItem>::iterator it = layerlist.begin(); it != layerlist.end(); it++)
	{
		if (it->isSelected)
		{
			return &(it->thumbnail);
		}
	}
}

LayerItem*LayerManager::getSelectItem()
{
	for (QList<LayerItem>::iterator it = layerlist.begin(); it != layerlist.end(); it++)
	{
		if (it->isSelected)
		{
			return &(*it);
		}
	}
}


void LayerManager::selectLayer(int index)
{
	int i = 0;
	for (QList<LayerItem>::iterator it = layerlist.begin(); it != layerlist.end(); it++)
	{
		if (i == index)
		{
			it->isSelected = true;
			i++;
			
		}else
		{
			it->isSelected = false;
			i++;
		}
	}

}

void LayerManager::duplicateLayer()
{
	LayerItem* selectedItem = getSelectItem();
	LayerItem item;
	item.isShow = selectedItem->isShow;
	item.layerName = selectedItem->layerName + " copy";
	item.transparence = selectedItem->transparence;
	item.anchor = selectedItem->anchor;
	QImage originImage = selectedItem->thumbnail;
	item.thumbnail = originImage.copy(0, 0, originImage.width(), originImage.height());

/*
	item.layerName = QString("Layer ") + QString::number(layerCounter++);
	item.transparence = 100;
	item.anchor = QPoint(0, 0);
	QImage tmpImage(layerSize, QImage::Format_ARGB32);
	QColor black(0, 0, 0, 0);
	black.setAlpha(0);

	QImage *selectedImage = this->getSelectedLayer();
	item.thumbnail = selectedImage->copy(0,0,selectedImage->width(), selectedImage->height());*/
	
	QList<LayerItem>::iterator it;

	//Get the index of first selected layer.
	int firstSelectedIndex = 0;
	for (it = layerlist.begin(); it != layerlist.end(); it++)
	{
		if (it->isSelected == true)
		{
			break;
		}
		firstSelectedIndex++;
	}

	//Unselect all layers.
	for (it = layerlist.begin(); it != layerlist.end(); it++)
	{
		it->isSelected = false;
	}

	//Select new layer.
	item.isSelected = true;

	layerlist.insert(firstSelectedIndex, item);

	emit(updateModel());
}

void LayerManager::updateLayerPanel()
{
	emit(updateModel());
}

int LayerManager::getSelectedLayerIndex()
{
	int index = 0;
	QList<LayerItem>::iterator it;
	for (it = layerlist.begin(); it != layerlist.end(); it++)
	{
		if (it->isSelected)
		{
			break;
		}
		index++;
	}
	return index;
}

void  LayerManager::crop(QRect cropRect)
{
	QList<LayerItem>::iterator it;
	this->layerSize = QSize(cropRect.width(), cropRect.height());
	for (it = layerlist.begin(); it != layerlist.end(); it++)
	{
		QImage cropped = it->thumbnail.copy(cropRect);
		it->thumbnail = cropped;
		it->anchor = QPoint(0, 0);
	}

	updateCanvas();
	updateModel();
}

QSize LayerManager::getLayerSize() const
{
	return layerSize;
}
