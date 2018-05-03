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

#include "calculationsettings.h"
#include "sheetwidget.h"
#include "resultsdialog.h"
#include "chartwindow.h"
#include "commanding.h"
#include "sheetdelegate.h"
#include "calculationworker.h"

QTXLSX_USE_NAMESPACE

SheetWidget::SheetWidget(QWidget *parent) : QMainWindow(parent)
{
    table = new QTableWidget(10001, 10001, this);
    table->setSizeAdjustPolicy(QTableWidget::AdjustToContents);

    undoStack = new QUndoStack(this);
    table->setItemDelegate(new SheetDelegate());

    #ifdef TARGET_OS_MAC
        table->setStyleSheet("QTableView {selection-background-color: #73E2A7; }");
    #endif

    QString value;

    for (int c = 0; c < 10000; ++c)
    {
        value = "";
        SheetTools::convertExcelColumn(value, c);
        table->setHorizontalHeaderItem(c, new QTableWidgetItem(value));
    }

    buildMenus();
    setCentralWidget(table);

    setWindowTitle(QString("Discounting Model Selector v" + QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD)));

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

    table->installEventFilter( this );

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadedFile(QNetworkReply*)) );

    #ifdef _WIN32
        manager->get(QNetworkRequest(QUrl("http://www.smallnstats.com/ModelSelectionRepository/Updates.xml")));
    #elif TARGET_OS_MAC
        manager->get(QNetworkRequest(QUrl("http://www.smallnstats.com/ModelSelectionRepositoryMac/Updates.xml")));
    #endif

}

/**
 * @brief SheetWidget::downloadedFile
 * @param reply
 */
