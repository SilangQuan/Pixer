#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QImage>
#include <QPoint>
#include <QDir>

enum COLORMODE{RGB,MONO};

enum OPERATIONTYPE{ NORMAL, PEN, CROP, ERASER, STRAW, ZOOMIN, \
	ZOOMOUT, SELECTION1, BUCKET, MOVE, DEEPEN, DODGE};

struct LayerItem
{
	QString layerName;
	QImage thumbnail;
	QPoint anchor;
	float transparence;
	bool isShow;
	bool isSelected;
	unsigned int layerNo;
	
};


#endif
