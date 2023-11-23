TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt


SOURCES += \
        main.cpp \
        tcpsocket.cpp

HEADERS += \
    tcpsocket.h

win32 {
    LIBS += -lws2_32
}
