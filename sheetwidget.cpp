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

   The SheetWidget class was inspired by the Qt 5.8 Spreadsheet example and Recent Files example, its license is below:

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

   The Discounting Model Selector uses ALGLIB to provide access to mathematical methods.

   ====================================================================================

   ALGLIB 3.11.0 (source code generated 2017-05-11)
   Copyright (c) Sergey Bochkanov (ALGLIB project).

   >>> SOURCE LICENSE >>>
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation (www.fsf.org); either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   A copy of the GNU General Public License is available at
   http://www.fsf.org/licensing/licenses
   >>> END OF LICENSE >>>

  */

#include <QtWidgets>
#include <QtXlsx>
#include <QTableWidgetItem>
#include <QtCharts>

#include "sheetwidget.h"
#include "resultsdialog.h"
#include "chartwindow.h"

QTXLSX_USE_NAMESPACE

struct QPairSecondComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> &one, const QPair<T1,T2> &two) const
    {
        return one.second > two.second;
    }
};

SheetWidget::SheetWidget(QWidget *parent) : QMainWindow(parent)
{
    table = new QTableWidget(10000, 10000, this);
    table->setSizeAdjustPolicy(QTableWidget::AdjustToContents);

    #ifdef TARGET_OS_MAC
        table->setStyleSheet("QTableView {selection-background-color: #73E2A7; selection-color: #73E2A7;}");
    #endif

    QString value;

    for (int c = 0; c < 10000; ++c)
    {
        value = "";
        convertExcelColumn(value, c);
        table->setHorizontalHeaderItem(c, new QTableWidgetItem(value));
    }

    buildMenus();
    setCentralWidget(table);

    setWindowTitle("Discounting Model Selector v" + QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));

    this->layout()->setSizeConstraint(QLayout::SetNoConstraint);

    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    #ifdef _WIN32

    this->window()->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            window()->size(),
            qApp->desktop()->availableGeometry()
        )
    );

    #endif

    mObj = new ModelSelection();
    table->installEventFilter( this );
}

