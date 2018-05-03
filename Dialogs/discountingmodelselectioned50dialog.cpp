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

#include <QMessageBox>
#include <QDesktopWidget>

#include "Dialogs/discountingmodelselectioned50dialog.h"
#include "ui_discountingmodelselectioned50dialog.h"

#include "Controls/sheetwidget.h"

DiscountingModelSelectionED50Dialog::DiscountingModelSelectionED50Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiscountingModelSelectionED50Dialog)
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

    this->setWindowFlags(Qt::Tool);
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);

    connect(ui->checkBoxRachlin, SIGNAL(clicked(bool)), this, SLOT(RachlinToggleButton(bool)));
}

DiscountingModelSelectionED50Dialog::~DiscountingModelSelectionED50Dialog()
{
    delete ui;
}

void DiscountingModelSelectionED50Dialog::UpdateDelays(QString label, int top, int left, int bottom, int right)
{
    ui->lineEditDelays->setText(label);

    topDelay = top;
    leftDelay = left;
    bottomDelay = bottom;
    rightDelay = right;
}

void DiscountingModelSelectionED50Dialog::UpdateValues(QString label, int top, int left, int bottom, int right)
{
    ui->lineEditValues->setText(label);

    topValue = top;
    leftValue = left;
    bottomValue = bottom;
    rightValue = right;
}

void DiscountingModelSelectionED50Dialog::UpdateMaxValue(QString label)
{
    ui->lineEditMaxValue->setText(label);
}

void DiscountingModelSelectionED50Dialog::ToggleButton(bool status)
{
    ui->pushButton->setEnabled(status);
}

FittingAlgorithm DiscountingModelSelectionED50Dialog::getFittingAlgorithm()
{
    if (ui->comboBoxFitting->currentIndex() == 0)
    {
        return FittingAlgorithm::Function;
    }
    else if (ui->comboBoxFitting->currentIndex() == 1)
    {
        return FittingAlgorithm::FunctionGradient;
    }
    else if (ui->comboBoxFitting->currentIndex() == 2)
    {
        return FittingAlgorithm::FunctionGradientHessian;
    }
    else
    {
        return FittingAlgorithm::FunctionGradientHessian;
    }
}

void DiscountingModelSelectionED50Dialog::RachlinToggleButton(bool status)
{
    if (status)
    {
        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, tr("Confirm Rachlin Bounding"),
                                      tr("Bounding the Rachlin model violates the assumptions necessary for approximate Bayesian model selection. If the Rachlin model's s parameter exceeds 1, it will be dropped as a candidate for that series. Do you wish to continue with this?"),
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            ui->checkBoxRachlin->setChecked(true);
        }
        else
        {
            ui->checkBoxRachlin->setChecked(false);
        }
    }
}

void DiscountingModelSelectionED50Dialog::on_pushButton_clicked()
{
    QString maxValueString = ui->lineEditMaxValue->text();

    bool isDouble;
    double maxValue = maxValueString.toDouble(&isDouble);

    if (!isDouble)
    {
        QMessageBox::critical(this, tr("Error"),
                              tr("You will need to enter a valid number for the maximum value of the commodity (i.e., A)."));

        return;
    }

    SheetWidget *temp = qobject_cast <SheetWidget *>(parent());

    temp->calculationSettings = new CalculationSettings();
    temp->calculationSettings->scriptName = "ED50";

    temp->calculationSettings->topDelay = topDelay;
    temp->calculationSettings->leftDelay = leftDelay;
    temp->calculationSettings->bottomDelay = bottomDelay;
    temp->calculationSettings->rightDelay = rightDelay;

    temp->calculationSettings->topValue = topValue;
    temp->calculationSettings->leftValue = leftValue;
    temp->calculationSettings->bottomValue = bottomValue;
    temp->calculationSettings->rightValue = rightValue;

    temp->calculationSettings->maxValue = maxValue;

    temp->calculationSettings->modelExponential = ui->checkBoxExponential->isChecked();
    temp->calculationSettings->modelHyperbolic = ui->checkBoxHyperbolic->isChecked();
    temp->calculationSettings->modelQuasiHyperbolic = ui->checkBoxQuasiHyperbolic->isChecked();
    temp->calculationSettings->modelMyersonGreen = ui->checkBoxMyersonHyperboloid->isChecked();
    temp->calculationSettings->modelRachlin = ui->checkBoxRachlinHyperboloid->isChecked();
    temp->calculationSettings->modelGeneralizedHyperbolic = ui->checkBoxGeneralizedHyper->isChecked();
    temp->calculationSettings->modelEbertPrelec = ui->checkBoxEbertPrelec->isChecked();
    temp->calculationSettings->modelBleichrodt = ui->checkBoxBleichrodt->isChecked();

    temp->calculationSettings->cbRachlin = ui->checkBoxRachlin->isChecked();
    temp->calculationSettings->johnsonBickelTest = ui->checkBoxJohnson->isChecked();
    temp->calculationSettings->cbBruteForce = ui->BruteForceMode->isChecked();

    if (ui->comboBoxFigures->currentIndex() == 0) {
        temp->calculationSettings->chartOption = ChartingOptions::None;
        temp->calculationSettings->showCharts = false;

    } else if (ui->comboBoxFigures->currentIndex() == 1) {
        temp->calculationSettings->chartOption = ChartingOptions::All;
        temp->calculationSettings->showCharts = true;
        ui->checkBoxArea->setChecked(true);

    }

    temp->calculationSettings->logNormalParameters = ui->checkBoxLog->isChecked();
    temp->calculationSettings->cbArea = ui->checkBoxArea->isChecked();
    temp->calculationSettings->settingsFitting = getFittingAlgorithm();

    temp->Calculate();
}
