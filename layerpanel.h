#ifndef LAYERPANEL_H
#define LAYERPANEL_H

#include <QDockWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>

#include "layertableview.h"

class LayerPanel : public QDockWidget
{
	Q_OBJECT

public:
	LayerPanel(QWidget *parent = 0);
	~LayerPanel();
	void setLayerSize(QSize s);

public slots:
	void enableBtns();
private:
	LayerTableView *layerTable;
	QPushButton *addBtn;
	QPushButton *deleteBtn;

};

#endif // LAYERPANEL_H