void SheetWidget::downloadedFile(QNetworkReply *reply) {
    QByteArray data = reply->readAll();

    QDomDocument versionXML;

    if(!versionXML.setContent(data))
    {
        return;
    }

    QDomElement root = versionXML.documentElement();
    QDomElement mNode = root.namedItem("PackageUpdate").toElement();
    QDomElement mNode2 = mNode.namedItem("Version").toElement();

    QStringList mVersionList = mNode2.text().split('.');

    if (mVersionList.count() != 3)
    {
        return;
    }

    bool hasUpdate = false;

    QString mNetworkVersionString = QString("%1%2%3").arg(mVersionList[0]).arg(mVersionList[1]).arg(mVersionList[2]);

    QString mLocalVersionString = QString("%1%2%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD);

    if (mNetworkVersionString.toInt() > mLocalVersionString.toInt())
    {
        hasUpdate = true;
    }

    if (hasUpdate)
    {
        QMessageBox *msgBox = new QMessageBox;
        msgBox->setWindowTitle(tr("Updates"));
        msgBox->setText(tr("There is an update available!"));
        msgBox->setWindowModality(Qt::NonModal);
        msgBox->show();
    }
}

/**
 * @brief SheetWidget::buildMenus
 */
void SheetWidget::buildMenus()
{
    /** File actions
     * @brief
     */

    newSheetAction = new QAction(tr("N&ew"), this);
    newSheetAction->setShortcut(QKeySequence::New);
    newSheetAction->setIcon(QIcon(":/images/document-new.png"));
    connect(newSheetAction, &QAction::triggered, this, &SheetWidget::clearSheet);

    openSheetAction = new QAction(tr("I&mport"), this);
    openSheetAction->setShortcut(QKeySequence::Open);
    openSheetAction->setIcon(QIcon(":/images/document-open.png"));
    connect(openSheetAction, &QAction::triggered, this, &SheetWidget::showOpenFileDialog);

    saveSheetAction = new QAction(tr("S&ave"), this);
    saveSheetAction->setShortcut(QKeySequence::Save);
    saveSheetAction->setIcon(QIcon(":/images/document-save.png"));
    connect(saveSheetAction, &QAction::triggered, this, &SheetWidget::showSaveFileDialog);

    saveAsSheetAction = new QAction(tr("S&ave As"), this);
    saveAsSheetAction->setShortcut(QKeySequence::SaveAs);
    saveAsSheetAction->setIcon(QIcon(":/images/document-save-as.png"));
    connect(saveAsSheetAction, &QAction::triggered, this, &SheetWidget::showSaveAsFileDialog);

    updateProgramAction = new QAction(tr("C&heck Updates"), this);
    updateProgramAction->setIcon(QIcon(":/images/view-refresh.png"));
    connect(updateProgramAction, &QAction::triggered, this, &SheetWidget::checkUpdatesAction);

    exitSheetAction = new QAction(tr("E&xit"), this);
    exitSheetAction->setShortcut(QKeySequence::Quit);
    exitSheetAction->setIcon(QIcon(":/images/system-log-out.png"));
    connect(exitSheetAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    /** Window actions
     * @brief
     */

    openDiscountingED50Window = new QAction(tr("M&odel Selection"), this);
    openDiscountingED50Window->setIcon(QIcon(":/images/applications-system.png"));
    connect(openDiscountingED50Window, &QAction::triggered, this, &SheetWidget::showDiscountingED50Window);

    /** Edit actions
     * @brief
     */

    cutAction = new QAction(tr("Cut"), this);
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setIcon(QIcon(":/images/edit-cut.png"));
    connect(cutAction, &QAction::triggered, this, &SheetWidget::cut);

    copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setIcon(QIcon(":/images/edit-copy.png"));
    connect(copyAction, &QAction::triggered, this, &SheetWidget::copy);

    pasteAction = new QAction(tr("Paste"), this);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setIcon(QIcon(":/images/edit-paste.png"));
    connect(pasteAction, &QAction::triggered, this, &SheetWidget::paste);

    pasteInvertedAction = new QAction(tr("Paste Transposed"), this);
    pasteInvertedAction->setIcon(QIcon(":/images/edit-paste.png"));
    connect(pasteInvertedAction, &QAction::triggered, this, &SheetWidget::pasteInverted);

    clearAction = new QAction(tr("Clear"), this);
    clearAction->setShortcut(Qt::Key_Delete);
    clearAction->setIcon(QIcon(":/images/edit-clear.png"));
    connect(clearAction, &QAction::triggered, this, &SheetWidget::clear);

    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setIcon(QIcon(":/images/edit-undo.png"));

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setIcon(QIcon(":/images/edit-redo.png"));

    table->addAction(undoAction);
    table->addAction(redoAction);

    /** Window actions
     * @brief
     */

    discountingED50Dialog = new DiscountingModelSelectionED50Dialog(this);

    openLicenseDMS = new QAction(tr("DMS License (GPL-V3)"), this);
    openLicenseDMS->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseDMS, &QAction::triggered, this, &SheetWidget::showDMSLicenseWindow);

    openLicenseALGLIB = new QAction(tr("ALGLIB License (GPL-V3)"), this);
    openLicenseALGLIB->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseALGLIB, &QAction::triggered, this, &SheetWidget::showALGLIBLicenseWindow);

    openLicenseBDS = new QAction(tr("BDS License (GPL-V3)"), this);
    openLicenseBDS->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseBDS, &QAction::triggered, this, &SheetWidget::showBDSLicenseWindow);

    openLicenseQt = new QAction(tr("Qt License (GPL-V3)"), this);
    openLicenseQt->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseQt, &QAction::triggered, this, &SheetWidget::showQTLicenseWindow);

    openLicenseTango = new QAction(tr("Tango Icons License (GPL-V3)"), this);
    openLicenseTango->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseTango, &QAction::triggered, this, &SheetWidget::showTangoLicenseWindow);

    openLicenseQtXlsx = new QAction(tr("QtXlsx License (MIT)"), this);
    openLicenseQtXlsx->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openLicenseQtXlsx, &QAction::triggered, this, &SheetWidget::showQtXlsxLicenseWindow);

    openAbout = new QAction(tr("Credits"), this);
    openAbout->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openAbout, &QAction::triggered, this, &SheetWidget::showCreditsWindow);

    openFAQ = new QAction(tr("FAQ"), this);
    openFAQ->setIcon(QIcon(":/images/format-justify-center.png"));
    connect(openFAQ, &QAction::triggered, this, &SheetWidget::showFAQWindow);

    /** Window helper actions
     * @brief
     */

    delayAction = new QAction(tr("Set Delays"), this);
    delayAction->setIcon(QIcon(":/images/preferences-system.png"));
    connect(delayAction, &QAction::triggered, this, &SheetWidget::updateDelayModalWindow);

    valueAction = new QAction(tr("Set Values"), this);
    valueAction->setIcon(QIcon(":/images/preferences-system.png"));
    connect(valueAction, &QAction::triggered, this, &SheetWidget::updateValueModalWindow);

    maxValueAction = new QAction(tr("Set Max Value"), this);
    maxValueAction->setIcon(QIcon(":/images/preferences-system.png"));
    connect(maxValueAction, &QAction::triggered, this, &SheetWidget::updateMaxValueModalWindow);

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        recentFileActs[i]->setIcon(QIcon(":/images/format-justify-center.png"));
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

    /** Menu area below
     * @brief
     */

    QMenu *sheetOptionsMenu = menuBar()->addMenu(tr("&File"));
    sheetOptionsMenu->addAction(newSheetAction);
    sheetOptionsMenu->addAction(openSheetAction);
    sheetOptionsMenu->addAction(saveSheetAction);
    sheetOptionsMenu->addAction(saveAsSheetAction);

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
    sheetEditMenu->addAction(redoAction);
    sheetEditMenu->addAction(undoAction);
    sheetEditMenu->addSeparator();
    sheetEditMenu->addAction(clearAction);

    QMenu *sheetCalculationsMenu = menuBar()->addMenu(tr("&Discounting"));
    sheetCalculationsMenu->addAction(openDiscountingED50Window);

    QMenu *sheetLicensesMenu = menuBar()->addMenu(tr("&Licenses"));
    sheetLicensesMenu->addAction(openLicenseALGLIB);
    sheetLicensesMenu->addAction(openLicenseDMS);
    sheetLicensesMenu->addAction(openLicenseBDS);
    sheetLicensesMenu->addAction(openLicenseQt);
    sheetLicensesMenu->addAction(openLicenseTango);
    sheetLicensesMenu->addAction(openLicenseQtXlsx);
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

