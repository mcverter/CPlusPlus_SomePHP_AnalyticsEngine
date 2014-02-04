#-------------------------------------------------
#
# Project created by QtCreator 2013-07-05T15:09:55
#
#-------------------------------------------------

QT       += core gui xml

TARGET = FootFootTheTracker
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../.represents/curiokulbooks.cpp \
    KinectMonitor.cpp

HEADERS  += mainwindow.h \
    QtKinectWrapper/QtKinectWrapper.h \
    ../../.represents/curiokulbooks.h \
    KinectMonitor.h

QTKINECTWRAPPER_SOURCE_PATH = /home/dsion/curio_mitchell/FootFootTheTracker/QtKinectWrapper
OPENNI_LIBRARY_PATH = /usr/include/ni

INCLUDEPATH += $$QTKINECTWRAPPER_SOURCE_PATH
# Sources for Qt wrapper
SOURCES += $$QTKINECTWRAPPER_SOURCE_PATH/QKinectWrapper.cpp
HEADERS += $$QTKINECTWRAPPER_SOURCE_PATH/QKinectWrapper.h


INCLUDEPATH += $$QTKINECTWRAPPER_SOURCE_PATH/OpenNI/Include

   # Iterate through
   for(l, OPENNI_LIBRARY_PATH):LIBS+=-L$$l/lib
 LIBS += -L$$OPENNI_LIBRARY_PATH/lib

INCLUDEPATH += /usr/include/ni

LIBS += -L/usr/lib -lOpenNI

OTHER_FILES += \
    ../../.represents/.represents.kbpf