void SheetWidget::buildMenus()
{
    /** File actions
     * @brief
     */

    newSheetAction = new QAction("N&ew Sheet", this);
    newSheetAction->setIcon(QIcon(":/images/document-new.png"));
    connect(newSheetAction, &QAction::triggered, this, &SheetWidget::clearSheet);

    openSheetAction = new QAction("I&mport a Sheet", this);
    openSheetAction->setShortcut(QKeySequence("Ctrl+O"));
    openSheetAction->setIcon(QIcon(":/images/document-open.png"));
    connect(openSheetAction, &QAction::triggered, this, &SheetWidget::showOpenFileDialog);

    saveSheetAction = new QAction("S&ave Sheet", this);
    saveSheetAction->setShortcut(QKeySequence("Ctrl+S"));
    saveSheetAction->setIcon(QIcon(":/images/document-save.png"));
    connect(saveSheetAction, &QAction::triggered, this, &SheetWidget::showSaveFileDialog);

    updateProgramAction = new QAction("C&heck Updates", this);
    updateProgramAction->setIcon(QIcon(":/images/view-refresh.png"));
    connect(updateProgramAction, &QAction::triggered, this, &SheetWidget::checkUpdatesAction);

    exitSheetAction = new QAction("E&xit", this);
    exitSheetAction->setShortcut(QKeySequence("Ctrl+Q"));
    exitSheetAction->setIcon(QIcon(":/images/application-exit.png"));
    connect(exitSheetAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    /** Window actions
     * @brief
     */

    openDiscountingAreaWindow = new QAction("M&odel Selection (AUC)", this);
    openDiscountingAreaWindow->setIcon(QIcon(":/images/applications-other.png"));
    connect(openDiscountingAreaWindow, &QAction::triggered, this, &SheetWidget::showDiscountingAreaWindow);

    openDiscountingED50Window = new QAction("M&odel Selection (ED50)", this);
    openDiscountingED50Window->setIcon(QIcon(":/images/applications-other.png"));
    connect(openDiscountingED50Window, &QAction::triggered, this, &SheetWidget::showDiscountingED50Window);

    /** Edit actions
     * @brief
     */

    cutAction = new QAction("Cut", this);
    cutAction->setShortcut(QKeySequence("Ctrl+X"));
    cutAction->setIcon(QIcon(":/images/edit-cut.png"));
    connect(cutAction, &QAction::triggered, this, &SheetWidget::cut);

    copyAction = new QAction("Copy", this);
    copyAction->setShortcut(QKeySequence("Ctrl+C"));
    copyAction->setIcon(QIcon(":/images/edit-copy.png"));
    connect(copyAction, &QAction::triggered, this, &SheetWidget::copy);

    pasteAction = new QAction("Paste", this);
    pasteAction->setShortcut(QKeySequence("Ctrl+V"));
    pasteAction->setIcon(QIcon(":/images/edit-paste.png"));
    connect(pasteAction, &QAction::triggered, this, &SheetWidget::paste);

    pasteInvertedAction = new QAction("Paste Transposed", this);
    pasteInvertedAction->setShortcut(QKeySequence("Ctrl+B"));
    pasteInvertedAction->setIcon(QIcon(":/images/edit-paste.png"));
    connect(pasteInvertedAction, &QAction::triggered, this, &SheetWidget::pasteInverted);

    clearAction = new QAction("Clear", this);
    clearAction->setShortcut(Qt::Key_Delete);
    clearAction->setIcon(QIcon(":/images/edit-clear.png"));
    connect(clearAction, &QAction::triggered, this, &SheetWidget::clear);

    /** Window actions
     * @brief
     */

    discountingAreaDialog = new DiscountingModelSelectionAreaDialog(this);

    discountingED50Dialog = new DiscountingModelSelectionED50Dialog(this);

    openLicenseDMS = new QAction("DMS License (GPL-V3)", this);
    openLicenseDMS->setIcon(QIcon(":/images/text-x-generic.png"));
    connect(openLicenseDMS, &QAction::triggered, this, &SheetWidget::showDMSLicenseWindow);

    openLicenseALGLIB = new QAction("ALGLIB License (GPL-V3)", this);
    openLicenseALGLIB->setIcon(QIcon(":/images/text-x-generic.png"));
    connect(openLicenseALGLIB, &QAction::triggered, this, &SheetWidget::showALGLIBLicenseWindow);

    openLicenseBDS = new QAction("BDS License (GPL-V3)", this);
    openLicenseBDS->setIcon(QIcon(":/images/text-x-generic.png"));
    connect(openLicenseBDS, &QAction::triggered, this, &SheetWidget::showBDSLicenseWindow);

    openLicenseQt = new QAction("Qt License (GPL-V3)", this);
    openLicenseQt->setIcon(QIcon(":/images/text-x-generic.png"));
    connect(openLicenseQt, &QAction::triggered, this, &SheetWidget::showQTLicenseWindow);

    openLicenseGnome = new QAction("Gnome Icons License (GPL-V3)", this);
    openLicenseGnome->setIcon(QIcon(":/images/text-x-generic.png"));
    connect(openLicenseGnome, &QAction::triggered, this, &SheetWidget::showGnomeLicenseWindow);

    openAbout = new QAction("Credits", this);
    openAbout->setIcon(QIcon(":/images/text-x-generic.png"));
    connect(openAbout, &QAction::triggered, this, &SheetWidget::showCreditsWindow);

    openFAQ = new QAction("FAQ", this);
    openFAQ->setIcon(QIcon(":/images/text-x-generic.png"));
    connect(openFAQ, &QAction::triggered, this, &SheetWidget::showFAQWindow);

    /** Window helper actions
     * @brief
     */

    delayAction = new QAction("Set Delays", this);
    delayAction->setIcon(QIcon(":/images/system-run.png"));
    connect(delayAction, &QAction::triggered, this, &SheetWidget::updateDelayModalWindow);

    valueAction = new QAction("Set Values", this);
    valueAction->setIcon(QIcon(":/images/system-run.png"));
    connect(valueAction, &QAction::triggered, this, &SheetWidget::updateValueModalWindow);

    maxValueAction = new QAction("Set Max Value", this);
    maxValueAction->setIcon(QIcon(":/images/system-run.png"));
    connect(maxValueAction, &QAction::triggered, this, &SheetWidget::updateMaxValueModalWindow);

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

    /** Menu area below
     * @brief
     */

    QMenu *sheetOptionsMenu = menuBar()->addMenu(tr("&File"));
    sheetOptionsMenu->addAction(newSheetAction);
    sheetOptionsMenu->addAction(openSheetAction);
    sheetOptionsMenu->addAction(saveSheetAction);

    separatorAct = sheetOptionsMenu->addSeparator();

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        sheetOptionsMenu->addAction(recentFileActs[i]);
    }

    sheetOptionsMenu->addSeparator();
    sheetOptionsMenu->addAction(updateProgramAction);
    sheetOptionsMenu->addSeparator();
    sheetOptionsMenu->addAction(exitSheetAction);

    updateRecentFileActions();

    QMenu *sheetEditMenu = menuBar()->addMenu(tr("&Edit"));
    sheetEditMenu->addAction(cutAction);
    sheetEditMenu->addAction(copyAction);
    sheetEditMenu->addAction(pasteAction);
    sheetEditMenu->addAction(pasteInvertedAction);
    sheetEditMenu->addSeparator();
    sheetEditMenu->addAction(clearAction);

    QMenu *sheetCalculationsMenu = menuBar()->addMenu(tr("&Discounting"));
    sheetCalculationsMenu->addAction(openDiscountingED50Window);

    if (VERSION_TESTING == 1)
    {
        sheetCalculationsMenu->addAction(openDiscountingAreaWindow);
    }

    QMenu *sheetLicensesMenu = menuBar()->addMenu(tr("&Licenses"));
    sheetLicensesMenu->addAction(openLicenseALGLIB);
    sheetLicensesMenu->addAction(openLicenseDMS);
    sheetLicensesMenu->addAction(openLicenseBDS);
    sheetLicensesMenu->addAction(openLicenseQt);
    sheetLicensesMenu->addAction(openLicenseGnome);
    sheetLicensesMenu->addAction(openAbout);

    QMenu *sheetAboutMenu = menuBar()->addMenu(tr("&Help"));
    sheetAboutMenu->addAction(openFAQ);

    /** Context Menu
     * @brief
     */

    addAction(delayAction);
    addAction(valueAction);
    addAction(maxValueAction);

    QAction *separatorTwo = new QAction(this);
    separatorTwo->setSeparator(true);
    addAction(separatorTwo);

    addAction(cutAction);
    addAction(copyAction);
    addAction(pasteAction);
    addAction(pasteInvertedAction);

    QAction *separatorThree = new QAction(this);
    separatorThree->setSeparator(true);
    addAction(separatorThree);

    addAction(clearAction);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void SheetWidget::checkUpdatesAction()
{
    QString mCommand = "";

#ifdef _WIN32
    mCommand = "maintenancetool.exe";
#elif TARGET_OS_MAC
    QDir mDir = QDir(QCoreApplication::applicationDirPath());
    mDir.cdUp();
    mDir.cdUp();
    mDir.cdUp();

    mCommand = QDir::cleanPath(mDir.path() + QDir::separator() + "maintenancetool.app");
#endif

    if (QFile::exists(mCommand))
    {
        QProcess p;
        QStringList args;
        args << "--updater";
        p.start(mCommand, args);
        p.waitForStarted();
        p.waitForFinished(-1);
    }
}

void SheetWidget::clearSheet()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    table->clearContents();

    curFile = "";
    setWindowFilePath(curFile);

    QApplication::setOverrideCursor(Qt::WaitCursor);
}