/**
 * @brief SheetWidget::checkUpdatesAction
 */
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
        QProcess *p = new QProcess();

        connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            QMessageBox::information(this, "Information", "Please close and re-open application if updated.", QMessageBox::Ok);
        });

        p->start(mCommand, QStringList() << "--updater");
    }
}

/**
 * @brief SheetWidget::clearSheet
 */
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

/**
 * @brief SheetWidget::closeEvent
 * @param event
 */
void SheetWidget::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton confirm = QMessageBox::question( this, tr("Discounting Model Selector"),
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

/**
 * @brief SheetWidget::openRecentFile
 */
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

                table->setUpdatesEnabled(false);
                table->blockSignals(true);

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

                table->setUpdatesEnabled(true);
                table->blockSignals(false);

                setCurrentFile(mFile);
                statusBar()->showMessage(tr("File loaded"), 2000);
            }
        }
    }
}

/**
 * @brief SheetWidget::setCurrentFile
 * @param fileName
 */
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

/**
 * @brief SheetWidget::saveSettings
 */
void SheetWidget::saveSettings()
{
    QSettings settings(QSettings::UserScope, QLatin1String("Discounting Model Selector"));
    settings.beginGroup(QLatin1String("SheetWindow"));

    QStringList files = settings.value("recentFileList").toStringList();

    settings.setValue(QLatin1String("recentFileList"), files);
    settings.endGroup();

    settings.sync();
}

