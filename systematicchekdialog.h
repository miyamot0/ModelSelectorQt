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

#ifndef SYSTEMATICCHEKDIALOG_H
#define SYSTEMATICCHEKDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStandardItem>
#include <QCheckBox>
#include <QHBoxLayout>

namespace Ui {
class SystematicChekDialog;
}

class SystematicChekDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SystematicChekDialog(QWidget *parent = 0);
    ~SystematicChekDialog();
    QTableWidgetItem *item;
    QWidget *pWidget;
    QCheckBox *pCheckBox;
    QHBoxLayout *pLayout;
    QList<bool> mJonhsonBickelSelections;

    bool canProceed = false;
    bool flagRaised = false;

    void appendRow(QString participant, QString criteriaOne, QString criteriaTwo);
    int getIndexOption();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::SystematicChekDialog *ui;
};

#endif // SYSTEMATICCHEKDIALOG_H
