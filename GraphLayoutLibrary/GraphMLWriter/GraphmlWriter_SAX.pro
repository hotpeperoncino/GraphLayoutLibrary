#-------------------------------------------------
#
# Project created by QtCreator 2013-08-28T12:52:40
#
#-------------------------------------------------

QT       += core xml

QT       -= gui

TARGET = GraphmlWriter_SAX
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    graphmlgenerator.cpp \
    testgraphmlwriter.cpp

HEADERS += \
    graphmlgenerator.h \
    testgraphmlwriter.h \
    DefaultValues.h \
    AttributeConstants.h \
    Attribute.h \
    GraphElementProperties.h \
    typedefs.h

INCLUDEPATH += D:/boost_1_53_0


