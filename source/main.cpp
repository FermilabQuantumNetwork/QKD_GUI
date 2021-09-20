#include <QApplication>
#include "mainwindow.h"
#include "logging.h"

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
    int i;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QApplication::setGraphicsSystem("raster");
#endif

    for (i = 0; i < argc; i++) {
        if (!strcmp(argv[i],"--loglevel") && (i + 1 < argc)) {
            if (!strcmp(argv[i+1],"debug"))
                verbosity = DEBUG;
            else if (!strcmp(argv[i+1],"verbose"))
                verbosity = VERBOSE;
            else if (!strcmp(argv[i+1],"notice"))
                verbosity = NOTICE;
            else if (!strcmp(argv[i+1],"warning"))
                verbosity = WARNING;
            else
                fprintf(stderr, "unknown loglevel '%s'\n", argv[i+1]);

            i += 1;
        }
    }

    QApplication a(argc, argv);

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
