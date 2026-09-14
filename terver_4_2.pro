QT += core gui widgets
CONFIG += c++17
TARGET = trajectory_app
TEMPLATE = app
SOURCES += main.cpp Distribution.cpp TrajectoryModel.cpp MainWindow.cpp \
    Distribution.cpp \
    TrajectoryModel.cpp
HEADERS += Distribution.h TrajectoryModel.h MainWindow.h \
    Distribution.h \
    TrajectoryModel.h