/** Events
 * @brief
 */

/**
 * @brief SheetWidget::eventFilter
 * @param object
 * @param event
 * @return
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

#ifdef _WIN32
        else if (keyCode->key() == (int) Qt::Key_Space && keyCode->modifiers().testFlag(Qt::ControlModifier))
        {
            showTestingFeatures();
        }
#elif TARGET_OS_MAC
        else if (keyCode->key() == (int) Qt::Key_Space && keyCode->modifiers().testFlag(Qt::ControlModifier) && keyCode->modifiers().testFlag(Qt::ShiftModifier))
        {
            showTestingFeatures();
        }
#endif


    }

    return QObject::eventFilter(object, event);
}

/**
 * @brief SheetWidget::showOpenFileDialog
 */
void SheetWidget::showOpenFileDialog()
{
    QString file_name;
    QString fileFilter = "Spreadsheet (*.xlsx)";

    #ifdef _WIN32

    file_name = QFileDialog::getOpenFileName(this, tr("Open spreadsheet file"), QDir::homePath(),
                                             fileFilter);

    #elif TARGET_OS_MAC

    file_name = QFileDialog::getOpenFileName(nullptr, tr("Open spreadsheet file"), QDir::homePath(),
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

            table->setUpdatesEnabled(false);
            table->blockSignals(true);

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

                if (((w / xlsx2.dimension().lastColumn())*100) % 10 == 0)
                {
                    statusBar()->showMessage(QString::number(((w / xlsx2.dimension().lastColumn())*100)), 2000);
                }
            }

            table->setUpdatesEnabled(true);
            table->blockSignals(false);

            setCurrentFile(file_name);
            statusBar()->showMessage(tr("File loaded"), 2000);
        }

        QApplication::restoreOverrideCursor();
    }
}

/**
 * @brief SheetWidget::updateRecentFileActions
 */
void SheetWidget::updateRecentFileActions()
{
    QSettings settings(QSettings::UserScope, QLatin1String("Discounting Model Selector"));
    settings.beginGroup(QLatin1String("SheetWindow"));
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(SheetTools::strippedName(files[i]));
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

/**
 * @brief SheetWidget::showSaveFileDialog
 */
void SheetWidget::showSaveFileDialog()
{
    if (curFile == "")
    {
        showSaveAsFileDialog();

        return;
    }

    if(!curFile.trimmed().isEmpty())
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

        xlsx.saveAs(curFile);

        QApplication::restoreOverrideCursor();

        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

/**
 * @brief SheetWidget::showSaveAsFileDialog
 */
void SheetWidget::showSaveAsFileDialog()
{

    QString file_name;
    QString fileFilter = "Spreadsheet (*.xlsx)";

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, tr("Save spreadsheet file"), QDir::homePath(),
                                         fileFilter);

#elif TARGET_OS_MAC

        file_name = QFileDialog::getSaveFileName(this, tr("Save spreadsheet file"), QDir::homePath(),
                                         fileFilter, &fileFilter, QFileDialog::Option::DontUseNativeDialog);

        if (!file_name.contains(".xlsx"))
        {
            file_name.append(".xlsx");
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

/**
 * @brief SheetWidget::showDiscountingED50Window
 */
void SheetWidget::showDiscountingED50Window()
{
    if (isToolWindowShown())
    {
        discountingED50Dialog->showNormal();

        return;
    }

    discountingED50Dialog = new DiscountingModelSelectionED50Dialog(this);
    discountingED50Dialog->setModal(false);
    discountingED50Dialog->show();
}

/**
 * @brief SheetWidget::showDMSLicenseWindow
 */
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
    licenseDialog->setWindowTitle(tr("DMS License (GPL-V3)"));
    licenseDialog->setModal(true);
    licenseDialog->show();
}

/**
 * @brief SheetWidget::showALGLIBLicenseWindow
 */
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
    licenseDialog->setWindowTitle(tr("ALGLIB (GPL-V3)"));
    licenseDialog->setModal(true);
    licenseDialog->show();
}

/**
 * @brief SheetWidget::showBDSLicenseWindow
 */
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
    licenseDialog->setWindowTitle(tr("BDS License (GPL-V3)"));
    licenseDialog->setModal(true);
    licenseDialog->show();
}

