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

#include "systematicchekdialog.h"
#include "ui_systematicchekdialog.h"

SystematicChekDialog::SystematicChekDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystematicChekDialog)
{
    ui->setupUi(this);

    canProceed = false;

    ui->tableWidget->clearContents();

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Include"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Participant"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Criteria #1"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Criteria #2"));

    ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem("Criteria #1/#2"));
}

void SystematicChekDialog::appendRow(QString participant, QString criteriaOne, QString criteriaTwo)
{
    bool isFirstFlagged = false;
    bool isSecondFlagged = false;

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());

    pWidget = new QWidget();
    pCheckBox = new QCheckBox();
    pCheckBox->setChecked(true);
    pLayout = new QHBoxLayout(pWidget);
    pLayout->addWidget(pCheckBox);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(0,0,0,0);
    pWidget->setLayout(pLayout);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, pWidget);

    item = new QTableWidgetItem(participant);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, item);

    item = new QTableWidgetItem(criteriaOne);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    if (!item->text().contains("Pass", Qt::CaseInsensitive))
    {
        flagRaised = true;
        isFirstFlagged = true;
        item->setBackgroundColor(QColor(Qt::red));
        item->setTextColor(QColor(Qt::white));
    }
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, item);

    item = new QTableWidgetItem(criteriaTwo);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    if (!item->text().contains("Pass", Qt::CaseInsensitive))
    {
        flagRaised = true;
        isSecondFlagged = true;
        item->setBackgroundColor(QColor(Qt::red));
        item->setTextColor(QColor(Qt::white));
    }
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 3, item);

    if (isFirstFlagged && isSecondFlagged)
    {
        item = new QTableWidgetItem("Both Failed");
        item->setBackgroundColor(QColor(Qt::red));
        item->setTextColor(QColor(Qt::white));
    }
    else if (isFirstFlagged || isSecondFlagged)
    {
        item = new QTableWidgetItem("One Failed");
        item->setBackgroundColor(QColor(Qt::yellow));
    }
    else
    {
        item = new QTableWidgetItem("Both Passed");
        item->setBackgroundColor(QColor(Qt::green));
    }

    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 4, item);
}

int SystematicChekDialog::getIndexOption()
{
    return ui->comboBox->currentIndex();
}

SystematicChekDialog::~SystematicChekDialog()
{
    delete ui;
}

void SystematicChekDialog::on_buttonBox_accepted()
{
    mJonhsonBickelSelections.clear();

    for (int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QWidget *mWidgetRef = ui->tableWidget->cellWidget(i, 0);
        QCheckBox *mCheckBoxRef = qobject_cast<QCheckBox*>(mWidgetRef->children().at(1));
        mJonhsonBickelSelections.append(mCheckBoxRef->isChecked());
    }

    canProceed = true;
}

void SystematicChekDialog::on_buttonBox_rejected()
{
    canProceed = false;
}
