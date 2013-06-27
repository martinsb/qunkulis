QMAKE_CFLAGS += -fpermissive
QMAKE_CXXFLAGS += -fpermissive

# Input
HEADERS += \
	unqlite-db/unqlite.h \
    errorcode.h \
	database.h \
	converter.h \ 
    virtualmachine.h \
    executionresult.h

SOURCES += \ 
	unqlite-db/unqlite.c \
	database.cpp \
	converter.cpp \
    virtualmachine.cpp \
    executionresult.cpp

RESOURCES += unqlite-qt.qrc
