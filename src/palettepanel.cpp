#include "palettepanel.h"
#include "QDir"

PalettePanel::PalettePanel(QWidget * parent) : QDockWidget(parent)
{
	//this->setMinimumSize(200, 150);

    setStyleSheet("background-color: #393939;");
	this->setFeatures(QDockWidget::DockWidgetFloatable | \
		QDockWidget::DockWidgetMovable | \
		QDockWidget::DockWidgetClosable);

	squareList = new QList<PaletteSquare*> ();
	
	scrollArea = new QScrollArea();
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setWidgetResizable(true);
	scrollArea->setAlignment(Qt::AlignHCenter);

	mainLayout = new QGridLayout();
	mainLayout->setVerticalSpacing(15);


	mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);

	scrollArea->setWidget(mainWidget);
	this->setWidget(scrollArea);

	contextMenu = new QMenu(this);
	this->createActions();
	//deletePaletteSquare(0,  0);
}

PalettePanel::~PalettePanel()
{

}

void PalettePanel::createActions()
{
	addAct = new QAction(tr("Add New Swatches"), contextMenu);
	connect(addAct, SIGNAL(triggered()), this, SLOT(addPalettesquare()));
	contextMenu->addAction(addAct);
}

void PalettePanel::addPalettesquare()
{
	QColor c = QColorDialog::getColor();
	if (c.isValid())
	{
		int row = squareList->size() / 5;
		int column = squareList->size() - 5 * row;
		PaletteSquare *s = new PaletteSquare(c, row, column);
		s->setName(QString("Untitled"));
		connect(s, SIGNAL(changeForecolorSignal(QColor)), this, SLOT(paletteSquareClicked(QColor)));

		squareList->append(s);
		mainLayout->addWidget(s, s->getRow(), s->getColumn(), Qt::AlignTop);
	}

}

void PalettePanel::paletteSquareClicked(QColor c)
{
	emit changeForecolorSignal(c);
}

void PalettePanel::loadPalette(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "Palette load error!", file.errorString());
	}

	QTextStream in(&file);
	int row = 0;
	bool isStart = false;
	int count = 0;
	//Init palette.
	while (!in.atEnd()) {
		QString line = in.readLine();
		//Break if reach the end of doc.
		if (line.size() == 0)
		{
			break;
		}
		QStringList fields = line.split(" ");

		if (isStart)
		{
			row = count / 5;
			//mainLayout->count();
			if (count < squareList->size())
			{
				(*squareList)[count]->setName(fields[3]);
				(*squareList)[count]->setColor(QColor(fields[0].toInt(), fields[1].toInt(), fields[2].toInt()));
			
			}
			else{
				PaletteSquare *s = new PaletteSquare(QColor(fields[0].toInt(), fields[1].toInt(), fields[2].toInt()), row, count - 5 * row);
				s->setName(fields[3]);
				connect(s, SIGNAL(changeForecolorSignal(QColor)), this, SLOT(paletteSquareClicked(QColor)));

				squareList->append(s);
				mainLayout->addWidget(s, s->getRow(), s->getColumn(), Qt::AlignTop);
			}
			count++;
		}

		//Start parser color when come up with "#".
		if (fields[0] == "#")
		{
			isStart = true;
		}

	}

	int squareListSize = squareList->size();
	//When loaded palette count less than current palette, delete them.
	for (int i = count; i < squareListSize; i++)
	{
		this->deletePaletteSquare(squareList->at(i)->getRow(), squareList->at(i)->getColumn());
		//squareList->removeLast();
	}

	for (int i = count; i < squareListSize; i++)
	{
		//this->deletePaletteSquare(squareList->at(count)->getRow(), squareList->at(count)->getColumn());
		squareList->removeLast();
	}

	file.close();

	this->update();
}

void PalettePanel::savePalette(QString &path)
{
	QString name = path.mid(path.lastIndexOf("/") + 1, path.lastIndexOf(".") - path.lastIndexOf("/") - 1);
	QFile file(path);
	
	if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
	{
		QTextStream out(&file);
		out << "GIMP Palette" << endl;
		out << "Name: " << name << endl;
		out << "#" << endl;

		for (QList<PaletteSquare*>::iterator it = squareList->begin(); it < squareList->end(); it++)
		{
			out << (*it)->getColor().red() << " " << (*it)->getColor().green() << " " << (*it)->getColor().blue() <<" " << (*it)->getName()<< endl;;
		}
	}
	file.close();
}

void PalettePanel::deletePaletteSquare(int row, int column)
{
	// We avoid usage of QGridLayout::itemAtPosition() here to improve performance.
	for (int i = mainLayout->count() - 1; i >= 0; i--) {
		int r, c, rs, cs;
		mainLayout->getItemPosition(i, &r, &c, &rs, &cs);
		if ((r <= row && r + rs - 1 >= row) && (c <= column && c + cs - 1 >= column)) {
			// This layout item is subject to deletion.
			QLayoutItem *item = mainLayout->takeAt(i);
			delete item->widget();
		}
	}
	this->update();
}

void PalettePanel::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		contextMenu->popup(mapToGlobal(event->pos()));
	}
}
