#include "layeritemdelegate.h"

LayerItemDelegate::LayerItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	QImage gridImage(200, 200, QImage::Format_RGB32);
	QRgb grey = QColor(204, 204, 204).rgb();
	QRgb white = QColor(255, 255, 255).rgb();
	for (int i = 0; i < 200; i++)
	for (int j = 0; j < 200; j++)
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

LayerItemDelegate::~LayerItemDelegate()
{

}



void LayerItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	if (index.column() == 1) // value column
	{
		if (option.state & QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());

		QImage image = qvariant_cast<QImage>(index.data(Qt::DecorationRole));
		//QImage image = index.model()->data(index, Qt::DecorationRole).toString();
		QRect rect = option.rect;
		int x = rect.x() + 10;
		int y = rect.y() + 5;

		QBrush brush;
		//Draw grid background
		brush.setTexture(m_gridPixmap);
		painter->fillRect(x, y, 40, 40, brush);

		//Draw image
		QImage scaledImage;
		if (image.width() > image.height())
		{
			scaledImage = image.scaledToWidth(40);
		}
		else{
			scaledImage = image.scaledToHeight(40);
		}
		
		painter->drawImage(x, y, scaledImage);
		QRect textRect(rect.x() + 60, rect.y(), rect.width() - 60, rect.height());

		QString layerName = index.model()->data(index, Qt::DisplayRole).toString();
		QTextOption textOption = Qt::AlignLeft | Qt::AlignVCenter;
		painter->drawText(textRect, layerName, textOption);
		//如果当前有焦点，就绘制一个焦点矩形，否则什么都不做    
		//drawFocus(painter, myOption, myOption.rect);
	}
	else
	{
		QStyledItemDelegate::paint(painter, option, index);
	}
}


bool LayerItemDelegate::editorEvent(QEvent * event,
	QAbstractItemModel * model,
	const QStyleOptionViewItem & option,
	const QModelIndex & index)
{
	
	/*if (event->type() == QEvent::MouseButtonPress &&
		index.column() == 0){
		QVariant var = model->data(index, Qt::CheckStateRole);
		bool isFavourite = var.toBool();
		if (var.isValid())
			isFavourite = isFavourite ? false : true;
		else
			isFavourite = true;
		model->setData(index, isFavourite, Qt::CheckStateRole);
		return true;//I have handled the event  
	}
*/

	return false;
}

QWidget *LayerItemDelegate::createEditor(QWidget *parent,
	const QStyleOptionViewItem &option,
	const QModelIndex &index) const
{
	if (index.column() == 1) // value column
	{
		QLineEdit* edit = new QLineEdit(parent);
		edit->setFixedWidth(200);
		edit->setFixedHeight(36);
		edit->setContentsMargins(56, 17, 50, 0);
		return edit;
	}
	else return 0;  // no editor attached
}

void LayerItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();

	QLineEdit *edit = static_cast<QLineEdit*>(editor);
	edit->setText(value);
}

void LayerItemDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	editor->setGeometry(option.rect);
}

void LayerItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const
{
	QLineEdit *edit = static_cast<QLineEdit*>(editor);
	model->setData(index, edit->text(), Qt::EditRole);
}
