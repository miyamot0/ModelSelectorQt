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

#ifndef CALCULATIONWORKER_H
#define CALCULATIONWORKER_H

#include <QObject>

#include "Modeling/modelselection.h"
#include "Models/calculationsettings.h"
#include "Models/fitresults.h"
#include "Models/fitresult.h"

#include "Modeling/gridsearch.h"

#include "Utilities/tags.h"

/**
 * @brief The QPairSecondComparer struct
 */
struct QPairSecondComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> &one, const QPair<T1,T2> &two) const
    {
        return one.second > two.second;
    }
};

class CalculationWorker : public QObject
{
    Q_OBJECT

public:
    CalculationWorker(QList<QPair<QString, QString>> mJohnsonBickelResults,
                      QList<bool> *mJonhsonBickelSelections,
                      QList<QStringList> mStoredValues,
                      CalculationSettings* calculationSettings,
                      int processChecking);

private:

    // TODO: new models need to have lognormal options

    QList<QPair<QString, QString>> mLocalJohnsonBickelResults;
    QList<bool> *mLocalJonhsonBickelSelections;
    QList<QStringList> mLocalStoredValues;
    ModelSelection *mFittingObject;

    FitResults *fitResults;

    FitResult *fitResultExponential,
              *fitResultParabolic,
              *fitResultHyperbolic,
              *fitResultBetaDelta,
              *fitResultGreenMyerson,
              *fitResultRachlin,
              *fitResultGeneralizedHyperbolic,
              *fitResultEbertPrelec,
              *fitResultBleichrodt,
              *fitResultNoise;

    int processCheckingLocal, grandLoop;

    double p1Span,
           p1Step,
           p2Span,
           p2Step,
           p3Span,
           p3Step;

    CalculationSettings *settings;

    BruteForceValues provisionalValues;

    bool BruteSorter(BruteForce const& lhs, BruteForce const& rhs) {
        return lhs.err < rhs.err;
    }

    QString formatStringResult(double value, bool returnLogNormal)
    {
        if (!isnan(value))
        {
            if (value == 0)
            {
                return QString("NA");
            }
            else if (returnLogNormal)
            {
                qreal res = exp(value);
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

    unsigned int popSize = 100;

    QStringList tempList;

    QList<double> UnwrapArray(QString array)
    {
        QList<double> holder;

        QString preString1 = array.replace('[',' ');
        QString preString2 = preString1.replace(']',' ');

        QStringList elements = preString2.split(',', QString::SkipEmptyParts);

        bool isValid;

        double tempResult;

        for (int i = 0; i < elements.length(); i++)
        {
            tempResult = elements[i].toDouble(&isValid);

            if (isValid)
            {
                holder.append(tempResult);
            }
        }

        return holder;
    }

signals:
    void workStarted();
    void workingResult(const FitResults &value);
    void workFinished();

public slots:
    void startWork();
    void working();
};

#endif // CALCULATIONWORKER_H
