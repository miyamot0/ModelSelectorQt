#ifndef SYSTEMATICCHEKDIALOG_H
#define SYSTEMATICCHEKDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>

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

    bool canProceed = false;
    bool flagRaised = false;

    void appendRow(QString participant, QString criteriaOne, QString criteriaTwo);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::SystematicChekDialog *ui;
};

#endif // SYSTEMATICCHEKDIALOG_H
