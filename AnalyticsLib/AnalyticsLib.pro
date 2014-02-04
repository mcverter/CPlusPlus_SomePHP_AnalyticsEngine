#-------------------------------------------------
#
# Project created by QtCreator 2013-07-17T12:42:49
#
#-------------------------------------------------

QT       +=  core xml declarative


TARGET = AnalyticsLib
CONFIG   += console

TEMPLATE = app


SOURCES += main.cpp \
    KbpfLogger.cpp \
    KbpfAnalyzer.cpp \
    AnalyticsGrapher.cpp \
    AnalyticsGui.cpp \
    AnalyticsUtils.cpp \
    ExperimentalKbpfAnalyzer.cpp

HEADERS += \
    KbpfLogger.h \
    KbpfAnalyzer.h \
    AnalyticsGrapher.h \
    AnalyticsGui.h \
    AnalyticsUtils.h \
    ExperimentalKbpfAnalyzer.h

OTHER_FILES += \
    qml/graph.qml \
    qml/graphComponents/Point.qml \
    qml/graphComponents/GraphPoints.qml \
    qml/graphComponents/GraphNode.qml \
    qml/graphComponents/GraphNewNode.qml \
    qml/graphComponents/GraphLine.qml \
    qml/graphComponents/Graph.qml
