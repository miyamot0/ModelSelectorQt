#ifndef DISCOUNTINGMODELSELECTIONAREADIALOG_H
#define DISCOUNTINGMODELSELECTIONAREADIALOG_H

#include <QDialog>

namespace Ui {
class DiscountingModelSelectionAreaDialog;
}

class DiscountingModelSelectionAreaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DiscountingModelSelectionAreaDialog(QWidget *parent = 0);
    ~DiscountingModelSelectionAreaDialog();

public slots:
    /** Update coordinates for relevant spreadsheet data
     * @brief UpdateDelays
     * @param label
     * @param top
     * @param left
     * @param bottom
     * @param right
     */
    void UpdateDelays(QString label, int top, int left, int bottom, int right);

    /** Update coordinates for relevant spreadsheet data
     * @brief UpdateValues
     * @param label
     * @param top
     * @param left
     * @param bottom
     * @param right
     */
    void UpdateValues(QString label, int top, int left, int bottom, int right);

    /** Update value of data
     * @brief UpdateMaxValue
     * @param label
     */
    void UpdateMaxValue(QString label);

    /** Begin calculations, supplying commands to background worker
     * @brief ToggleButton
     * @param status
     */
    void ToggleButton(bool status);

    void RachlinToggleButton(bool status);

private slots:
    /** Slot, fired calculation button
     * @brief on_pushButton_clicked
     */
    void on_pushButton_clicked();

private:
    Ui::DiscountingModelSelectionAreaDialog *ui;

    int topDelay;
    int leftDelay;
    int bottomDelay;
    int rightDelay;

    int topValue;
    int leftValue;
    int bottomValue;
    int rightValue;

};

#endif // DISCOUNTINGMODELSELECTIONAREADIALOG_H
