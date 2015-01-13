#include "newfiledlg.h"

NewFileDlg::NewFileDlg(QWidget *parent)
	: QDialog(parent)
{
	presetBox = new QComboBox();
	presetBox->addItem("32 * 32");
	presetBox->addItem("64 * 64");
	presetBox->addItem("128 * 128");

	colormodeBox = new QComboBox();
	colormodeBox->addItem("RGBA");
	colormodeBox->addItem("Mono");

	widthEdit = new QLineEdit();
	widthEdit->setText("32");

	heightEdit = new QLineEdit();
	heightEdit->setText("32");
	widthEdit->setValidator(new QIntValidator(0, 1920, this));
	heightEdit->setValidator(new QIntValidator(0, 1080, this));

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(presetBox, SIGNAL(currentIndexChanged(int)), this, SLOT(presetBoxChanged(int)));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout *layoutLeft = new QVBoxLayout();
	layoutLeft->addWidget(new QLabel("Preset: "));
	layoutLeft->addWidget(new QLabel("Width: "));
	layoutLeft->addWidget(new QLabel("Height: "));
	layoutLeft->addWidget(new QLabel("Color Mode: "));

	QVBoxLayout *layoutRight = new QVBoxLayout();
	layoutRight->addWidget(presetBox);
	layoutRight->addWidget(widthEdit);
	layoutRight->addWidget(heightEdit);
	layoutRight->addWidget(colormodeBox);

	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainLayout->addLayout(layoutLeft);
	mainLayout->addLayout(layoutRight);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addLayout(mainLayout);
	layout->addWidget(buttonBox);
	this->setLayout(layout);

}

NewFileDlg::~NewFileDlg()
{

}

void NewFileDlg::accept()
{
	emit configConfirmed(widthEdit->text().toInt(), heightEdit->text().toInt(), RGB);
	this->close();
}

void NewFileDlg::presetBoxChanged(int index)
{
	switch (index)
	{
	case 0:
		widthEdit->setText("32");
		heightEdit->setText("32");
		break;
	case 1:
		widthEdit->setText("64");
		heightEdit->setText("64");
		break;
	case 2:
		widthEdit->setText("128");
		heightEdit->setText("128");
		break;
	default:
		break;
	}
}