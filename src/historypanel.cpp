#include "historypanel.h"

HistoryPanel::HistoryPanel(QUndoStack *stack,QWidget *parent)
: QDockWidget(parent), undoView(stack)
{
	this->setFeatures(QDockWidget::DockWidgetFloatable | \
		QDockWidget::DockWidgetMovable | \
		QDockWidget::DockWidgetClosable);
	this->setAttribute(Qt::WA_QuitOnClose, false);

	undoView.setBackgroundRole(QPalette::Dark);
	undoView.setFrameShape(QFrame::NoFrame);
	this->setWidget(&undoView);

	connect(&undoView, SIGNAL(pressed(const QModelIndex &)), this, SLOT(itemClickedSlot(const QModelIndex &)));

}

HistoryPanel::~HistoryPanel()
{

}

void HistoryPanel::itemClickedSlot(const QModelIndex &index)
{
	qDebug() << "itemClickedSlot";
	emit historyTriggered();
}
