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

    item = new QTableWidgetItem(participant);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, item);

    item = new QTableWidgetItem(criteriaOne);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    if (!item->text().contains("Pass", Qt::CaseInsensitive))
    {
        flagRaised = true;
        isFirstFlagged = true;
        item->setBackgroundColor(QColor(Qt::red));
        item->setTextColor(QColor(Qt::white));
    }
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, item);

    item = new QTableWidgetItem(criteriaTwo);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    if (!item->text().contains("Pass", Qt::CaseInsensitive))
    {
        flagRaised = true;
        isSecondFlagged = true;
        item->setBackgroundColor(QColor(Qt::red));
        item->setTextColor(QColor(Qt::white));
    }
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, item);

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
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 3, item);
}

SystematicChekDialog::~SystematicChekDialog()
{
    delete ui;
}

void SystematicChekDialog::on_buttonBox_accepted()
{
    canProceed = true;
}

void SystematicChekDialog::on_buttonBox_rejected()
{
    canProceed = false;
}
