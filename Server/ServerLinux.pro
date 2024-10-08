TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        tcpsocket.cpp \
        lib/sqlite-amalgamation-3430000/sqlite3.c

HEADERS += \
    clientrequesthandler.h \
    connection.h \
    database.h \
    tcpsocket.h \
    terminal.h \
    lib/sqlite-amalgamation-3430000/sqlite3.h

win32 {
    LIBS += -lws2_32
}
