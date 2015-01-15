#include "palettesquare.h"

PaletteSquare::PaletteSquare(QWidget  *parent):QWidget(parent)
{
	currentColor = QColor(255, 255, 255);
	this->setFixedSize(32, 32);
}

PaletteSquare::PaletteSquare(QColor c,  int r, int col, QWidget  *parent)
{
	currentColor = c;
	row = r;
	column = col;
	this->setFixedSize(32, 32);
}

PaletteSquare::~PaletteSquare()
{

}

void PaletteSquare::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QPen p;
	p.setStyle(Qt::NoPen);
	painter.setPen(p);
	painter.setBrush(currentColor);
	painter.drawRect(rect());

}

void PaletteSquare::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit changeForecolorSignal(currentColor);
	}
	if (event->button() == Qt::RightButton)
	{
		QColor c = QColorDialog::getColor();
		if (c.isValid())
		{
			currentColor = c;
			this->update();
		}
	}
}


void PaletteSquare::setName(const QString &s)
{
	this->name = s;
}

void PaletteSquare::setColor(const QColor &c)
{
	this->currentColor = c;
}

int PaletteSquare::getRow()
{
	return row;
}

int PaletteSquare::getColumn()
{
	return column;
}

QColor PaletteSquare::getColor()
{
	return currentColor;
}

QString PaletteSquare::getName()
{
	return name;
}
/*
QSize PaletteSquare::sizeHint()
{
	return QSize(100, 300);
}
*/