/** Window methods
 * @brief
 */

void SheetWidget::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton confirm = QMessageBox::question( this, "Discounting Model Selector",
                                                                tr("Are you sure you want to quit?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (confirm != QMessageBox::Yes) {
        event->ignore();
    } else {
        saveSettings();
        event->accept();
    }
}

void SheetWidget::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString mFile = action->data().toString();

        if (QFile::exists(mFile))
        {
            Document xlsx2(mFile);
            QStringList sheets = xlsx2.sheetNames();

            sheetSelectDialog = new SheetSelectDialog(this);
            sheetSelectDialog->UpdateUI(sheets);
            sheetSelectDialog->setModal(true);

            if(sheetSelectDialog->exec())
            {
                table->clearContents();

                xlsx2.selectSheet(sheetSelectDialog->GetSelected());

                for (int w = 0; w < xlsx2.dimension().lastColumn() + 1; w++)
                {
                    for (int h = 0; h < xlsx2.dimension().lastRow() + 1; h++)
                    {
                        if (QXlsx::Cell *cell = xlsx2.cellAt(h, w))
                        {
                            if (cell->cellType() == Cell::NumberType || cell->cellType() == Cell::StringType || cell->cellType() == Cell::SharedStringType)
                            {
                                table->setItem(h-1, w-1, new QTableWidgetItem(cell->value().toString()));
                            }
                        }
                    }
                }

                setCurrentFile(mFile);
                statusBar()->showMessage(tr("File loaded"), 2000);
            }
        }
    }
}

void SheetWidget::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings(QSettings::UserScope, QLatin1String("Discounting Model Selector"));
    settings.beginGroup(QLatin1String("SheetWindow"));

    QStringList files = settings.value(QLatin1String("recentFileList")).toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);

    while (files.size() > MaxRecentFiles)
    {
        files.removeLast();
    }

    settings.setValue("recentFileList", files);
    settings.endGroup();
    settings.sync();

    updateRecentFileActions();
}

void SheetWidget::saveSettings()
{
    QSettings settings(QSettings::UserScope, QLatin1String("Discounting Model Selector"));
    settings.beginGroup(QLatin1String("SheetWindow"));

    QStringList files = settings.value("recentFileList").toStringList();

    settings.setValue(QLatin1String("recentFileList"), files);
    settings.endGroup();

    settings.sync();
}

/** Window methods
 * @brief
 */

bool SheetWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        auto keyCode = static_cast<QKeyEvent *>(event);
        if (keyCode->key() == (int) Qt::Key_Return)
        {
            if (table->currentRow() + 1 >= table->rowCount())
            {
                return QObject::eventFilter(object, event);
            }

            table->setCurrentCell(table->currentRow() + 1, table->currentColumn());
        }
    }

    return QObject::eventFilter(object, event);
}

void SheetWidget::showOpenFileDialog()
{
    QString file_name;
    QString fileFilter = "Spreadsheet (*.xlsx)";

    #ifdef _WIN32

    file_name = QFileDialog::getOpenFileName(this, "Open spreadsheet file", QDir::homePath(),
                                             fileFilter);

    #elif TARGET_OS_MAC

    file_name = QFileDialog::getOpenFileName(nullptr, "Open spreadsheet file", QDir::homePath(),
                                             fileFilter, nullptr, QFileDialog::Option::DontUseNativeDialog);

    #endif

    if(!file_name.trimmed().isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        Document xlsx2(file_name);
        QStringList sheets = xlsx2.sheetNames();

        sheetSelectDialog = new SheetSelectDialog(this);
        sheetSelectDialog->UpdateUI(sheets);
        sheetSelectDialog->setModal(true);

        if(sheetSelectDialog->exec())
        {
            table->clearContents();

            xlsx2.selectSheet(sheetSelectDialog->GetSelected());

            for (int w = 0; w < xlsx2.dimension().lastColumn() + 1; w++)
            {
                for (int h = 0; h < xlsx2.dimension().lastRow() + 1; h++)
                {
                    if (QXlsx::Cell *cell = xlsx2.cellAt(h, w))
                    {
                        if (cell->cellType() == Cell::NumberType || cell->cellType() == Cell::StringType || cell->cellType() == Cell::SharedStringType)
                        {
                            table->setItem(h-1, w-1, new QTableWidgetItem(cell->value().toString()));
                        }
                    }
                }
            }

            setCurrentFile(file_name);
            statusBar()->showMessage(tr("File loaded"), 2000);
        }

        QApplication::restoreOverrideCursor();
    }
}

void SheetWidget::updateRecentFileActions()
{
    QSettings settings(QSettings::UserScope, QLatin1String("Discounting Model Selector"));
    settings.beginGroup(QLatin1String("SheetWindow"));
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    {
        recentFileActs[j]->setVisible(false);
    }

    separatorAct->setVisible(numRecentFiles > 0);
}

