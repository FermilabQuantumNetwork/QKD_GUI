#include <QApplication>
#include "mainwindow.h"

#include <signal.h>//ctl+c
#include <stdlib.h>
#include <stdio.h>//printf

#define UNUSED(x) (void)(x)

void my_handler(int s)
{
    UNUSED(s);
    printf("\n ADIOS :D \n");
    exit(1); 
}

int main(int argc, char **argv)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QApplication::setGraphicsSystem("raster");
#endif

    QApplication a(argc, argv);
    a.setStyleSheet("QComboBox { background-color: darkGray }" "QListView { color: white; }");

    ///////////ctl+c/////////////////////////////

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    ////////////////////////////////////////////

    MainWindow w;

    w.show();

    return a.exec();
}
