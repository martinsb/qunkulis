######################################################################
# Automatically generated by qmake (3.0) Thu Jun 27 17:10:30 2013
######################################################################

CONFIG += qtestlib
TEMPLATE = app

#include(../src/unqlite-qt.pri)

HEADERS += tst_qunkulis.h

SOURCES += tst_qunkulis.cpp

INCLUDEPATH += ../src
#unix:LIBS += -L../src -lunqlite-qt
unix:LIBS += ../src/libunqlite-qt.a
win32:LIBS += ../src/libunqlite-qt.lib
