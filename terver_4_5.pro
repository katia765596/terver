QT += core gui widgets
CONFIG += c++17
TARGET = step_figures
TEMPLATE = app
SOURCES += main.cpp MainWindow.cpp RandomDistributions.cpp StepFigureModel.cpp \
    RandomDistributions.cpp \
    StepFigureModel.cpp
HEADERS += MainWindow.h RandomDistributions.h StepFigureModel.h