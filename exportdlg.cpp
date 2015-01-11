#include "exportdlg.h"

ExportDlg::ExportDlg(QWidget *parent)
	: QDialog()
{
	this->setWindowTitle("Export image");
	LayerManager *layerManager = LayerManager::getInstance();
	imageSize = layerManager->getLayerSize();
	originalSizeLabel = new QLabel(QString("Original size: " + QString::number(imageSize.width()) + "px  *  " + QString::number(imageSize.height()) + "px"));
	exportSizeLabel = new QLabel(QString("Export Size:"));
	widthLabel = new QLabel(QString("Width:"));
	widthLabel->setFixedWidth(50);
	heightLabel = new QLabel(QString("Height: "));
	heightLabel->setFixedWidth(50);

	widthEdit = new QLineEdit();
	widthEdit->setText(QString::number(imageSize.width()));
	connect(widthEdit, SIGNAL(textChanged(const QString &)), this, SLOT(widthEditChangedSlot(const QString &)));
	heightEdit = new QLineEdit();
	heightEdit->setText(QString::number(imageSize.height()));
	connect(heightEdit, SIGNAL(textChanged(const QString &)), this, SLOT(heightEditChangedSlot(const QString &)));
	widthEdit->setValidator(new QIntValidator(1, 1920, this));
	heightEdit->setValidator(new QIntValidator(1, 1080, this));


	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	canvas = new ThumbnailCanvas(false);
	canvas->setFixedSize(200, 200);

	//Merge all layer to one image.
	QImage image(layerManager->getLayerSize().width(), layerManager->getLayerSize().height(), QImage::Format_ARGB32);
	QColor transparent(0, 0, 0, 0);
	image.fill(transparent);
	QList<LayerItem>* layerList = layerManager->getpLayerList();

	QPainter mergePainter(&image);
	mergePainter.setCompositionMode(QPainter::CompositionMode_DestinationOver);

	for (QList<LayerItem>::iterator it = layerList->begin(); it != layerList->end(); it++)
	{
		if (it->isShow)
		{
			mergePainter.drawImage(it->anchor.rx(), it->anchor.ry(), it->thumbnail);
		}
	}
	previewImage = QPixmap::fromImage(image);
	canvas->setPixmap(previewImage);

	QHBoxLayout *widthLayout = new QHBoxLayout();
	QHBoxLayout *heightLayout = new QHBoxLayout();
	widthLayout->addWidget(widthLabel);
	widthLayout->addWidget(widthEdit);
	widthLayout->addWidget(new QLabel("px"));

	heightLayout->addWidget(heightLabel);
	heightLayout->addWidget(heightEdit);
	heightLayout->addWidget(new QLabel("px"));


	QVBoxLayout *rightLayout = new QVBoxLayout();
	rightLayout->addWidget(originalSizeLabel);
	rightLayout->addWidget(exportSizeLabel);
	rightLayout->addLayout(widthLayout);
	rightLayout->addLayout(heightLayout);

	QHBoxLayout *leftLayout = new QHBoxLayout();
	leftLayout->addWidget(canvas);
	leftLayout->addLayout(rightLayout);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(leftLayout);
	mainLayout->addWidget(buttonBox);

	this->setLayout(mainLayout);

}

ExportDlg::~ExportDlg()
{

}

void ExportDlg::widthEditChangedSlot(const QString & text)
{
	if (heightEdit->hasFocus())
	{
		return;
	}
	if (text.toInt() == 0 )
	{
		return;
	}
	qDebug()<<(text);
	int w = text.toInt();
	int h = imageSize.height() * w / imageSize.width();
	heightEdit->setText(QString::number(h));
	setPreviewSize(w, h);
}

void ExportDlg::heightEditChangedSlot(const QString & text)
{
	if (widthEdit->hasFocus())
	{
		return;
	}
	if (text.toInt() == 0)
	{
		return;
	}
	int h = text.toInt();
	int w = imageSize.width() * h / imageSize.height();
	widthEdit->setText(QString::number(w));
	setPreviewSize(w, h);
}

void ExportDlg::setPreviewSize(int w, int h)
{
	canvas->setPixmap(previewImage.scaledToHeight(h));
	this->update();
}

void ExportDlg::accept()
{
	int  w = widthEdit->text().toInt();
	int  h = heightEdit->text().toInt();

	QString filepath = QFileDialog::getSaveFileName(this,
		tr("Save As"), "",
		tr("PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp)"));
	if (filepath.isEmpty())
	{
		return;
	}
	else
	{
		emit exportImage(filepath, w, h);
		this->close();
	}
}
