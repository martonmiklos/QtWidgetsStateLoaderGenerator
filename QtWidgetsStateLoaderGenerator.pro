#-------------------------------------------------
#
# Project created by QtCreator 2014-07-12T09:17:26
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtwidgets_stateloader_gen
TEMPLATE = app

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build
DESTDIR = bin

SOURCES += main.cpp\
        mainwindow.cpp \
    settingssavermodel.cpp \
    uifileimporter.cpp \
    widgetsettingssaver.cpp \
    mainwindow_settings.cpp \
    projectparser.cpp

HEADERS  += mainwindow.h \
    settingssavermodel.h \
    uifileimporter.h \
    widgetsettingssaver.h \
    projectparser.h

FORMS    += mainwindow.ui \
    widgetsettingssaver.ui

OTHER_FILES += \
    README.md