void SheetWidget::showSaveFileDialog()
{

    QString file_name;
    QString fileFilter = "Spreadsheet (*.xlsx)";

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, "Save spreadsheet file", QDir::homePath(),
                                         fileFilter);

#elif TARGET_OS_MAC

        file_name = QFileDialog::getSaveFileName(this, "Save spreadsheet file", QDir::homePath(),
                                         fileFilter, &fileFilter, QFileDialog::Option::DontUseNativeDialog);

        if (!file_name.contains(".xlsx"))
        {
            file_name.append(".xlxs");
        }

#endif

    if(!file_name.trimmed().isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QXlsx::Document xlsx;

        int rows = table->rowCount();
        int cols = table->columnCount();

        QString temp;

        for (int i=0; i<rows; i++)
        {
            for (int j=0; j<cols; j++)
            {
                QTableWidgetItem *item = table->item(i, j);

                if (item != NULL && !item->text().isEmpty())
                {
                    temp = table->item(i, j)->data(Qt::DisplayRole).toString();
                    xlsx.write(i + 1, j + 1, temp);
                }
            }
        }

        xlsx.saveAs(file_name);
        setCurrentFile(file_name);

        QApplication::restoreOverrideCursor();

        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void SheetWidget::showDiscountingAreaWindow()
{
    if (isToolWindowShown())
    {
        return;
    }

    discountingAreaDialog = new DiscountingModelSelectionAreaDialog(this);
    discountingAreaDialog->setModal(false);
    discountingAreaDialog->show();
}

void SheetWidget::showDiscountingED50Window()
{
    if (isToolWindowShown())
    {
        return;
    }

    discountingED50Dialog = new DiscountingModelSelectionED50Dialog(this);
    discountingED50Dialog->setModal(false);
    discountingED50Dialog->show();
}

void SheetWidget::showDMSLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "COPYING";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "COPYING";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle("DMS License (GPL-V3)");
    licenseDialog->setModal(true);
    licenseDialog->show();
}

void SheetWidget::showALGLIBLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_ALGLIB.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_ALGLIB.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle("DMS License (GPL-V3)");
    licenseDialog->setModal(true);
    licenseDialog->show();
}

void SheetWidget::showBDSLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_BDS.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_BDS.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle("DMS License (GPL-V3)");
    licenseDialog->setModal(true);
    licenseDialog->show();
}

void SheetWidget::showQTLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_Qt.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_Qt.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle("Qt License (LGPL-V3)");
    licenseDialog->setModal(true);
    licenseDialog->show();
}

void SheetWidget::showGnomeLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_gnome_icons.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_gnome_icons.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle("Gnome Icon Set License (GPL-V3)");
    licenseDialog->setModal(true);
    licenseDialog->show();
}

void SheetWidget::showCreditsWindow()
{
    creditsDialog = new CreditsDialog();
    creditsDialog->setModal(true);
    creditsDialog->show();
}

void SheetWidget::showFAQWindow()
{
    aboutDialog = new AboutDialog();
    aboutDialog->setModal(true);
    aboutDialog->show();
}

/** Edit methods
 * @brief
 */

void SheetWidget::cut()
{
    SheetWidget::copy();
    SheetWidget::clear();
}

void SheetWidget::copy()
{
    if (table->selectedItems().count() < 1)
    {
        return;
    }

    QList<QTableWidgetSelectionRange> range = table->selectedRanges();
    QTableWidgetSelectionRange mRange = range.first();

    QString str;

    for (int i = 0; i < mRange.rowCount(); ++i) {
        if (i > 0)
        {
            str += "\n";
        }

        for (int j = 0; j < mRange.columnCount(); ++j) {
            if (j > 0)
            {
                str += "\t";
            }

            if (table->item(mRange.topRow() + i, mRange.leftColumn() + j) == NULL)
            {
                str += "";
            }
            else
            {
                str += table->item(mRange.topRow() + i, mRange.leftColumn() + j)->data(Qt::DisplayRole).toString();
            }
        }
    }

    QApplication::clipboard()->setText(str);
}

void SheetWidget::paste()
{
    QTableWidgetSelectionRange range = table->selectedRanges().first();
    QString pasteString = QApplication::clipboard()->text();
    QStringList pasteRows = pasteString.split('\n');

    int nRows = pasteRows.count();
    int nCols = pasteRows.first().count('\t') + 1;

    for (int i = 0; i < nRows; ++i) {
        QStringList columns = pasteRows[i].split('\t');

        for (int j = 0; j < nCols; ++j) {
            int row = range.topRow() + i;
            int column = range.leftColumn() + j;

            if (row < 10000 && column < 10000)
            {
                if (table->item(row, column) != NULL)
                {
                    if (j < columns.length())
                    {
                        table->item(row, column)->setText(columns[j]);
                    }
                }
                else
                {
                    if (j < columns.length())
                    {
                        table->setItem(row, column, new QTableWidgetItem(columns[j]));
                    }
                }
            }
        }
    }

    table->viewport()->update();
}

void SheetWidget::pasteInverted()
{
    QTableWidgetSelectionRange range = table->selectedRanges().first();
    QString pasteString = QApplication::clipboard()->text();
    QStringList pasteRows = pasteString.split('\n');

    int nRows = pasteRows.count();
    int nCols = pasteRows.first().count('\t') + 1;

    for (int i = 0; i < nRows; ++i) {
        QStringList columns = pasteRows[i].split('\t');

        for (int j = 0; j < nCols; ++j) {
            int row = range.topRow() + j;
            int column = range.leftColumn() + i;

            if (row < 10000 && column < 10000)
            {
                if (table->item(row, column) != NULL)
                {
                    if (j < columns.length())
                    {
                        table->item(row, column)->setText(columns[j]);
                    }
                }
                else
                {
                    if (j < columns.length())
                    {
                        table->setItem(row, column, new QTableWidgetItem(columns[j]));
                    }
                }
            }
        }
    }

    table->viewport()->update();
}