/**
 * @brief SheetWidget::showQTLicenseWindow
 */
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
    licenseDialog->setWindowTitle(tr("Qt License (LGPL-V3)"));
    licenseDialog->setModal(true);
    licenseDialog->show();
}

/**
 * @brief SheetWidget::showTangoLicenseWindow
 */
void SheetWidget::showTangoLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_Tango.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_Tango.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle(tr("Tango Icon Set License (Public Domain)"));
    licenseDialog->setModal(true);
    licenseDialog->show();
}

/**
 * @brief SheetWidget::showQtXlsxLicenseWindow
 */
void SheetWidget::showQtXlsxLicenseWindow()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_QtXlsx.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_QtXlsx.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle(tr("QtXlsx License (MIT)"));
    licenseDialog->setModal(true);
    licenseDialog->show();
}

/**
 * @brief SheetWidget::showCreditsWindow
 */
void SheetWidget::showCreditsWindow()
{
    creditsDialog = new CreditsDialog();
    creditsDialog->setModal(true);
    creditsDialog->show();
}

/**
 * @brief SheetWidget::showFAQWindow
 */
void SheetWidget::showFAQWindow()
{
    aboutDialog = new AboutDialog();
    aboutDialog->setModal(true);
    aboutDialog->show();
}

/**
 * @brief SheetWidget::showTestingFeatures
 */
void SheetWidget::showTestingFeatures()
{
    if (!openDiscountingAreaWindow->isVisible())
    {
        statusBar()->showMessage(tr("Testing features now unlocked"), 5000);
        openDiscountingAreaWindow->setVisible(true);
    }
}

/** Edit methods
 * @brief
 */

/**
 * @brief SheetWidget::cut
 */
void SheetWidget::cut()
{
    SheetWidget::copy();
    SheetWidget::clear();
}

/**
 * @brief SheetWidget::copy
 */
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

/**
 * @brief SheetWidget::paste
 */
void SheetWidget::paste()
{
    QTableWidgetSelectionRange range = table->selectedRanges().first();
    QString pasteString = QApplication::clipboard()->text();

    QStringList pasteRows = pasteString.split('\n');

    int nRows = pasteRows.count();
    int nCols = pasteRows.first().count('\t') + 1;

    if (nRows < 1 || nCols < 1)
    {
        return;
    }
    else if (nRows == 1 && nCols == 1)
    {
        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());
        QString mOldTest(table->model()->index(range.topRow(), range.leftColumn()).data(Qt::EditRole).toString());
        QString mNewTest(pasteRows[0].split('\t')[0]);

        undoStack->push(new UpdateCommand(&index, mOldTest, mNewTest));
    }
    else
    {
        QStringList mOlderHolder;
        QStringList mTemp;

        for (int i = 0; i < nRows; ++i)
        {
            QStringList columns = pasteRows[i].split('\t');

            mTemp.clear();

            for (int j = 0; j < nCols; ++j) {
                int row = range.topRow() + i;
                int column = range.leftColumn() + j;

                if (row < 10000 && column < 10000)
                {
                    if (table->item(row, column) != NULL)
                    {
                        if (j < columns.length())
                        {
                            mTemp << table->item(row, column)->data(Qt::EditRole).toString();
                        }
                    }
                    else
                    {
                        if (j < columns.length())
                        {
                            table->setItem(row, column, new QTableWidgetItem(""));

                            mTemp << "";
                        }
                    }
                }
            }

            mOlderHolder << mTemp.join('\t');
        }

        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());

        undoStack->push(new UpdateCommandBlock(&index, mOlderHolder, pasteRows));
    }
}

