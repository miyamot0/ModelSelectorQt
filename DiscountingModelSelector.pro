#-------------------------------------------------
#
# Discounting Model Selector, Qt Port
# Copyright 2017, Shawn P. Gilroy
# Released under the GPL-V3 license
#
# Source code for project hosted at:
#
# https://github.com/miyamot0/ModelSelectorQt
#
#-------------------------------------------------

#-------------------------------------------------
#
# Project created by QtCreator 2017-03-23T21:59:26
#
#-------------------------------------------------

TEST_FEATURES = 0

VERSION_MAJOR = 1
VERSION_MINOR = 2
VERSION_BUILD = 6

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"\
       "VERSION_TESTING=$$TEST_FEATURES"

QT += core gui widgets xlsx charts network xml

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
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
    sheetselectdialog.cpp \
    resultsdialog.cpp \
    sheetwidget.cpp \
    licensedialog.cpp \
    aboutdialog.cpp \
    creditsdialog.cpp \
    discountingmodelselectioned50dialog.cpp \
    discountingmodelselectionareadialog.cpp \
    Libraries/alglib-3.11.0/src/alglibinternal.cpp \
    Libraries/alglib-3.11.0/src/alglibmisc.cpp \
    Libraries/alglib-3.11.0/src/ap.cpp \
    Libraries/alglib-3.11.0/src/dataanalysis.cpp \
    Libraries/alglib-3.11.0/src/diffequations.cpp \
    Libraries/alglib-3.11.0/src/fasttransforms.cpp \
    Libraries/alglib-3.11.0/src/integration.cpp \
    Libraries/alglib-3.11.0/src/interpolation.cpp \
    Libraries/alglib-3.11.0/src/linalg.cpp \
    Libraries/alglib-3.11.0/src/optimization.cpp \
    Libraries/alglib-3.11.0/src/solvers.cpp \
    Libraries/alglib-3.11.0/src/specialfunctions.cpp \
    Libraries/alglib-3.11.0/src/statistics.cpp \
    modelselection.cpp \
    chartwindow.cpp \
    systematicchekdialog.cpp

HEADERS  += \
    sheetselectdialog.h \
    resultsdialog.h \
    sheetwidget.h \
    licensedialog.h \
    aboutdialog.h \
    creditsdialog.h \
    discountingmodelselectioned50dialog.h \
    discountingmodelselectionareadialog.h \
    Libraries/alglib-3.11.0/src/alglibinternal.h \
    Libraries/alglib-3.11.0/src/alglibmisc.h \
    Libraries/alglib-3.11.0/src/ap.h \
    Libraries/alglib-3.11.0/src/dataanalysis.h \
    Libraries/alglib-3.11.0/src/diffequations.h \
    Libraries/alglib-3.11.0/src/fasttransforms.h \
    Libraries/alglib-3.11.0/src/integration.h \
    Libraries/alglib-3.11.0/src/interpolation.h \
    Libraries/alglib-3.11.0/src/linalg.h \
    Libraries/alglib-3.11.0/src/optimization.h \
    Libraries/alglib-3.11.0/src/solvers.h \
    Libraries/alglib-3.11.0/src/specialfunctions.h \
    Libraries/alglib-3.11.0/src/statistics.h \
    Libraries/alglib-3.11.0/src/stdafx.h \
    modelselection.h \
    chartwindow.h \
    systematicchekdialog.h

FORMS    += \
    sheetselectdialog.ui \
    resultsdialog.ui \
    licensedialog.ui \
    aboutdialog.ui \
    creditsdialog.ui \
    discountingmodelselectioned50dialog.ui \
    discountingmodelselectionareadialog.ui \
    systematicchekdialog.ui

RESOURCES += \
    spreadsheet.qrc

###
#
# Win macro's, route to appropriate directories for installation prep
#
###
win32 {
    INCLUDEPATH += Libraries/alglib-3.11.0/src

    win32:RC_ICONS += SNS.ico

    DMS_FILES.files = License_BDS.txt \
                    License_gnome_icons.txt \
                    License_Qt.txt \
                    License_ALGLIB.txt \
                    License_Tango.txt \
                    COPYING \
                    SNS.ico

    CONFIG(debug, debug|release) {
        DESTDIR = $$OUT_PWD/build/debug
    } else {
        DESTDIR = $$OUT_PWD/build/release
    }

    DMS_FILES.path = $$DESTDIR

    INSTALLS += DMS_FILES
}

###
#
# Mac macro's, route to appropriate directories for installation prep
#
###
macx {
    INCLUDEPATH += Libraries/alglib-3.11.0/src

    macx:ICON = $${PWD}/SNS.icns

    DMS_FILES.files = License_BDS.txt \
                    License_gnome_icons.txt \
                    License_Qt.txt \
                    License_ALGLIB.txt \
                    License_Tango.txt \
                    COPYING \
                    SNS.icns

    DMS_FILES.path = Contents/Resources

    QMAKE_BUNDLE_DATA += DMS_FILES
}

DISTFILES += \
    README.md \
    COPYING \
    License_BDS.txt \
    License_Qt.txt \
    License_ALGLIB.txt \
    License_Tango.txt \
    SNS.ico \
    SNS.icns
