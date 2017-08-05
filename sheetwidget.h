/**
   Copyright 2017 Shawn Gilroy

   This file is part of Discounting Model Selector, Qt port.

   Discounting Model Selector is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Discounting Model Selector is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Discounting Model Selector.  If not, see http://www.gnu.org/licenses/.

   The Discounting Model Selector is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

   The SheetWidget class was inspired by the Qt 5.8 Spreadsheet example, its license is below:

   =======================================================================================================

   Copyright (C) 2016 The Qt Company Ltd.
   Contact: https://www.qt.io/licensing/

   This file is part of the demonstration applications of the Qt Toolkit.

   Commercial License Usage
   Licensees holding valid commercial Qt licenses may use this file in
   accordance with the commercial license agreement provided with the
   Software or, alternatively, in accordance with the terms contained in
   a written agreement between you and The Qt Company. For licensing terms
   and conditions see https://www.qt.io/terms-conditions. For further
   information use the contact form at https://www.qt.io/contact-us.

   BSD License Usage
   Alternatively, you may use this file under the terms of the BSD license
   as follows:

   "Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in
       the documentation and/or other materials provided with the
       distribution.
     * Neither the name of The Qt Company Ltd nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.


   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."

  */

#ifndef SHEETWIDGET_H
#define SHEETWIDGET_H

#include <QMainWindow>
#include <QProcess>
#include <QWidget>
#include <QTableWidget>
#include <QtGui>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDomDocument>
#include "systematicchekdialog.h"

#include "calculationsettings.h"
#include "sheetselectdialog.h"
#include "discountingmodelselectioned50dialog.h"
#include "discountingmodelselectionareadialog.h"
#include "resultsdialog.h"
#include "chartwindow.h"
#include "licensedialog.h"
#include "creditsdialog.h"
#include "aboutdialog.h"
#include "calculationworker.h"

#if VERSION_TESTING == 1
    #include <QDebug>
#endif

class SheetWidget : public QMainWindow
{
    Q_OBJECT

public:
    SheetWidget(QWidget *parent = 0);

    void convertExcelColumn(QString &mString, int column);

    QList<FitResults> allResults;
    QList<QStringList> allCharts;

    QUndoStack *undoStack;
    QTableWidget *table;

    CalculationSettings *calculationSettings;

public slots:
    void downloadedFile(QNetworkReply *reply);

protected:
    void buildMenus();

public slots:
    void cut();
    void copy();
    void paste();
    void pasteInverted();
    void clear();

    void checkUpdatesAction();

    void openRecentFile();

    bool eventFilter(QObject *object, QEvent *event);

    void updateDelayModalWindow();
    void updateValueModalWindow();
    void updateMaxValueModalWindow();

    void saveSettings();

    void clearSheet();
    void showOpenFileDialog();
    void showSaveFileDialog();
    void showSaveAsFileDialog();

    void showDiscountingAreaWindow();
    void showDiscountingED50Window();

    void showCreditsWindow();
    void showFAQWindow();

    void showALGLIBLicenseWindow();
    void showDMSLicenseWindow();
    void showBDSLicenseWindow();
    void showQTLicenseWindow();
    void showTangoLicenseWindow();
    void showTestingFeatures();

    bool isToolWindowShown();

    bool areDelayPointsValid(QStringList &delayPoints, bool isRowData, int topDelay, int leftDelay, int bottomDelay, int rightDelay);
    bool areDimensionsValid(bool isRowData, int dWidth, int vWidth, int dLength, int vLength);
    void areValuePointsValid(QStringList &valuePoints, QStringList &tempDelayPoints, QStringList delayPoints, bool isRowData, int topValue, int leftValue, int bottomValue, int rightValue, int i, double maxValue);

    void Calculate();

    void closeEvent(QCloseEvent* event);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();

    void WorkUpdate(FitResults results);
    void WorkFinished();

private:
    QAction *newSheetAction;
    QAction *openSheetAction;
    QAction *saveSheetAction;
    QAction *saveAsSheetAction;
    QAction *updateProgramAction;
    QAction *exitSheetAction;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *pasteInvertedAction;
    QAction *clearAction;

    QAction *undoAction;
    QAction *redoAction;

    QAction *openDiscountingAreaWindow;
    QAction *openDiscountingED50Window;

    QAction *openLicenseDMS;
    QAction *openLicenseBDS;
    QAction *openLicenseALGLIB;
    QAction *openLicenseQt;
    QAction *openLicenseTango;

    QAction *openAbout;
    QAction *openFAQ;

    QAction *delayAction;
    QAction *valueAction;
    QAction *maxValueAction;

    QAction *unlockTesting;

    SystematicChekDialog *checkDialog;

    SheetSelectDialog *sheetSelectDialog;

    DiscountingModelSelectionAreaDialog *discountingAreaDialog;
    DiscountingModelSelectionED50Dialog *discountingED50Dialog;

    LicenseDialog *licenseDialog;

    AboutDialog *aboutDialog;
    CreditsDialog *creditsDialog;

    ResultsDialog *resultsDialog;
    ChartWindow *graphicsWindow;

    bool tripAIC;
    bool tripBIC;
    bool tripBF;
    bool tripRMSE;
    bool tripLogNormal;

    bool displayFigures;

    QString settingsFile;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

    QString strippedName(const QString &fullFileName);
    QString formatStringResult(double value, bool returnLogNormal);
    QString curFile;

    QAction *separatorAct;

    QString mXString;
    QString mYString;

    QList<QPair<QString, QString>> mJohnsonBickelResults;
    QNetworkAccessManager *manager;

    QThread *workerThread;
    CalculationWorker *worker;
};


#endif // SHEETWIDGET_H
