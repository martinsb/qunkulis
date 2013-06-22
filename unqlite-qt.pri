QMAKE_CFLAGS += -fpermissive
QMAKE_CXXFLAGS += -fpermissive

# Input
HEADERS += \
	unqlite-db/unqlite.h \	
	database.h \
	converter.h 

SOURCES += \ 
	unqlite-db/unqlite.c \
	database.cpp \
	converter.cpp

RESOURCES += unqlite-qt.qrc