/**
 * @brief SheetWidget::pasteInverted
 */
void SheetWidget::pasteInverted()
{
    QTableWidgetSelectionRange range = table->selectedRanges().first();
    QString pasteString = QApplication::clipboard()->text();
    QStringList pasteRows = pasteString.split('\n');

    int nRows = pasteRows.count();
    int nCols = pasteRows.first().count('\t') + 1;

    if (nRows < 1 || nCols < 1)
    {
        return;
    }
    else if (nRows == 1 && nCols == 1)
    {
        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());
        QString mOldTest(table->model()->index(range.topRow(), range.leftColumn()).data(Qt::EditRole).toString());
        QString mNewTest(pasteRows[0].split('\t')[0]);

        undoStack->push(new UpdateCommand(&index, mOldTest, mNewTest));
    }
    else
    {
        QStringList mOlderHolder;
        QStringList mTemp;

        for (int i = 0; i < nRows; ++i)
        {
            QStringList columns = pasteRows[i].split('\t');

            mTemp.clear();

            for (int j = 0; j < nCols; ++j)
            {
                int row = range.topRow() + j;
                int column = range.leftColumn() + i;

                if (row < 10000 && column < 10000)
                {
                    if (table->item(row, column) != NULL)
                    {
                        if (j < columns.length())
                        {
                            mTemp << table->item(row, column)->data(Qt::EditRole).toString();
                        }
                    }
                    else
                    {
                        if (j < columns.length())
                        {
                            table->setItem(row, column, new QTableWidgetItem(""));

                            mTemp << "";
                        }
                    }
                }
            }

            mOlderHolder << mTemp.join('\t');
        }

        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());

        undoStack->push(new UpdateCommandBlockInvert(&index, mOlderHolder, pasteRows));
    }
}

/**
 * @brief SheetWidget::clear
 */
void SheetWidget::clear()
{
    QTableWidgetSelectionRange range = table->selectedRanges().first();

    int nRows = range.rowCount();
    int nCols = range.columnCount();

    if (nRows < 1 || nCols < 1)
    {
        return;
    }
    else if (nRows == 1 && nCols == 1)
    {
        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());
        QString mOldTest(table->model()->index(range.topRow(), range.leftColumn()).data(Qt::EditRole).toString());
        QString clear("");

        undoStack->push(new UpdateCommand(&index, mOldTest, clear));
    }
    else
    {
        QStringList mNewerHolder;
        QStringList mOlderHolder;

        QStringList mTempNew;
        QStringList mTemp;

        for (int i = 0; i < nRows; ++i)
        {
            mTemp.clear();
            mTempNew.clear();

            for (int j = 0; j < nCols; ++j)
            {
                int row = range.topRow() + i;
                int column = range.leftColumn() + j;

                if (row < 10000 && column < 10000)
                {
                    if (table->item(row, column) != NULL)
                    {
                        mTemp << table->item(row, column)->data(Qt::EditRole).toString();
                        mTempNew << "";
                    }
                    else
                    {
                        table->setItem(row, column, new QTableWidgetItem(""));

                        mTemp << "";
                        mTempNew << "";
                    }
                }
            }

            mOlderHolder << mTemp.join('\t');
            mNewerHolder << mTempNew.join('\t');
        }

        const QModelIndex index = table->model()->index(range.topRow(), range.leftColumn(), QModelIndex());
        undoStack->push(new UpdateCommandBlock(&index, mOlderHolder, mNewerHolder));
    }
}

/** Window helper methods
 * @brief
 */

