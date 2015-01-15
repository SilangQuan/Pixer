#ifndef PALETTEPANEL_H
#define PALETTEPANEL_H

#include <QObject>
#include <QDockWidget>
#include <QList>
#include <QGridLayout>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QScrollArea>
#include <QMenu>
#include <QAction>

#include "palettesquare.h"
#include "common.h"

class PalettePanel : public QDockWidget
{
	Q_OBJECT

public:
	PalettePanel(QWidget * parent = 0);
	~PalettePanel();

private:
	void deletePaletteSquare(int row, int column);
	void createActions();

	QList<PaletteSquare*> *squareList;
	QGridLayout *mainLayout;
	QScrollArea* scrollArea;
	QWidget *mainWidget;

	QMenu *contextMenu;
	QAction *addAct;


signals:
	void changeForecolorSignal(QColor c);

public slots:
	void paletteSquareClicked(QColor c);
    void loadPalette(const QString &path);
	void savePalette(QString &path);

private slots:
	void addPalettesquare();

protected:
	void mouseReleaseEvent(QMouseEvent *event);

};

#endif // PALETTEPANEL_H
