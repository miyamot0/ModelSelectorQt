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

  */

#include <QtXlsx>
#include <QDir>
#include <QFileDialog>
#include <QDesktopWidget>

#include "resultsdialog.h"
#include "ui_resultsdialog.h"
#include "sheetwidget.h"

ResultsDialog::ResultsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsDialog)
{
    ui->setupUi(this);

    this->window()->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            window()->size(),
            qApp->desktop()->availableGeometry()
        )
    );

    copyAction = new QAction("Copy", this);
    copyAction->setShortcut(QKeySequence("Ctrl+C"));
    copyAction->setIcon(QIcon(":/images/edit-copy.png"));
    connect(copyAction, &QAction::triggered, this, &ResultsDialog::copy);

    addAction(copyAction);

    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
}

void ResultsDialog::ImportDataAndShow(bool tripLogNormal, QString metric)
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(parent());

    QList<int> skipList;

    QStringList columnList;

    // 0
    columnList << "Series";

    if (tripLogNormal)
    {
        columnList << "Mazur.k";
    }
    else
    {
        columnList << "Mazur.lnk";
    }

    columnList << "Mazur.RMSE";
    columnList << "Mazur.BIC";
    columnList << "Mazur.AIC";
    columnList << "Mazur.BF";
    columnList << "Mazur.prob";
    columnList << "Mazur.notes";

    // 10
    if (tripLogNormal)
    {
        columnList << "exp.k";
    }
    else
    {
        columnList << "exp.lnk";
    }

    columnList << "exp.RMSE";
    columnList << "exp.BIC";
    columnList << "exp.AIC";
    columnList << "exp.BF";
    columnList << "exp.prob";
    columnList << "exp.notes";

    //19
    columnList << "BD.beta";
    columnList << "BD.delta";
    columnList << "BD.RMSE";
    columnList << "BD.BIC";
    columnList << "BD.AIC";
    columnList << "BD.BF";
    columnList << "BD.prob";
    columnList << "BD.notes";

    // 27
    if (tripLogNormal)
    {
        columnList << "MG.k";
    }
    else
    {
        columnList << "MG.lnk";
    }

    columnList << "MG.s";
    columnList << "MG.RMSE";
    columnList << "MG.BIC";
    columnList << "MG.AIC";
    columnList << "MG.BF";
    columnList << "MG.prob";
    columnList << "MG.notes";

    // 35
    if (tripLogNormal)
    {
        columnList << "Rachlin.k";
    }
    else
    {
        columnList << "Rachlin.lnk";
    }

    columnList << "Rachlin.s";
    columnList << "Rachlin.RMSE";
    columnList << "Rachlin.BIC";
    columnList << "Rachlin.AIC";
    columnList << "Rachlin.BF";
    columnList << "Rachlin.prob";
    columnList << "Rachlin.notes";

    columnList << "RodriguezLogue.k";
    columnList << "RodriguezLogue.beta";
    columnList << "RodriguezLogue.RMSE";
    columnList << "RodriguezLogue.BIC";
    columnList << "RodriguezLogue.AIC";
    columnList << "RodriguezLogue.BF";
    columnList << "RodriguezLogue.prob";
    columnList << "RodriguezLogue.notes";

    columnList << "EbertPrelec.k";
    columnList << "EbertPrelec.s";
    columnList << "EbertPrelec.RMSE";
    columnList << "EbertPrelec.BIC";
    columnList << "EbertPrelec.AIC";
    columnList << "EbertPrelec.BF";
    columnList << "EbertPrelec.prob";
    columnList << "EbertPrelec.notes";

    columnList << "noise.mean";
    columnList << "noise.RMSE";
    columnList << "noise.BIC";
    columnList << "noise.AIC";
    columnList << "noise.BF";
    columnList << "noise.prob";

    columnList << "probmodel";
    columnList << "probmodel.ED50";
    columnList << "probmodel.AUC";
    columnList << "probmodel.Log10AUC";
    columnList << "delays";
    columnList << "values";

    // Create columns
    for(int i=0; i<columnList.count();i++)
    {
        if (!skipList.contains(i))
        {
            ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
            ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem(columnList.at(i)));
        }
    }

    int spacer;

    for(int i=0; i<temp->allResults.count(); i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        FitResults mResults = temp->allResults.at(i);

        QTableWidgetItem *item = new QTableWidgetItem(mResults.Header);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, item);

        for (int j=0; j<mResults.FittingResults.length(); j++)
        {
            if (mResults.FittingResults.at(j)->Model == ModelType::Hyperbolic)
            {
                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 3, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 4, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 5, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 6, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 7, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::Exponential)
            {
                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 8, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 9, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,10, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,11, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,12, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,13, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,14, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::BetaDelta)
            {
                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,15, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,16, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,17, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,18, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,19, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,20, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,21, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,22, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::Myerson)
            {
                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,23, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,24, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,25, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,26, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,27, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,28, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,29, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,30, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::Rachlin)
            {
                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,31, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,32, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,33, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,34, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,35, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,36, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,37, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,38, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::RodriguezLogue)
            {
                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,39, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,40, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,41, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,42, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,43, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,44, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,45, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,46, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::EbertPrelec)
            {
                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,47, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,48, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,49, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,50, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,51, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,52, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,53, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,54, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::Noise)
            {
                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,55, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,56, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,57, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,58, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,59, item);

                item = new QTableWidgetItem(QString::number(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,60, item);
            }
        }

        item = new QTableWidgetItem(mResults.TopModel);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,61, item);

        item = new QTableWidgetItem(mResults.TopED50);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,62, item);

        item = new QTableWidgetItem(mResults.TopAUC);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,63, item);

        item = new QTableWidgetItem(mResults.TopAUCLog);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,64, item);

        item = new QTableWidgetItem(mResults.ParticipantDelays);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,65, item);

        item = new QTableWidgetItem(mResults.ParticipantValues);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,66, item);
    }

    ui->tableWidget->viewport()->update();

    show();
}