/**
 * @brief SheetWidget::updateDelayModalWindow
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
    SheetTools::convertExcelColumn(mLeft, range.leftColumn());

    QString mRight = "";
    SheetTools::convertExcelColumn(mRight, range.rightColumn());

    mLeft.append(QString::number(range.topRow() + 1));
    mLeft.append(":");
    mLeft.append(mRight);
    mLeft.append(QString::number(range.bottomRow() + 1));

    int mWidth = range.rightColumn() - range.leftColumn();
    int mHeight = range.bottomRow() - range.topRow();

    if (mWidth > 0 && mHeight > 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Please select a vector of delays (e.g., one column or one row)."));
        return;
    }
    else if (mWidth + mHeight < 3)
    {
        QMessageBox::critical(this, tr("Error"), tr("Please select at least 3 delay points. At least 3 points are needed to proceed with curve fitting."));
        return;
    }

    if (discountingED50Dialog->isVisible())
    {
        discountingED50Dialog->UpdateDelays(mLeft, range.topRow(), range.leftColumn(), range.bottomRow(), range.rightColumn());
    }
}

/**
 * @brief SheetWidget::updateValueModalWindow
 */
void SheetWidget::updateValueModalWindow()
{
    if (!isToolWindowShown())
    {
        return;
    }

    QList<QTableWidgetSelectionRange> mList = table->selectedRanges();
    QTableWidgetSelectionRange range = mList.first();

    QString mLeft = "";
    SheetTools::convertExcelColumn(mLeft, range.leftColumn());

    QString mRight = "";
    SheetTools::convertExcelColumn(mRight, range.rightColumn());

    mLeft.append(QString::number(range.topRow() + 1));
    mLeft.append(":");
    mLeft.append(mRight);
    mLeft.append(QString::number(range.bottomRow() + 1));

    if (discountingED50Dialog->isVisible())
    {
        discountingED50Dialog->UpdateValues(mLeft, range.topRow(), range.leftColumn(), range.bottomRow(), range.rightColumn());
    }
}

/**
 * @brief SheetWidget::updateMaxValueModalWindow
 */
void SheetWidget::updateMaxValueModalWindow()
{
    if (!isToolWindowShown())
    {
        return;
    }

    if (table->currentItem() != NULL)
    {
        if (discountingED50Dialog->isVisible())
        {
            discountingED50Dialog->UpdateMaxValue(table->currentItem()->data(Qt::DisplayRole).toString());
        }
    }   
}

/**
 * @brief SheetWidget::isToolWindowShown
 * @return
 */
bool SheetWidget::isToolWindowShown()
{
    if (discountingED50Dialog->isVisible())
    {
        return true;
    }

    return false;
}

/** Scoring methods
 * @brief
 */

/**
 * @brief SheetWidget::Calculate
 */
