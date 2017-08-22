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

#ifndef SHEETTOOLS_H
#define SHEETTOOLS_H

#include <QtWidgets>

#include "discountingmodelselectioned50dialog.h"

class SheetTools {
public:

    QString static SheetTools::strippedName(const QString &fullFileName)
    {
        return QFileInfo(fullFileName).fileName();
    }

    QString static SheetTools::formatStringResult(double value, bool returnLogNormal)
    {
        if (!isnan(value))
        {
            if (value == 0)
            {
                return QString("NA");
            }
            else if (returnLogNormal)
            {
                qreal res = qExp(value);
                return QString::number(res);
            }
            else
            {
                return QString::number(value);
            }
        }
        else
        {
            return QString("NA");
        }
    }

    bool static SheetTools::areDimensionsValid(bool isRowData, int dWidth, int vWidth, int dLength, int vLength, DiscountingModelSelectionED50Dialog *discountingED50Dialog)
    {
        if (isRowData)
        {
            if (dWidth != vWidth)
            {
                QMessageBox::critical(NULL, "Error",
                                      "You have row-based data, but the data selected appears to have different column counts. Please correct.");

                if (discountingED50Dialog->isVisible())
                {
                    discountingED50Dialog->ToggleButton(true);
                }

                return false;
            }
        }
        else
        {
            if (dLength != vLength)
            {
                QMessageBox::critical(NULL, "Error",
                                      "You have column-based data, but the data selected appears to have different row counts. Please correct.");

                if (discountingED50Dialog->isVisible())
                {
                    discountingED50Dialog->ToggleButton(true);
                }

                return false;
            }
        }

        return true;
    }

    bool static SheetTools::areDelayPointsValid(QStringList &delayPoints, bool isRowData, int topDelay, int leftDelay, int bottomDelay, int rightDelay,
                                    DiscountingModelSelectionED50Dialog *discountingED50Dialog, QTableWidget *table)
    {
        delayPoints.clear();

        QString holder;
        bool valueCheck = true;

        if (isRowData)
        {
            int r = topDelay;

            for (int c = leftDelay; c <= rightDelay; c++)
            {
                if (table->item(r, c) == NULL)
                {
                    QMessageBox::critical(NULL, "Error",
                                          "One of your delay measures doesn't look correct. Please re-check these values or selections.");

                    if (discountingED50Dialog->isVisible())
                    {
                        discountingED50Dialog->ToggleButton(true);
                    }

                    return false;
                }

                holder = table->item(r, c)->data(Qt::DisplayRole).toString();
                holder.toDouble(&valueCheck);

                delayPoints << holder;

                if (!valueCheck)
                {
                    QMessageBox::critical(NULL, "Error",
                                          "One of your delay measures doesn't look correct. Please re-check these values or selections.");

                    if (discountingED50Dialog->isVisible())
                    {
                        discountingED50Dialog->ToggleButton(true);
                    }

                    return false;
                }
            }
        }
        else
        {
            int c = leftDelay;

            for (int r = topDelay; r <= bottomDelay; r++)
            {
                if (table->item(r, c) == NULL)
                {
                    QMessageBox::critical(NULL, "Error",
                                          "One of your delay measures doesn't look correct. Please re-check these values or selections.");

                    if (discountingED50Dialog->isVisible())
                    {
                        discountingED50Dialog->ToggleButton(true);
                    }

                    return false;
                }

                holder = table->item(r, c)->data(Qt::DisplayRole).toString();
                holder.toDouble(&valueCheck);

                delayPoints << holder;

                if (!valueCheck)
                {
                    QMessageBox::critical(NULL, "Error",
                                          "One of your delay measures doesn't look correct. Please re-check these values or selections.");

                    if (discountingED50Dialog->isVisible())
                    {
                        discountingED50Dialog->ToggleButton(true);
                    }

                    return false;
                }
            }
        }

        return true;
    }

    void static SheetTools::areValuePointsValid(QStringList &valuePoints, QStringList &tempDelayPoints, QStringList delayPoints,
                                                bool isRowData, int topValue, int leftValue, int bottomValue, int rightValue, int i,
                                                double maxValue, QTableWidget *table)
    {
        valuePoints.clear();
        tempDelayPoints.clear();

        QString holder;
        bool valueCheck = true;
        double valHolder;

        int index = 0;

        if (isRowData)
        {
            int r = topValue;

            for (int c = leftValue; c <= rightValue; c++)
            {
                if (table->item(r + i, c) != NULL)
                {
                    holder = table->item(r + i, c)->data(Qt::DisplayRole).toString();
                    valHolder = holder.toDouble(&valueCheck);

                    if (valueCheck)
                    {
                        valHolder = valHolder / maxValue;

                        valuePoints << QString::number(valHolder);
                        tempDelayPoints << delayPoints.at(index);
                    }
                }

                index++;
            }
        }
        else
        {
            int c = leftValue;

            for (int r = topValue; r <= bottomValue; r++)
            {
                if (table->item(r, c + i) != NULL)
                {
                    holder = table->item(r, c + i)->data(Qt::DisplayRole).toString();
                    valHolder = holder.toDouble(&valueCheck);

                    if (valueCheck)
                    {
                        valHolder = valHolder / maxValue;

                        valuePoints << QString::number(valHolder);
                        tempDelayPoints << delayPoints.at(index);
                    }
                }

                index++;
            }
        }
    }

    void static SheetTools::convertExcelColumn(QString &mString, int column)
    {
        int dividend = column + 1;
        QString columnName = "";

        int modulo;

        while (dividend > 0)
        {
            modulo = (dividend - 1) % 26;
            columnName = new QString(65 + modulo) + columnName;
            dividend = (int)((dividend - modulo) / 26);
        }

        mString = columnName;
    }


};

#endif // SHEETTOOLS_H
