QT += core gui widgets
CONFIG += c++17
TARGET = random_walk
TEMPLATE = app
SOURCES += main.cpp MainWindow.cpp RandomWalkModel.cpp \
    RandomWalkModel.cpp
HEADERS += MainWindow.h RandomWalkModel.h \
    RandomWalkModel.h
DISTFILES += \
    config.json