ResultsDialog::~ResultsDialog()
{
    delete ui;
}

void ResultsDialog::copy()
{
    QList<QTableWidgetSelectionRange> range = ui->tableWidget->selectedRanges();
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

            if (ui->tableWidget->item(mRange.topRow() + i, mRange.leftColumn() + j) == NULL)
            {
                str += "";
            }
            else
            {
                str += ui->tableWidget->item(mRange.topRow() + i, mRange.leftColumn() + j)->data(Qt::DisplayRole).toString();
            }
        }
    }

    QApplication::clipboard()->setText(str);
}

void ResultsDialog::on_pushButton_2_clicked()
{
    QString selFilter="Spreadsheet (*.xlsx)";
    QString file_name = QFileDialog::getSaveFileName(this, "Save file", QDir::homePath(), "Spreadsheet (*.xlsx)", &selFilter);

    if(!file_name.trimmed().isEmpty())
    {
        QXlsx::Document xlsx;

        int rows = ui->tableWidget->rowCount();
        int cols = ui->tableWidget->columnCount();

        QString temp;
        QTableWidgetItem *col;

        for (int i=0; i<rows; i++)
        {
            for (int j=0; j<cols; j++)
            {
                if (i == 0)
                {
                    col = ui->tableWidget->horizontalHeaderItem(j);
                    temp = col->text();
                    xlsx.write(1, j + 1, temp);
                }

                QTableWidgetItem *item = ui->tableWidget->item(i, j);

                if (item != NULL && !item->text().isEmpty())
                {
                    temp = ui->tableWidget->item(i, j)->data(Qt::DisplayRole).toString();
                    xlsx.write(i + 2, j + 1, temp);
                }

            }
        }

        xlsx.saveAs(file_name);
    }
}

void ResultsDialog::convertExcelColumn(QString &mString, int column)
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
