#ifndef SWATCHES_H
#define SWATCHES_H

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QColorDialog>

class Swatches : public QWidget
{
	Q_OBJECT

public:
	Swatches(QWidget *parent = 0);
	~Swatches();
	QColor Color() const;

public slots:
	void changeColor(QColor c);

protected:
	void paintEvent(QPaintEvent* event);
	void mouseReleaseEvent(QMouseEvent *event);

private:
	QColor currentColor;

signals:
	void colorSelected(QColor c);
};

#endif // SWATCHES_H
