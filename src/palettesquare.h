#ifndef PALETTESQUARE_H
#define PALETTESQUARE_H

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QColorDialog>

class PaletteSquare : public QWidget
{
	Q_OBJECT

public:
	PaletteSquare(QWidget  *parent = 0);
	PaletteSquare(QColor c, int row, int column, QWidget  *parent = 0);
	~PaletteSquare();

    void setName(const QString &s);
    void setColor(const QColor &c);
	int getRow();
	int getColumn();
	QColor getColor();
	QString getName();

protected:
	void paintEvent(QPaintEvent* event);
	void mouseReleaseEvent(QMouseEvent *event);
	//QSize sizeHint();

private:
	QColor currentColor;
	QString name;
	int row;
	int column;
	
signals:
	void changeForecolorSignal(QColor c);
};

#endif // PALETTESQUARE_H
