

QT       += core gui multimedia sql widgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport network


INCLUDEPATH += /usr/include/timetagger
DEFINES += LINUX linux
TARGET = qkd_gui
TEMPLATE = app
LIBS += -DLINUX -Dlinux -L/usr/lib64/timetagger/x64/driver -lTimeTagger -lokFrontPanel
PKGCONFIG += 
CONFIG += link_pkgconfig 


INCPATH +=




SOURCES += main.cpp\
           dbcontrol.cpp \
           mainwindow.cpp \
           qcustomplot.cpp \
           qkd_param.cpp \
           swabian.cpp \
           CustomStartStop.cpp \
           ps.cpp \
           logging.cpp
          

HEADERS  += mainwindow.h \
            dbcontrol.h \
            qcustomplot.h \
            qkd_param.h \
            swabian.h \
            CustomStartStop.h \
            ps.h \
            logging.h
	   

FORMS    += mainwindow.ui \
    qkd_param.ui


RESOURCES += \
    resources.qrc



unix:!macx: LIBS += -L$$PWD/../lib/ -lhdf5

unix:!macx: LIBS += -L$$PWD/../lib/ -lhdf5_cpp