void SheetWidget::clear()
{
    if (table->selectedItems().count() < 1)
    {
        return;
    }

    foreach (QTableWidgetItem *i, table->selectedItems())
    {
        i->setText("");
    }
}

/** Window helper methods
 * @brief
 */

void SheetWidget::updateDelayModalWindow()
{
    if (!isToolWindowShown())
    {
        return;
    }

    QList<QTableWidgetSelectionRange> mList = table->selectedRanges();
    QTableWidgetSelectionRange range = mList.first();

    QString mLeft = "";
    convertExcelColumn(mLeft, range.leftColumn());

    QString mRight = "";
    convertExcelColumn(mRight, range.rightColumn());

    mLeft.append(QString::number(range.topRow() + 1));
    mLeft.append(":");
    mLeft.append(mRight);
    mLeft.append(QString::number(range.bottomRow() + 1));

    int mWidth = range.rightColumn() - range.leftColumn();
    int mHeight = range.bottomRow() - range.topRow();

    if (mWidth > 0 && mHeight > 0)
    {
        QMessageBox::critical(this, "Error", "Please select a vector of delays (e.g., one column or one row).");
        return;
    }
    else if (mWidth + mHeight < 3)
    {
        QMessageBox::critical(this, "Error", "Please select at least 3 delay points. At least 3 points are needed to proceed with curve fitting.");
        return;
    }

    if (discountingAreaDialog->isVisible())
    {
        discountingAreaDialog->UpdateDelays(mLeft, range.topRow(), range.leftColumn(), range.bottomRow(), range.rightColumn());
    }
    else if (discountingED50Dialog->isVisible())
    {
        discountingED50Dialog->UpdateDelays(mLeft, range.topRow(), range.leftColumn(), range.bottomRow(), range.rightColumn());
    }
}

void SheetWidget::updateValueModalWindow()
{
    if (!isToolWindowShown())
    {
        return;
    }

    QList<QTableWidgetSelectionRange> mList = table->selectedRanges();
    QTableWidgetSelectionRange range = mList.first();

    QString mLeft = "";
    convertExcelColumn(mLeft, range.leftColumn());

    QString mRight = "";
    convertExcelColumn(mRight, range.rightColumn());

    mLeft.append(QString::number(range.topRow() + 1));
    mLeft.append(":");
    mLeft.append(mRight);
    mLeft.append(QString::number(range.bottomRow() + 1));

    if (discountingAreaDialog->isVisible())
    {
        discountingAreaDialog->UpdateValues(mLeft, range.topRow(), range.leftColumn(), range.bottomRow(), range.rightColumn());
    }
    else if (discountingED50Dialog->isVisible())
    {
        discountingED50Dialog->UpdateValues(mLeft, range.topRow(), range.leftColumn(), range.bottomRow(), range.rightColumn());
    }
}

void SheetWidget::updateMaxValueModalWindow()
{
    if (!isToolWindowShown())
    {
        return;
    }

    if (table->currentItem() != NULL)
    {
        if (discountingAreaDialog->isVisible())
        {
            discountingAreaDialog->UpdateMaxValue(table->currentItem()->data(Qt::DisplayRole).toString());
        }
        else if (discountingED50Dialog->isVisible())
        {
            discountingED50Dialog->UpdateMaxValue(table->currentItem()->data(Qt::DisplayRole).toString());
        }
    }   
}

bool SheetWidget::isToolWindowShown()
{
    if (discountingAreaDialog->isVisible())
    {
        return true;
    }
    else if (discountingED50Dialog->isVisible())
    {
        return true;
    }

    return false;
}

/** Scoring methods
 * @brief
 */

