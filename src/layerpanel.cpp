#include "layerpanel.h"

LayerPanel::LayerPanel(QWidget * parent) : QDockWidget(parent)
{
	this->setMinimumSize(200, 200);
    this->setStyleSheet("background-color: #393939; padding: 0px;");
	this->setFeatures(QDockWidget::DockWidgetFloatable | \
		QDockWidget::DockWidgetMovable | \
		QDockWidget::DockWidgetClosable);
	this->setContentsMargins(0, 0, 0, 0);

	layerTable = new LayerTableView(this);

	addBtn = new QPushButton(tr("Add"), this);
	addBtn->setDisabled(true);
	deleteBtn = new QPushButton(tr("Delete"), this);
	deleteBtn->setDisabled(true);
	QHBoxLayout *layout = new QHBoxLayout();

	LayerManager* pManager = LayerManager::getInstance();
//	connect(pManager, SIGNAL(updateModel()), this, SLOT(updateView()));
	connect(addBtn, SIGNAL(clicked()), pManager, SLOT(addNewLayer()));
	connect(deleteBtn, SIGNAL(clicked()), layerTable, SLOT(deleteLayer()));

	layout->addWidget(addBtn);
	layout->addWidget(deleteBtn);
	
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(layerTable);
	mainLayout->addLayout(layout);

	QWidget *mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);
	this->setWidget(mainWidget);

}
LayerPanel::~LayerPanel()
{

}

void LayerPanel::setLayerSize(QSize s)
{
	layerTable->setLayerSize(s);
}

void LayerPanel::enableBtns()
{
	addBtn->setEnabled(true);
	deleteBtn->setEnabled(true);
}

