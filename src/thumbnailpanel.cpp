#include "thumbnailpanel.h"

ThumbnailPanel::ThumbnailPanel(QWidget * parent) : QDockWidget(parent)
{
	this->setMinimumSize(200, 200);
    setStyleSheet("background-color: #393939;");
	this->setFeatures(QDockWidget::DockWidgetFloatable |\
		QDockWidget::DockWidgetMovable|\
		QDockWidget::DockWidgetClosable);

	thumbnailLabel = new QLabel();
	QPixmap pixmap("images\\sample.jpg");
	QPixmap scaled = pixmap.scaledToWidth(200);
	thumbnailLabel->setPixmap(scaled);
	thumbnailLabel->setAlignment(Qt::AlignCenter);

	thumbnailCanvas = new ThumbnailCanvas(true);

	scaleFactorLabel = new QLabel();
	scaleFactorLabel->setFixedWidth(40);
	scaleFactorLabel->setText("100%");
	zoomInBtn = new QPushButton(tr("in"), this);
	zoomOutBtn = new QPushButton(tr("out"), this);
	zoomInBtn->setFixedWidth(30);
	zoomOutBtn-> setFixedWidth(30);

#if defined(Q_OS_MAC)
	scaleFactorLabel->setStyleSheet("color: #ffffff;");
	zoomInBtn->setStyleSheet("color: #ffffff;");
	zoomOutBtn->setStyleSheet("color: #ffffff;");
#endif

	connect(zoomInBtn, SIGNAL(clicked()), this, SLOT(thumbZoomIn()));
	connect(zoomOutBtn, SIGNAL(clicked()), this, SLOT(thumbZoomOut()));
	connect(this, SIGNAL(thumbZoom(float)), this, SLOT(updateZoomLabel(float)));

	zoomSlider = new QSlider(Qt::Horizontal);
	zoomSlider->setValue(50);
	zoomSlider->setMaximum(100);
	connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(zoomValueChanged(int)));
	QHBoxLayout *zoomLayout = new QHBoxLayout();
	zoomLayout->addWidget(scaleFactorLabel);
	zoomLayout->addWidget(zoomInBtn);
	zoomLayout->addWidget(zoomSlider);
	zoomLayout->addWidget(zoomOutBtn);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(thumbnailCanvas);
	
	mainLayout->addLayout(zoomLayout);
	
	QWidget *mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);
	this->setWidget(mainWidget);
}


ThumbnailPanel::~ThumbnailPanel()
{

}

void ThumbnailPanel::thumbZoomIn()
{
	zoomSlider->setValue(zoomSlider->value() * 0.9);
	zoomValueChanged(zoomSlider->value());
}

void ThumbnailPanel::thumbZoomOut()
{
	int tmp = zoomSlider->value();
	qDebug() << tmp<<tmp*1.1;
	zoomSlider->setValue(zoomSlider->value() * 1.1);
	if (zoomSlider->value() - tmp == 0 )
	{
		zoomSlider->setValue(zoomSlider->value() + 1);
	}
	zoomValueChanged(zoomSlider->value());
}

void ThumbnailPanel::zoomValueChanged(int value)
{
	if (value >50)
	{
		if (value == 100)
		{
			emit thumbZoom(32.0);
			updateZoomLabel(32.0);
		}
		emit thumbZoom(1.0 + (value - 50) * PROPOTION4);
		updateZoomLabel(1.0 + (value - 50) * PROPOTION4);
	}
	else
	{
		emit thumbZoom(1.0 - (50 - value) * PROPOTION3);
		updateZoomLabel(1.0 - (50 - value) * PROPOTION3);
	}
}


void ThumbnailPanel::updateZoomLabel(float scaleFactor)
{
	scaleFactorLabel->setText(QString::number(scaleFactor * 100) + "%");
}

void ThumbnailPanel::updateZoomSlider(float scaleFactor)
{
	if (scaleFactor - 1.0 > 0.00001)
	{
		//value bigger than 100%
		zoomSlider->setValue(50 + (scaleFactor - 1.0) * PROPOTION2);
	}
	else
	{
		//value less than 100%
		zoomSlider->setValue(50 - (1.0 - scaleFactor) * PROPOTION1);
	}
}

ThumbnailCanvas * ThumbnailPanel::getThumbnailCanvas()
{
	return thumbnailCanvas;
}

void ThumbnailPanel::setThumb(QPixmap &p)
{
	thumbnailCanvas->setPixmap(p);
	thumbnailCanvas->update();
}