void SheetWidget::Calculate(QString scriptName,
                            int topDelay, int leftDelay, int bottomDelay, int rightDelay,
                            int topValue, int leftValue, int bottomValue, int rightValue,
                            double maxValue,
                            bool cbRachlin,
                            bool modelExponential, bool modelHyperbolic, bool modelQuasiHyperbolic, bool modelMyersonGreen, bool modelRachlin,
                            bool johnsonBickelTest, bool showCharts, bool logNormalParameters)
{

    tripLogNormal = logNormalParameters;

    if (discountingAreaDialog->isVisible())
    {
        discountingAreaDialog->ToggleButton(false);
    }
    else if (discountingED50Dialog->isVisible())
    {
        discountingED50Dialog->ToggleButton(false);
    }

    displayFigures = showCharts;

    bool isRowData = (rightDelay - leftDelay == 0) ? false : true;
    int nSeries = (isRowData) ? bottomValue - topValue + 1 : nSeries = rightValue - leftValue + 1;

    int dWidth = rightDelay - leftDelay + 1;
    int dLength = bottomDelay - topDelay + 1;

    int vWidth = rightValue - leftValue + 1;
    int vLength = bottomValue - topValue + 1;

    if (!areDimensionsValid(isRowData, dWidth, vWidth, dLength, vLength))
    {
        if (discountingAreaDialog->isVisible())
        {
            discountingAreaDialog->ToggleButton(true);
        }
        else if (discountingED50Dialog->isVisible())
        {
            discountingED50Dialog->ToggleButton(true);
        }

        return;
    }

    QStringList delayPoints;

    if(!areDelayPointsValid(delayPoints, isRowData, topDelay, leftDelay, bottomDelay, rightDelay))
    {
        if (discountingAreaDialog->isVisible())
        {
            discountingAreaDialog->ToggleButton(true);
        }
        else if (discountingED50Dialog->isVisible())
        {
            discountingED50Dialog->ToggleButton(true);
        }

        return;
    }

    QStringList valuePoints;
    QStringList delayPointsTemp;

    int mSeriesScoring = 0;

    mJohnsonBickelResults.clear();

    if (johnsonBickelTest)
    {
        checkDialog = new SystematicChekDialog(this);
        double prev, curr;
        bool criteriaOne, criteriaTwo;
        QString criteriaOneStr, criteriaTwoStr;

        for (int i = 0; i < nSeries; i++)
        {
            criteriaOne = true;
            criteriaTwo = true;

            valuePoints.clear();
            delayPointsTemp.clear();

            areValuePointsValid(valuePoints, delayPointsTemp, delayPoints, isRowData, topValue, leftValue, bottomValue, rightValue, i, maxValue);

            for (int i=1; i<valuePoints.length(); i++)
            {
                prev = valuePoints[i-1].toDouble();
                curr = valuePoints[i].toDouble();

                if ((curr - prev) > 0.2)
                {
                    criteriaOne = false;
                }
            }

            prev = valuePoints[0].toDouble();
            curr = valuePoints[valuePoints.count() - 1].toDouble();

            if ((prev - curr) < 0.1)
            {
                criteriaTwo = false;
            }

            criteriaOneStr = (criteriaOne) ? "Pass" : "Fail";
            criteriaTwoStr = (criteriaTwo) ? "Pass" : "Fail";

            mJohnsonBickelResults.append(QPair<QString, QString>(criteriaOneStr, criteriaTwoStr));

            checkDialog->appendRow(QString::number(i + 1), criteriaOneStr, criteriaTwoStr);
        }

        checkDialog->exec();
        mSeriesScoring = checkDialog->getIndexOption();

        if (!checkDialog->canProceed)
        {
            if (discountingAreaDialog->isVisible())
            {
                discountingAreaDialog->ToggleButton(true);
            }
            else if (discountingED50Dialog->isVisible())
            {
                discountingED50Dialog->ToggleButton(true);
            }

            return;
        }
    }

    resultsDialog = new ResultsDialog(this);
    resultsDialog->setModal(false);

    QDir runDirectory = QDir(QCoreApplication::applicationDirPath());

    statusBar()->showMessage("Beginning calculations...", 3000);
    allResults.clear();

    for (int i = 0; i < nSeries; i++)
    {
        QStringList resultsList;
        resultsList << QString::number(i+1);


        if (mSeriesScoring == 1)
        {
            if (mJohnsonBickelResults[i].first.contains("Fail", Qt::CaseInsensitive) || mJohnsonBickelResults[i].second.contains("Fail", Qt::CaseInsensitive))
            {
                resultsList[0] = resultsList[0] + " (Dropped)";

                allResults.append(resultsList);

                continue;
            }
        }
        else if (mSeriesScoring == 2)
        {
            if (!checkDialog->mJonhsonBickelSelections[i])
            {
                resultsList[0] = resultsList[0] + " (Dropped)";

                allResults.append(resultsList);

                continue;
            }
        }

        statusBar()->showMessage("Calculating #" + QString::number(i + 1) + " of " + QString::number(nSeries), 3000);

        valuePoints.clear();
        delayPointsTemp.clear();

        areValuePointsValid(valuePoints, delayPointsTemp, delayPoints, isRowData, topValue, leftValue, bottomValue, rightValue, i, maxValue);

        mXString = "[";

        for (int i=0; i<delayPointsTemp.length(); i++)
        {
            if (i == 0)
            {
                mXString.append("[" + delayPointsTemp[i] + "]");
            }
            else
            {
                mXString.append(",[" + delayPointsTemp[i] + "]");
            }
        }

        mXString.append("]");

        mYString = "[";

        for (int i=0; i<valuePoints.length(); i++)
        {
            if (i == 0)
            {
                mYString.append(valuePoints[i]);
            }
            else
            {
                mYString.append("," + valuePoints[i]);
            }
        }

        mYString.append("]");

        mObj->SetX(mXString.toUtf8().constData());
        mObj->SetY(mYString.toUtf8().constData());
        mObj->mBicList.clear();


        if (modelHyperbolic)
        {
            mObj->FitHyperbolic("[0.3]");

            if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                mObj->mBicList.append(QPair<QString, double>("Hyperbolic", mObj->bicHyperbolic));
            }

            resultsList << formatStringResult(mObj->fitHyperbolicK, tripLogNormal);
            resultsList << QString::number(mObj->GetReport().rmserror);
            resultsList << QString::number(mObj->bicHyperbolic);
            resultsList << QString::number(mObj->aicHyperbolic);
            resultsList << "";
            resultsList << "";
            resultsList << mObj->formatStringResult((int) mObj->GetInfo());
        }
        else
        {
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
        }

        if (modelExponential)
        {
            mObj->FitExponential("[0.3]");

            if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                mObj->mBicList.append(QPair<QString, double>("Exponential", mObj->bicExponential));
            }

            resultsList << formatStringResult(mObj->fitExponentialK, tripLogNormal);
            resultsList << QString::number(mObj->GetReport().rmserror);
            resultsList << QString::number(mObj->bicExponential);
            resultsList << QString::number(mObj->aicExponential);
            resultsList << "";
            resultsList << "";
            resultsList << mObj->formatStringResult((int) mObj->GetInfo());
        }
        else
        {
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
        }

        if (modelQuasiHyperbolic)
        {
            mObj->FitQuasiHyperbolic("[0.3, 0.3]");

            if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                mObj->mBicList.append(QPair<QString, double>("Beta Delta", mObj->bicQuasiHyperbolic));
            }

            resultsList << QString::number(mObj->fitQuasiHyperbolicBeta);
            resultsList << QString::number(mObj->fitQuasiHyperbolicDelta);
            resultsList << QString::number(mObj->GetReport().rmserror);
            resultsList << QString::number(mObj->bicQuasiHyperbolic);
            resultsList << QString::number(mObj->aicQuasiHyperbolic);
            resultsList << "";
            resultsList << "";
            resultsList << mObj->formatStringResult((int) mObj->GetInfo());
        }
        else
        {
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
        }

        if (modelMyersonGreen)
        {
            mObj->FitMyerson("[0.3, 0.3]");

            if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
            {
                mObj->mBicList.append(QPair<QString, double>("Myerson Hyperbola", mObj->bicMyerson));
            }

            resultsList << formatStringResult(mObj->fitMyersonK, tripLogNormal);
            resultsList << QString::number(mObj->fitMyersonS);
            resultsList << QString::number(mObj->GetReport().rmserror);
            resultsList << QString::number(mObj->bicMyerson);
            resultsList << QString::number(mObj->aicMyerson);
            resultsList << "";
            resultsList << "";
            resultsList << mObj->formatStringResult((int) mObj->GetInfo());
        }
        else
        {
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
        }

        if (modelRachlin)
        {
            mObj->FitRachlin("[0.3, 0.3]");

            if (cbRachlin && mObj->GetParams()[1] > 1)
            {
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
            }
            else
            {
                if ((int) mObj->GetInfo() == 2 || (int) mObj->GetInfo() == 5)
                {
                    mObj->mBicList.append(QPair<QString, double>("Rachlin Hyperbola", mObj->bicRachlin));
                }

                resultsList << formatStringResult(mObj->fitRachlinK, tripLogNormal);
                resultsList << QString::number(mObj->fitRachlinS);
                resultsList << QString::number(mObj->GetReport().rmserror);
                resultsList << QString::number(mObj->bicRachlin);
                resultsList << QString::number(mObj->aicRachlin);
                resultsList << "";
                resultsList << "";
                resultsList << mObj->formatStringResult((int) mObj->GetInfo());
            }
        }
        else
        {
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
        }

        mObj->FitNoise();
        mObj->NoiseBIC = mObj->bicNoise;

        resultsList << QString::number(mObj->GetNoiseMean());
        resultsList << QString::number(mObj->GetReport().rmserror);
        resultsList << QString::number(mObj->bicNoise);
        resultsList << QString::number(mObj->aicNoise);
        resultsList << "";
        resultsList << "";

        mObj->PrepareProbabilities();

        qSort(mObj->mProbList.begin(), mObj->mProbList.end(), QPairSecondComparer());

        if (modelHyperbolic)
        {
            resultsList[5] = QString::number(mObj->bfHyperbolic);
            resultsList[6] = QString::number(mObj->probsHyperbolic);
        }

        if (modelExponential)
        {
            resultsList[12] = QString::number(mObj->bfExponential);
            resultsList[13] = QString::number(mObj->probsExponential);
        }

        if (modelQuasiHyperbolic)
        {
            resultsList[20] = QString::number(mObj->bfQuasiHyperbolic);
            resultsList[21] = QString::number(mObj->probsQuasiHyperbolic);
        }

        if (modelMyersonGreen)
        {
            resultsList[28] = QString::number(mObj->bfMyerson);
            resultsList[29] = QString::number(mObj->probsMyerson);
        }

        if (modelRachlin)
        {
            resultsList[36] = QString::number(mObj->bfRachlin);
            resultsList[37] = QString::number(mObj->probsRachlin);
        }

        resultsList[43] = QString::number(mObj->bfNoise);
        resultsList[44] = QString::number(mObj->probsNoise);

        QString model = mObj->mProbList.first().first;

        resultsList << model;

        resultsList << delayPointsTemp.join(",");
        resultsList << valuePoints.join(",");

        if (scriptName.contains("Area", Qt::CaseInsensitive))
        {
            resultsList << mObj->getAUCBestModel(model);
        }
        else
        {
            resultsList << mObj->getED50BestModel(model);
        }

        allResults.append(resultsList);
    }

    if (displayFigures)
    {
        statusBar()->showMessage("Drawing figures...", 3000);

        if (discountingAreaDialog->isVisible())
        {
            graphicsWindow = new ChartWindow(allResults, true, this);

        }
        else if (discountingED50Dialog->isVisible())
        {
            graphicsWindow = new ChartWindow(allResults, false, this);
        }

        graphicsWindow->show();
    }

    if (discountingAreaDialog->isVisible())
    {
        discountingAreaDialog->ToggleButton(true);
        discountingAreaDialog->setEnabled(true);
        resultsDialog->ImportDataAndShow(tripLogNormal, "AUC.mostprob");
    }
    else if (discountingED50Dialog->isVisible())
    {
        discountingED50Dialog->ToggleButton(true);
        discountingED50Dialog->setEnabled(true);
        resultsDialog->ImportDataAndShow(tripLogNormal, "lnED50.mostprob");
    }
}

