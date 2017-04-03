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
