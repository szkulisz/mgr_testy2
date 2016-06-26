QT += core
QT -= gui

TARGET = load_test
    target.files = load_test
    target.path = /home/szymon/load_tests
INSTALLS+=target

CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += main.cpp \
    profiler.cpp \
    posixtimer.cpp \
    program.cpp \
    nanotimer.cpp

HEADERS += \
    profiler.h \
    posixtimer.h \
    program.h \
    nanotimer.h

QMAKE_LIBS += -lrt
LIBS += -pthread
