

QT       += core gui multimedia sql widgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport network


INCLUDEPATH += /usr/include/timetagger /usr/local/include
DEFINES += LINUX linux
TARGET = qkd_gui
TEMPLATE = app
LIBS += -DLINUX -Dlinux -L/usr/lib64/timetagger/x64/driver -lTimeTagger -lokFrontPanel -L/usr/local/lib -lgsl -lgslcblas
PKGCONFIG += 
CONFIG += link_pkgconfig 


INCPATH +=




SOURCES += main.cpp\
           dbcontrol.cpp \
           mainwindow.cpp \
           qcustomplot.cpp \
           qkd_param.cpp \
           save_dialog.cpp \
           swabian.cpp \
           CustomStartStop.cpp \
           ps.cpp \
           logging.cpp \
           fit.cpp
          

HEADERS  += mainwindow.h \
            dbcontrol.h \
            qcustomplot.h \
            qkd_param.h \
            save_dialog.h \
            swabian.h \
            CustomStartStop.h \
            ps.h \
            logging.h \
            fit.h
	   

FORMS    += mainwindow.ui \
    qkd_param.ui \
    save_dialog.ui


RESOURCES += \
    resources.qrc



unix:!macx: LIBS += -L$$PWD/../lib/ -lhdf5

unix:!macx: LIBS += -L$$PWD/../lib/ -lhdf5_cpp