bool SheetWidget::areDelayPointsValid(QStringList &delayPoints, bool isRowData, int topDelay, int leftDelay, int bottomDelay, int rightDelay)
{
    delayPoints.clear();

    QString holder;
    bool valueCheck = true;

    if (isRowData)
    {
        int r = topDelay;

        for (int c = leftDelay; c <= rightDelay; c++)
        {
            if (table->item(r, c) == NULL)
            {
                QMessageBox::critical(this, "Error",
                                      "One of your delay measures doesn't look correct. Please re-check these values or selections.");

                if (discountingAreaDialog->isVisible())
                {
                    discountingAreaDialog->ToggleButton(true);
                }
                else if (discountingED50Dialog->isVisible())
                {
                    discountingED50Dialog->ToggleButton(true);
                }

                return false;
            }

            holder = table->item(r, c)->data(Qt::DisplayRole).toString();
            holder.toDouble(&valueCheck);

            delayPoints << holder;

            if (!valueCheck)
            {
                QMessageBox::critical(this, "Error",
                                      "One of your delay measures doesn't look correct. Please re-check these values or selections.");

                if (discountingAreaDialog->isVisible())
                {
                    discountingAreaDialog->ToggleButton(true);
                }
                else if (discountingED50Dialog->isVisible())
                {
                    discountingED50Dialog->ToggleButton(true);
                }

                return false;
            }
        }
    }
    else
    {
        int c = leftDelay;

        for (int r = topDelay; r <= bottomDelay; r++)
        {
            if (table->item(r, c) == NULL)
            {
                QMessageBox::critical(this, "Error",
                                      "One of your delay measures doesn't look correct. Please re-check these values or selections.");

                if (discountingAreaDialog->isVisible())
                {
                    discountingAreaDialog->ToggleButton(true);
                }
                else if (discountingED50Dialog->isVisible())
                {
                    discountingED50Dialog->ToggleButton(true);
                }

                return false;
            }

            holder = table->item(r, c)->data(Qt::DisplayRole).toString();
            holder.toDouble(&valueCheck);

            delayPoints << holder;

            if (!valueCheck)
            {
                QMessageBox::critical(this, "Error",
                                      "One of your delay measures doesn't look correct. Please re-check these values or selections.");

                if (discountingAreaDialog->isVisible())
                {
                    discountingAreaDialog->ToggleButton(true);
                }
                else if (discountingED50Dialog->isVisible())
                {
                    discountingED50Dialog->ToggleButton(true);
                }

                return false;
            }
        }
    }

    return true;
}

