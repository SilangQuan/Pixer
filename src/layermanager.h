#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include <QList>
#include <QObject>
#include "common.h"
#include <QColor>
#include <QListIterator>
#include <QDebug>
#include <QPainter>
class LayerTableModel;
class Canvas;

class LayerManager :public QObject
{
	Q_OBJECT
public:
	~LayerManager();
	static  LayerManager *getInstance();
	QList<LayerItem>* getpLayerList();
	void clearLayers();
    void setLayerSize(const QSize &size);
	QImage* getSelectedLayer();
	int getSelectedLayerIndex();
	QSize getLayerSize() const;
	void crop(QRect cropRect);
	LayerItem* getSelectItem();
	
public slots:
	void addNewLayer();
	void deleteLayer();
	void duplicateLayer();
	void changeLayerVisibility(int index);
	void selectLayer(int index);
	void updateLayerPanel();

private:
	LayerManager(QObject *parent = 0);
	static LayerManager *m_pInstance;
	QList<LayerItem> layerlist;
	QSize layerSize;
	static int layerCounter;

signals:
	void updateCanvas();
	void updateModel();
};

#endif
