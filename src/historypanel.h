#ifndef HISTORYPANEL_H
#define HISTORYPANEL_H

#include <QDockWidget>
#include <QUndoView>
#include <QDebug>
class HistoryPanel : public QDockWidget
{
	Q_OBJECT

public:
	HistoryPanel(QUndoStack* unduStack,  QWidget *parent = 0);
	~HistoryPanel();

private:
	QUndoView undoView;

private slots:
void itemClickedSlot(const QModelIndex &index);

signals:
	void historyTriggered();
};

#endif // HISTORYPANEL_H
