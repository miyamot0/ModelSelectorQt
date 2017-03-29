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

#include <QDialog>

namespace Ui {
class ResultsDialog;
}

class ResultsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsDialog(QWidget *parent = 0);
    void ImportDataAndShow(bool cbBIC, bool cbAIC, bool cbRMSE, bool cbBF);
    void convertExcelColumn(QString &mString, int column);
    ~ResultsDialog();

private slots:
    void on_pushButton_2_clicked();

private:
    Ui::ResultsDialog *ui;

};

#endif // RESULTSDIALOG_H
