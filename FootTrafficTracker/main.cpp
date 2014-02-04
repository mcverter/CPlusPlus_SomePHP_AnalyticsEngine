#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QDebug>
#include "KinectMonitor.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.setWindowState(Qt::WindowMinimized);

    QObject::connect(&a, SIGNAL(aboutToQuit()), &w,  SLOT(handleApplicationCloseSlot()));
    return a.exec();
}
