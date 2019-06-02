#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("JoWin");
    QCoreApplication::setOrganizationDomain("jowin.at");
    QCoreApplication::setApplicationName("JoWin Hue");

    MainWindow w;
    w.show();

    return a.exec();
}
