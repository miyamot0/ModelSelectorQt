#include "discountingmodelselectioned50dialog.h"
#include "ui_discountingmodelselectioned50dialog.h"
#include "sheetwidget.h"
#include <QMessageBox>
#include <QDesktopWidget>

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

void DiscountingModelSelectionED50Dialog::on_pushButton_clicked()
{
    QString maxValueString = ui->lineEditMaxValue->text();

    bool isDouble;
    double maxValue = maxValueString.toDouble(&isDouble);

    if (!isDouble)
    {
        QMessageBox::critical(this, "Error",
                              "You will need to enter a valid number for the maximum value of the commodity (i.e., A).");

        return;
    }

    SheetWidget *temp = qobject_cast <SheetWidget *>(parent());
    temp->Calculate("DiscountingED50Computation.R", topDelay, leftDelay, bottomDelay, rightDelay, topValue, leftValue, bottomValue, rightValue, maxValue,
                    ui->checkBoxBIC->isChecked(), ui->checkBoxAIC->isChecked(), ui->checkBoxRMSE->isChecked(),
                    ui->checkBoxBF->isChecked(), ui->checkBoxRachlin->isChecked(),
                    ui->checkBoxExponential->isChecked(), ui->checkBoxHyperbolic->isChecked(), ui->checkBoxQuasiHyperbolic->isChecked(),
                    ui->checkBoxMyersonHyperboloid->isChecked(), ui->checkBoxRachlinHyperboloid->isChecked(),
                    ui->displayPlotFigures->isChecked());
}
