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
#include "modelselection.h"
#include "calculationsettings.h"
#include "fitresults.h"
#include "fitresult.h"

class CalculationWorker : public QObject
{
    Q_OBJECT

public:
    CalculationWorker(QList<QPair<QString, QString>> mJohnsonBickelResults, QList<bool> *mJonhsonBickelSelections, QList<QStringList> mStoredValues, CalculationSettings* calculationSettings, int processChecking);

private:
    QString computationTypeLocal;

    QString formatStringResult(double value, bool returnLogNormal);

    QList<QPair<QString, QString>> mLocalJohnsonBickelResults;
    QList<bool> *mLocalJonhsonBickelSelections;
    QList<QStringList> mLocalStoredValues;
    ModelSelection *mFittingObject;

    FitResults *fitResults;

    FitResult *fitResultExponential,
              *fitResultHyperbolic,
              *fitResultBetaDelta,
              *fitResultGreenMyerson,
              *fitResultRachlin,
              *fitResultRodriguezLogue,
              *fitResultEbertPrelec,
              *fitResultNoise;

    bool runLocalHyperbolic,
         runLocalExponential,
         runLocalBetaDelta,
         runLocalMyersonGreen,
         runLocalRachlin,
         runLocalRodriguezLogue,
         runLocalEbertPrelec,
         runLocalBleicholdt;

    bool runLogarithmicResults;
    bool boundRachlinModel;
    int processCheckingLocal;

signals:
    void workStarted();
    void workingResult(const FitResults &value);
    void workFinished();

public slots:
    void startWork();
    void working();
};

#endif // CALCULATIONWORKER_H
