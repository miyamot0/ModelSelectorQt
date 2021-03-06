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

#ifndef RESULTSDIALOG_H
#define RESULTSDIALOG_H

#include <QTableWidget>
#include <QDialog>

#include "Models/fitresults.h"

namespace Ui {
class ResultsDialog;
}

class ResultsDialog : public QDialog
{
    Q_OBJECT

public:
    /** Widget that handles showing of results to user
     * @brief ResultsDialog
     * @param parent
     */
    explicit ResultsDialog(int nSeries, bool tripLogNormal, QWidget *parent = 0);

    /** Optional display parameters, as they are added
     * @brief ImportDataAndShow
     * @param cbBIC - add BIC
     * @param cbAIC - add AIC
     * @param cbRMSE - add RMSE
     * @param cbBF - add Bayes Factors
     */
    void ImportDataAndShow(bool hasAreaMetrics);

    /** Convert indices to Excel-friendly equivalents
     * @brief convertExcelColumn
     * @param mString
     * @param column
     */
    void convertExcelColumn(QString &mString, int column);
    QString formatNumberCatchNulls(double value);
    ~ResultsDialog();

public slots:
    void copy();

private slots:
    void on_pushButton_2_clicked();

private:
    Ui::ResultsDialog *ui;
    QAction *copyAction;
};

#endif // RESULTSDIALOG_H