void SheetWidget::Calculate()
{
    tripLogNormal = calculationSettings->logNormalParameters;

    if (discountingED50Dialog->isVisible())
    {
        discountingED50Dialog->ToggleButton(false);
    }

    displayFigures = calculationSettings->showCharts;

    bool isRowData = (calculationSettings->rightDelay - calculationSettings->leftDelay == 0) ? false : true;
    int nSeries = (isRowData) ? calculationSettings->bottomValue - calculationSettings->topValue + 1 :
                                nSeries = calculationSettings->rightValue - calculationSettings->leftValue + 1;

    int dWidth = calculationSettings->rightDelay - calculationSettings->leftDelay + 1;
    int dLength = calculationSettings->bottomDelay - calculationSettings->topDelay + 1;

    int vWidth = calculationSettings->rightValue - calculationSettings->leftValue + 1;
    int vLength = calculationSettings->bottomValue - calculationSettings->topValue + 1;

    if (!SheetTools::areDimensionsValid(isRowData, dWidth, vWidth, dLength, vLength, discountingED50Dialog))
    {
        if (discountingED50Dialog->isVisible())
        {
            discountingED50Dialog->ToggleButton(true);
        }

        return;
    }

    QStringList delayPoints;

    if(!SheetTools::areDelayPointsValid(delayPoints, isRowData,
                            calculationSettings->topDelay, calculationSettings->leftDelay,
                            calculationSettings->bottomDelay, calculationSettings->rightDelay, discountingED50Dialog, table))
    {
        if (discountingED50Dialog->isVisible())
        {
            discountingED50Dialog->ToggleButton(true);
        }

        return;
    }

    QStringList valuePoints;
    QStringList delayPointsTemp;

    int mSeriesScoring = 0;

    mJohnsonBickelResults.clear();

    if (calculationSettings->johnsonBickelTest)
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

            SheetTools::areValuePointsValid(valuePoints, delayPointsTemp, delayPoints, isRowData,
                                calculationSettings->topValue, calculationSettings->leftValue, calculationSettings->bottomValue, calculationSettings->rightValue,
                                i, calculationSettings->maxValue, table);

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
            if (discountingED50Dialog->isVisible())
            {
                discountingED50Dialog->ToggleButton(true);
            }

            return;
        }
    }

    resultsDialog = new ResultsDialog(nSeries, tripLogNormal, this);
    resultsDialog->setModal(false);

    QDir runDirectory = QDir(QCoreApplication::applicationDirPath());

    statusBar()->showMessage(tr("Beginning calculations..."), 3000);
    allResults.clear();

    QList<QStringList> mStoredValues;
    QStringList mStoredValueHolder;

    for (int i = 0; i < nSeries; i++)
    {
        valuePoints.clear();
        delayPointsTemp.clear();

        SheetTools::areValuePointsValid(valuePoints, delayPointsTemp, delayPoints, isRowData,
                            calculationSettings->topValue, calculationSettings->leftValue, calculationSettings->bottomValue, calculationSettings->rightValue,
                            i, calculationSettings->maxValue, table);

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

        mStoredValueHolder.clear();
        mStoredValueHolder << mXString << mYString << delayPointsTemp.join(",") << valuePoints.join(",");

        mStoredValues << mStoredValueHolder;
    }

    workerThread = new QThread();

    worker = new CalculationWorker(mJohnsonBickelResults, &checkDialog->mJonhsonBickelSelections, mStoredValues,
                                   calculationSettings, mSeriesScoring);

    worker->moveToThread(workerThread);

    connect(worker, SIGNAL(workStarted()), workerThread, SLOT(start()));
    connect(workerThread, SIGNAL(started()), worker, SLOT(working()));
    connect(worker, SIGNAL(workingResult(FitResults)), this, SLOT(WorkUpdate(FitResults)));
    connect(worker, SIGNAL(workFinished()), workerThread, SLOT(quit()), Qt::DirectConnection);
    connect(worker, SIGNAL(workFinished()), this, SLOT(WorkFinished()));

    allResults.clear();

    workerThread->wait();
    worker->startWork();
}

/** Threading Tools
 *
 */

/**
 * @brief SheetWidget::WorkUpdate
 * @param results
 */
void SheetWidget::WorkUpdate(FitResults results)
{
    allResults.append(results);

    if (calculationSettings->cbBruteForce)
    {
        statusBar()->showMessage(QString("Series #%1 Computed... Brute Force mode will take time").arg(allResults.count()), 10000);
    }
    else
    {
        statusBar()->showMessage(QString("Series #%1 Computed").arg(allResults.count()), 3000);
    }
}

/**
 * @brief SheetWidget::WorkFinished
 */
void SheetWidget::WorkFinished()
{
    statusBar()->showMessage(tr("Calculations Complete."), 3000);

    if (displayFigures)
    {
        statusBar()->showMessage(tr("Drawing figures..."), 3000);

        if (discountingED50Dialog->isVisible())
        {
            graphicsWindow = new ChartWindow(allResults, tripLogNormal, calculationSettings->chartOption, this);
        }

        graphicsWindow->show();
    }

    if (discountingED50Dialog->isVisible())
    {
        discountingED50Dialog->ToggleButton(true);
        discountingED50Dialog->setEnabled(true);

        resultsDialog->ImportDataAndShow(calculationSettings->cbArea);
    }
}
