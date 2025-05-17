#include "mainwindow.h"
#include <QApplication>
#include "logger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Logger::init("cornifer.log");

    MainWindow w;
    w.show();

    return app.exec();
}
