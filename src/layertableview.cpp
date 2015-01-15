#include "layertableview.h"

LayerTableView::LayerTableView(QWidget *parent)
: QTableView(parent)
{
	delegate = new LayerItemDelegate();
	model = new LayerTableModel();
	
	this->setContentsMargins(0, 0, 0, 0);
	this->setModel(model);
	this->setItemDelegate(delegate);
	this->setEditTriggers(QAbstractItemView::DoubleClicked);

	this->horizontalHeader()->setStretchLastSection(true);
	this->horizontalHeader()->setHighlightSections(false);
	this->setFrameShape(QFrame::NoFrame);

#if defined(Q_OS_WIN)
this->setColumnWidth(0, 20);
#elif defined(Q_OS_MAC)
this->setColumnWidth(0, 30);
#endif

    this->setColumnWidth(1, 170);
	this->verticalHeader()->setVisible(false);
	this->horizontalHeader()->setVisible(false);

	this->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->setDefaultDropAction(Qt::MoveAction);
	this->setDragEnabled(true);
	this->setDragDropOverwriteMode(false);
	this->viewport()->setAcceptDrops(true);
	this->setDragDropMode(QAbstractItemView::InternalMove);
	this->setAcceptDrops(true);
	this->setDropIndicatorShown(true);


	this->setMouseTracking(true);//important

	connect(this->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), \
		this, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection&)));

	//When click on the checkbox it will emit signal twice.Click on the cell emit only once.
	connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(itemClicked(const QModelIndex&)));
	LayerManager* pManager = LayerManager::getInstance();
	connect(pManager, SIGNAL(updateModel()), this, SLOT(updateView()));

	contextMenu = new QMenu(this);
	this->createActions();
}

LayerTableView::~LayerTableView()
{

}


void LayerTableView::createActions()
{
	LayerManager* pManager = LayerManager::getInstance();

	deleteAct = new QAction(tr("Delete"), contextMenu);
	connect(deleteAct, SIGNAL(triggered()), pManager, SLOT(deleteLayer()));
	contextMenu->addAction(deleteAct);

	duplicateAct = new QAction(tr("Duplicate"), contextMenu);
	connect(duplicateAct, SIGNAL(triggered()), pManager, SLOT(duplicateLayer()));
	contextMenu->addAction(duplicateAct);
}


void LayerTableView::contextMenuEvent(QContextMenuEvent * event)
{
	const QModelIndex& index = this->indexAt(event->pos());
	if (index.isValid())
	{
		contextMenu->popup(mapToGlobal(event->pos()));
	}
}


void LayerTableView::updateView()
{
	//Select layer at init.
	model->refreshModel();
	this->resizeRowsToContents();

	LayerManager* pManager = LayerManager::getInstance();
	QList<LayerItem>* pList = pManager->getpLayerList();
	int index = 0;
	for (QList<LayerItem>::iterator it = pList->begin(); it != pList->end(); it++)
	{
		if (it->isSelected)
		{
			this->selectRow(index);
			break;
		}
		index++;
	}
	this->update();
}

void LayerTableView::itemClicked(const QModelIndex& index)
{
	if (index.isValid() )
	{
		//When click in column 0.
		if (index.column() == 0)
		{
			model->changeLayerVisibility(index);

		}
	}
}


void LayerTableView::deleteLayer()
{
	LayerManager* pManager = LayerManager::getInstance();
	pManager->deleteLayer();
}

void LayerTableView::setLayerSize(QSize s)
{
	layerSize = s;
}

void LayerTableView::selectionChangedSlot(const QItemSelection&, const QItemSelection&)
{
	QModelIndexList indexes = this->selectionModel()->selection().indexes();
	
	if (indexes.count() == 1)
	{
		LayerManager* pManager = LayerManager::getInstance();
		pManager->selectLayer(indexes.at(0).row());
	}

}
