#-------------------------------------------------
#
# Discounting Model Selector, Qt Port
# Copyright 2007, Shawn P. Gilroy
# Released under the GPL-V3 license
#
#-------------------------------------------------

#-------------------------------------------------
#
# Project created by QtCreator 2017-03-23T21:59:26
#
#-------------------------------------------------

QT += core gui widgets xlsx

TARGET = DiscountingModelSelector

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it...
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
    sheetselectdialog.cpp \
    modelselectiondialog.cpp \
    resultsdialog.cpp \
    statusdialog.cpp \
    fitworker.cpp \
    sheetwidget.cpp \
    graphicaloutputdialog.cpp \
    licensedialog.cpp \
    aboutdialog.cpp \
    creditsdialog.cpp \
    rworker.cpp

HEADERS  += \
    sheetselectdialog.h \
    modelselectiondialog.h \
    resultsdialog.h \
    statusdialog.h \
    fitworker.h \
    sheetwidget.h \
    graphicaloutputdialog.h \
    licensedialog.h \
    aboutdialog.h \
    creditsdialog.h \
    rworker.h

FORMS    += \
    sheetselectdialog.ui \
    modelselectiondialog.ui \
    resultsdialog.ui \
    statusdialog.ui \
    graphicaloutputdialog.ui \
    licensedialog.ui \
    aboutdialog.ui \
    creditsdialog.ui

RESOURCES += \
    spreadsheet.qrc
