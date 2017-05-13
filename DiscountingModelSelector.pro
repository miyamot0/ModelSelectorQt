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

QT += core gui widgets xlsx svg

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
    resultsdialog.cpp \
    statusdialog.cpp \
    fitworker.cpp \
    sheetwidget.cpp \
    graphicaloutputdialog.cpp \
    licensedialog.cpp \
    aboutdialog.cpp \
    creditsdialog.cpp \
    rworker.cpp \
    discountingmodelselectioned50dialog.cpp \
    discountingmodelselectionareadialog.cpp

HEADERS  += \
    sheetselectdialog.h \
    resultsdialog.h \
    statusdialog.h \
    fitworker.h \
    sheetwidget.h \
    graphicaloutputdialog.h \
    licensedialog.h \
    aboutdialog.h \
    creditsdialog.h \
    rworker.h \
    discountingmodelselectioned50dialog.h \
    discountingmodelselectionareadialog.h

FORMS    += \
    sheetselectdialog.ui \
    resultsdialog.ui \
    statusdialog.ui \
    graphicaloutputdialog.ui \
    licensedialog.ui \
    aboutdialog.ui \
    creditsdialog.ui \
    discountingmodelselectioned50dialog.ui \
    discountingmodelselectionareadialog.ui

RESOURCES += \
    spreadsheet.qrc

win32 {
    DMS_FILES.files = scripts/DiscountingAreaComputation.R \
                    scripts/DiscountingED50Computation.R scripts/installDependencyBase64.R \
                    scripts/installDependencyJsonlite.R scripts/installDependencyReshape.R \
                    License_base64enc.txt \
                    License_BDS.txt \
                    License_gnome_icons.txt \
                    License_jsonlite.txt \
                    License_NLS.txt \
                    License_Qt.txt \
                    License_R.txt \
                    License_reshape.txt \
                    COPYING

    release: DESTDIR = $$OUT_PWD/build/release
    debug:   DESTDIR = $$OUT_PWD/build/debug

    DMS_FILES.path = $$DESTDIR

    INSTALLS += DMS_FILES
}
macx {
    DMS_FILES.files = scripts/DiscountingAreaComputation.R \
                    scripts/DiscountingED50Computation.R scripts/installDependencyBase64.R \
                    scripts/installDependencyJsonlite.R scripts/installDependencyReshape.R \
                    License_base64enc.txt \
                    License_BDS.txt \
                    License_gnome_icons.txt \
                    License_jsonlite.txt \
                    License_NLS.txt \
                    License_Qt.txt \
                    License_R.txt \
                    License_reshape.txt \
                    COPYING

    DMS_FILES.path = Contents/Resources

    QMAKE_BUNDLE_DATA += DMS_FILES
}

DISTFILES += \
    README.md \
    scripts/DiscountingAreaComputation.R \
    scripts/DiscountingED50Computation.R \
    scripts/installDependencyJsonlite.R \
    scripts/installDependencyBase64.R \
    scripts/installDependencyReshape.R \
    COPYING \
    License_BDS.txt \
    License_NLS.txt \
    License_Qt.txt \
    License_R.txt \
    License_base64enc.txt \
    License_gnome_icons.txt \
    License_jsonlite.txt \
    License_reshape.txt
