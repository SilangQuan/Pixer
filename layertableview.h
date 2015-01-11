#ifndef LAYERLISTVIEW_H
#define LAYERLISTVIEW_H

#include <QTableView>
#include <QMouseEvent>
#include <QDebug>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QContextMenuEvent>
#include <QMenu>
#include "layertablemodel.h"
#include "layeritemdelegate.h"
#include "layermanager.h"

class LayerTableView : public QTableView
{
	Q_OBJECT

public:
	LayerTableView(QWidget *parent = 0);
	~LayerTableView();
	void setLayerSize(QSize s);

public slots:
	void deleteLayer();
	void updateView();

protected:
	//void mouseMoveEvent(QMouseEvent * event);
	//void mousePressEvent(QMouseEvent *event);
	void contextMenuEvent(QContextMenuEvent * event);

private:
	void createActions();
	LayerItemDelegate *delegate;
	LayerTableModel *model;
	QSize layerSize;

	QMenu *contextMenu;
	QAction *deleteAct;
	QAction *duplicateAct;

private slots:
	void itemClicked(const QModelIndex&);
	void selectionChangedSlot(const QItemSelection&, const QItemSelection&);
};

#endif // LAYERLISTVIEW_H
