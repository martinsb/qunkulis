QMAKE_CFLAGS += -fpermissive
QMAKE_CXXFLAGS += -fpermissive

# Input
HEADERS += \
	unqlite-db/unqlite.h \
    constants.h \
	database.h \
    virtualmachine.h

SOURCES += \ 
	unqlite-db/unqlite.c \
	database.cpp \
    virtualmachine.cpp

RESOURCES += unqlite-qt.qrc
