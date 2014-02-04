
#include <QtCore>
#include <QApplication>
#include "KbpfAnalyzer.h"
#include "AnalyticsGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    KbpfAnalyzer analyzer;

    qDebug() << "hello world";

    AnalyticsGUI * ag = new AnalyticsGUI();
    ag->show();


    return app.exec();
}
