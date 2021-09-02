

QT       += core gui multimedia sql widgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport network


INCLUDEPATH += $$PWD/../lib
DEPENDPATH += $$PWD/../lib
DEFINES += LINUX linux
TARGET = PROGRAM
TEMPLATE = app
LIBS += -DLINUX -Dlinux -L$$PWD/../lib/ -lftd3xx -ltdcbase -lTimeTagger -lokFrontPanel
PKGCONFIG += 
CONFIG += link_pkgconfig 


INCPATH +=




SOURCES += main.cpp\
           dbcontrol.cpp \
           mainwindow.cpp \
           qcustomplot.cpp \
           qkd_param.cpp \
           qutag_adq.cpp \
           qutag_anl.cpp \
           socket_com.cpp \
           swabian.cpp
          

HEADERS  += mainwindow.h \
            dbcontrol.h \
            qcustomplot.h \
            qkd_param.h \
            qutag_adq.h \
            qutag_anl.h \
            socket_com.h \
            tdcbase.h \
            tdcdecl.h \
            tdcstartstop.h \
            swabian.h
	   

FORMS    += mainwindow.ui \
    dialog.ui \
    qkd_param.ui


RESOURCES += \
    resources.qrc



unix:!macx: LIBS += -L$$PWD/../lib/ -lhdf5

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

unix:!macx: LIBS += -L$$PWD/../lib/ -lhdf5_cpp

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include
