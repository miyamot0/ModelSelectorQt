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

#ifndef MODELSELECTIONDIALOG_H
#define MODELSELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class ModelSelectionDialog;
}

class ModelSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelSelectionDialog(QWidget *parent = 0);
    ~ModelSelectionDialog();

public slots:
    void UpdateDelays(QString label, int top, int left, int bottom, int right);
    void UpdateValues(QString label, int top, int left, int bottom, int right);
    void UpdateMaxValue(QString label);
    void ToggleButton(bool status);

private slots:
    void on_pushButton_clicked();

private:
    Ui::ModelSelectionDialog *ui;
    int topDelay;
    int leftDelay;
    int bottomDelay;
    int rightDelay;

    int topValue;
    int leftValue;
    int bottomValue;
    int rightValue;
};

#endif // MODELSELECTIONDIALOG_H
