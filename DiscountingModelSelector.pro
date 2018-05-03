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

TEST_FEATURES = 1

VERSION_MAJOR = 1
VERSION_MINOR = 7
VERSION_BUILD = 0

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

SOURCES += \
    Controls/sheetwidget.cpp \
    Controls/chartwindow.cpp \
    Dialogs/aboutdialog.cpp \
    Dialogs/creditsdialog.cpp \
    Dialogs/discountingmodelselectioned50dialog.cpp \
    Dialogs/licensedialog.cpp \
    Dialogs/resultsdialog.cpp \
    Dialogs/sheetselectdialog.cpp \
    Dialogs/systematicchekdialog.cpp \
    Libraries/alglib-3.11.0/src/alglibinternal.cpp \
    Libraries/alglib-3.11.0/src/alglibmisc.cpp \
    Libraries/alglib-3.11.0/src/ap.cpp \
    Libraries/alglib-3.11.0/src/integration.cpp \
    Libraries/alglib-3.11.0/src/interpolation.cpp \
    Libraries/alglib-3.11.0/src/linalg.cpp \
    Libraries/alglib-3.11.0/src/optimization.cpp \
    Libraries/alglib-3.11.0/src/solvers.cpp \
    Libraries/alglib-3.11.0/src/specialfunctions.cpp \
    Libraries/alglib-3.11.0/src/statistics.cpp \
    Helpers/Threading/calculationworker.cpp \
    Modeling/modelselection.cpp \
    Models/calculationsettings.cpp \
    Models/fitresult.cpp \
    Models/fitresults.cpp \
    Utilities/commanding.cpp \
    Utilities/qcustomplot.cpp \
    Utilities/sheetdelegate.cpp \
    main.cpp

HEADERS  += \
    Controls/sheetwidget.h \
    Controls/chartwindow.h \
    Dialogs/aboutdialog.h \
    Dialogs/creditsdialog.h \
    Dialogs/discountingmodelselectioned50dialog.h \
    Dialogs/licensedialog.h \
    Dialogs/resultsdialog.h \
    Dialogs/sheetselectdialog.h \
    Dialogs/systematicchekdialog.h \
    Libraries/alglib-3.11.0/src/alglibinternal.h \
    Libraries/alglib-3.11.0/src/alglibmisc.h \
    Libraries/alglib-3.11.0/src/ap.h \
    Libraries/alglib-3.11.0/src/integration.h \
    Libraries/alglib-3.11.0/src/interpolation.h \
    Libraries/alglib-3.11.0/src/linalg.h \
    Libraries/alglib-3.11.0/src/optimization.h \
    Libraries/alglib-3.11.0/src/solvers.h \
    Libraries/alglib-3.11.0/src/specialfunctions.h \
    Libraries/alglib-3.11.0/src/statistics.h \
    Libraries/alglib-3.11.0/src/stdafx.h \
    Helpers/Threading/calculationworker.h \
    Modeling/modelselection.h \
    Models/calculationsettings.h \
    Models/fitresult.h \
    Models/fitresults.h \
    Modeling/gridsearch.h \
    Utilities/commanding.h \
    Utilities/qcustomplot.h \
    Utilities/sheetdelegate.h \
    Utilities/sheettools.h

FORMS    += \
    Dialogs/aboutdialog.ui \
    Dialogs/creditsdialog.ui \
    Dialogs/discountingmodelselectioned50dialog.ui \
    Dialogs/licensedialog.ui \
    Dialogs/resultsdialog.ui \
    Dialogs/sheetselectdialog.ui \
    Dialogs/systematicchekdialog.ui

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
                    License_Qt.txt \
                    License_ALGLIB.txt \
                    License_Tango.txt \
                    License_QtXlsx.txt \
                    COPYING \
                    scripts/manualDiscountingEd50AreaComputation.R \
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
                    License_Qt.txt \
                    License_ALGLIB.txt \
                    License_Tango.txt \
                    License_QtXlsx.txt \
                    COPYING \
                    scripts/manualDiscountingEd50AreaComputation.R \
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
    License_QtXlsx.txt \
    SNS.ico \
    SNS.icns \
    scripts/manualDiscountingEd50AreaComputation.R \
    screencaps/Analyses.gif \
    screencaps/SpreadsheetInterface.gif \
    screencaps/SpreadsheetOptions.gif
