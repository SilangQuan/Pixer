#include "swatches.h"

Swatches::Swatches(QWidget *parent)
	: QWidget(parent)
{
	currentColor = QColor(255, 255, 255);
}

Swatches::~Swatches()
{

}

void Swatches::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine, Qt::FlatCap));
	painter.setBrush(currentColor);
	painter.drawRect(rect());
}

void Swatches::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		currentColor = QColorDialog::getColor();
		this->update();
		emit(colorSelected(currentColor));
	}
}

QColor Swatches::Color() const
{
	return currentColor;
}

void Swatches::changeColor(QColor c)
{
	currentColor = c;
	emit(colorSelected(currentColor));
	this->update();
}