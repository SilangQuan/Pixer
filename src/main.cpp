#include "mainwindow.h"
#include <QApplication>
#include <QIcon>
#include "thumbnailcanvas.h"
#include "layermanager.h"

LayerManager * LayerManager::m_pInstance = NULL;
int LayerManager::layerCounter = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    return a.exec();
}
