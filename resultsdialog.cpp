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

ResultsDialog::ResultsDialog(int nSeries, bool tripLogNormal, QWidget *parent) :
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

    columnList << "Bleichrodt.k";
    columnList << "Bleichrodt.s";
    columnList << "Bleichrodt.beta";
    columnList << "Bleichrodt.RMSE";
    columnList << "Bleichrodt.BIC";
    columnList << "Bleichrodt.AIC";
    columnList << "Bleichrodt.BF";
    columnList << "Bleichrodt.prob";
    columnList << "Bleichrodt.notes";

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

    ui->tableWidget->setRowCount(nSeries);
    ui->tableWidget->setColumnCount(columnList.count());

    for(int i=0; i<columnList.count();i++)
    {
        ui->tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(columnList.at(i)));
    }
}

void ResultsDialog::ImportDataAndShow(bool hasAreaMetrics)
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(parent());

    for(int i=0; i<temp->allResults.count(); i++)
    {
        FitResults mResults = temp->allResults.at(i);

        QTableWidgetItem *item = new QTableWidgetItem(mResults.Header);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i, 0, item);

        for (int j=0; j<mResults.FittingResults.length(); j++)
        {
            if (mResults.FittingResults.at(j)->Model == ModelType::Hyperbolic)
            {
                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i, 1, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i, 2, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i, 3, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i, 4, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i, 5, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i, 6, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i, 7, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::Exponential)
            {
                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i, 8, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i, 9, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,10, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,11, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,12, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,13, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,14, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::BetaDelta)
            {
                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,15, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,16, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,17, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,18, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,19, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,20, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,21, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,22, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::Myerson)
            {
                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,23, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,24, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,25, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,26, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,27, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,28, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,29, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,30, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::Rachlin)
            {
                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,31, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,32, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,33, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,34, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,35, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,36, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,37, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,38, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::RodriguezLogue)
            {
                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,39, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,40, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,41, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,42, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,43, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,44, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,45, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,46, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::EbertPrelec)
            {
                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,47, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,48, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,49, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,50, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,51, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,52, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,53, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,54, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::Beleichrodt)
            {
                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,55, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params[1].second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,56, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.last().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,57, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,58, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,59, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,60, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,61, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,62, item);

                item = new QTableWidgetItem(mResults.FittingResults.at(j)->Status);
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,63, item);
            }

            if (mResults.FittingResults.at(j)->Model == ModelType::Noise)
            {
                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Params.first().second));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,64, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->RMS));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,65, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,66, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->AIC));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,67, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->BF));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,68, item);

                item = new QTableWidgetItem(formatNumberCatchNulls(mResults.FittingResults.at(j)->Probability));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui->tableWidget->setItem(i,69, item);
            }
        }

        item = new QTableWidgetItem(mResults.TopModel);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i,70, item);

        item = new QTableWidgetItem(mResults.TopED50);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i,71, item);

        if (hasAreaMetrics)
        {
            item = new QTableWidgetItem(mResults.TopAUC);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget->setItem(i,72, item);

            item = new QTableWidgetItem(mResults.TopAUCLog);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget->setItem(i,73, item);
        }

        item = new QTableWidgetItem(mResults.ParticipantDelays);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i,74, item);

        item = new QTableWidgetItem(mResults.ParticipantValues);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i,75, item);
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

QString ResultsDialog::formatNumberCatchNulls(double value)
{
    return (value == NULL) ? QString("---") : QString::number(value);
}
