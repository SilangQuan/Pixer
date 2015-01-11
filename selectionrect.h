#ifndef SELECTIONRECT_H
#define SELECTIONRECT_H
#include <QPoint>
#include <QPainter>

class SelectionRect
{
public:
	SelectionRect(){}
	~SelectionRect(){}

	void setStart(QPoint s) 
	{ 
		start = s; 
	} 

	void setEnd(QPoint e) 
	{ 
		end = e; 
	} 

	QPoint startPoint() 
	{ 
		return start; 
	} 

	QPoint endPoint() 
	{ 
		return end; 
	} 
	void paint(QPainter &painter);

	void reset()
	{
		QPoint P(0,0);
		start = P;
		end = P;
	}
	
	int height()
	{
		return qAbs(startPoint().y() - endPoint().y());
	}

	int width()
	{
		return qAbs(startPoint().x() - endPoint().x());
	}

	int perimeter()
	{
		return 2 * height() + 2 * width() - 2;
	}

	bool contains(QPoint p)
	{
		QRect rec(startPoint(), endPoint());
		return rec.contains(p);
	}
private:
	QPoint start;
	QPoint end;
};

#endif // SELECTIONRECT_H
