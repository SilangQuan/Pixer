#include "layertablemodel.h"

LayerTableModel::LayerTableModel( QObject *parent)
: QAbstractTableModel(parent)
{
	LayerManager *manager = LayerManager::getInstance();

	playerList = manager->getpLayerList();
	QImage image("images\\sample.jpg");
	selectedRow = 0; 
}

LayerTableModel::~LayerTableModel()
{

}

QVariant LayerTableModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	int column = index.column();
	if (column == 0)
	{
		if(role ==  Qt::CheckStateRole)
		{
			return playerList->at(index.row()).isShow ? Qt::Checked : Qt::Unchecked;
		}
		if (role == Qt::SizeHintRole)
		{
            return QSize(30, 50);
		}
	}
	else
	{
		if (role == Qt::EditRole)
		{
			return QVariant(playerList->at(index.row()).layerName);
		}
		if (role == Qt::DisplayRole)
		{
			return QVariant(playerList->at(index.row()).layerName);
		}

		if (role == Qt::DecorationRole)
		{
			if (playerList->at(index.row()).thumbnail.isNull())
			{
				return  playerList->at(index.row()).thumbnail;
			}else
			{
				return  playerList->at(index.row()).thumbnail.scaledToHeight(40);

			}
		}
		if (role == Qt::SizeHintRole)
		{
			return QSize(200, 50);
		}
		if (role == Qt::TextAlignmentRole) 
		{
			return int(Qt::AlignVCenter);
		}
	}

	return QVariant();
}

int LayerTableModel::rowCount(const QModelIndex &parent) const
{
	return (parent.isValid() && parent.column() != 0) ? 0 : playerList->size();
}

int LayerTableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 2;
}

QVariant LayerTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
		return QString::number(section);
	//if (role == Qt::DecorationRole)
		//return QVariant::fromValue(services);
	return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags LayerTableModel::flags(const QModelIndex &index) const
{

	if (!index.isValid())
		return 0;

	if (index.column() == 0)
		return Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsDropEnabled;

	return Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
}

bool LayerTableModel::setData(const QModelIndex &index, const
	QVariant &value, int role)
{
	if (!index.isValid())
	{
		return false;
	}

/*
	if (role == Qt::CheckStateRole && value.type() == QVariant::Bool)
	{
		(*playerList)[index.row()].isShow = value.toBool();
		emit(dataChanged(index, index));
		return true;
	}*/
	if (role == Qt::EditRole && index.column() == 1)
	{
		(*playerList)[index.row()].layerName = value.toString();
		emit(dataChanged(index, index));
		return true;
	}
	return false;
}

void LayerTableModel::changeLayerVisibility(const QModelIndex& index)
{
	if (index.isValid()&&index.column() == 0)
	{
		//setData(index, !(playerList->at(index.row()).isShow), Qt::CheckStateRole);
		LayerManager *pManager = LayerManager::getInstance();
		pManager->changeLayerVisibility(index.row());
	}
}

void LayerTableModel::deleteItem(int index)
{
	//QList<LayerItem>::iterator it = playerList->begin();
//	playerList->erase(it + index);
	//LayerManager *manager = LayerManager::getInstance();
	//manager->deleteLayer(index);
}

void LayerTableModel::addNewItem()
{
	LayerManager *manager = LayerManager::getInstance();
	manager->addNewLayer();
}

void LayerTableModel::refreshModel()
{
	beginResetModel();
	endResetModel();
	//emit updateCount(this->rowCount(QModelIndex()));
}

const QModelIndex& LayerTableModel::selecttedIndex(int row)
{
	return this->createIndex(row, 1);
}

void LayerTableModel::setSelecttedRow(int row)
{
	selectedRow = row;
}

int LayerTableModel::getSelecttedRow() const
{
	int index = 0;
	QList<LayerItem>::iterator it;
	for (it = playerList->begin(); it != playerList->end(); it++)
	{
		if (it->isSelected)
		{
			break;
		}
		index++;
	}
	return index;
}

Qt::DropActions LayerTableModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

QStringList LayerTableModel::mimeTypes() const
{
	QStringList types;
	types << "application/pixer-layer";
	return types;
}

QMimeData *LayerTableModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach(QModelIndex index, indexes) {
		if (index.isValid()) {
			QPixmap pixmap = qvariant_cast<QPixmap>(data(index, Qt::DecorationRole));
		//	QPoint location = data(index, Qt::UserRole + 1).toPoint();
			QString layername = data(index, Qt::DisplayRole).toString();
			stream << pixmap << layername;
		}
	}

	mimeData->setData("application/pixer-layer", encodedData);
	return mimeData;
}

bool LayerTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
	int row, int column, const QModelIndex &parent)
{

	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat("application/pixer-layer"))
		return false;

	if (column > 0)
		return false;

	int endRow;

	if (!parent.isValid()) {
		if (row < 0)
			endRow = playerList->size();
		else
			endRow = qMin(row, playerList->size());
	}
	else {
		endRow = parent.row();
	}

	playerList->swap(getSelecttedRow(), endRow);

	LayerManager *manager = LayerManager::getInstance();
	manager->updateLayerPanel();
	return true;
}