bool SheetWidget::areDimensionsValid(bool isRowData, int dWidth, int vWidth, int dLength, int vLength)
{
    if (isRowData)
    {
        if (dWidth != vWidth)
        {
            QMessageBox::critical(this, "Error",
                                  "You have row-based data, but the data selected appears to have different column counts. Please correct.");

            if (discountingAreaDialog->isVisible())
            {
                discountingAreaDialog->ToggleButton(true);
            }
            else if (discountingED50Dialog->isVisible())
            {
                discountingED50Dialog->ToggleButton(true);
            }

            return false;
        }
    }
    else
    {
        if (dLength != vLength)
        {
            QMessageBox::critical(this, "Error",
                                  "You have column-based data, but the data selected appears to have different row counts. Please correct.");

            if (discountingAreaDialog->isVisible())
            {
                discountingAreaDialog->ToggleButton(true);
            }
            else if (discountingED50Dialog->isVisible())
            {
                discountingED50Dialog->ToggleButton(true);
            }

            return false;
        }
    }

    return true;
}

void SheetWidget::areValuePointsValid(QStringList &valuePoints, QStringList &tempDelayPoints, QStringList delayPoints, bool isRowData, int topValue, int leftValue, int bottomValue, int rightValue, int i, double maxValue)
{
    valuePoints.clear();
    tempDelayPoints.clear();

    QString holder;
    bool valueCheck = true;
    double valHolder;

    int index = 0;

    if (isRowData)
    {
        int r = topValue;

        for (int c = leftValue; c <= rightValue; c++)
        {
            if (table->item(r + i, c) != NULL)
            {
                holder = table->item(r + i, c)->data(Qt::DisplayRole).toString();
                valHolder = holder.toDouble(&valueCheck);

                if (valueCheck)
                {
                    valHolder = valHolder / maxValue;

                    valuePoints << QString::number(valHolder);
                    tempDelayPoints << delayPoints.at(index);
                }
            }

            index++;
        }
    }
    else
    {
        int c = leftValue;

        for (int r = topValue; r <= bottomValue; r++)
        {
            if (table->item(r, c + i) != NULL)
            {
                holder = table->item(r, c + i)->data(Qt::DisplayRole).toString();
                valHolder = holder.toDouble(&valueCheck);

                if (valueCheck)
                {
                    valHolder = valHolder / maxValue;

                    valuePoints << QString::number(valHolder);
                    tempDelayPoints << delayPoints.at(index);
                }
            }

            index++;
        }
    }
}

/** Utilities
 * @brief
 */

QString SheetWidget::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

QString SheetWidget::formatStringResult(double value, bool returnLogNormal)
{
    if (!isnan(value))
    {
        if (value == 0)
        {
            return QString("NA");
        }
        else if (returnLogNormal)
        {
            qreal res = qExp(value);
            return QString::number(res);
        }
        else
        {
            return QString::number(value);
        }
    }
    else
    {
        return QString("NA");
    }
}

void SheetWidget::convertExcelColumn(QString &mString, int column)
{
    int dividend = column + 1;
    QString columnName = "";

    int modulo;

    while (dividend > 0)
    {
        modulo = (dividend - 1) % 26;
        columnName = new QString(65 + modulo) + columnName;
        dividend = (int)((dividend - modulo) / 26);
    }

    mString = columnName;
}

