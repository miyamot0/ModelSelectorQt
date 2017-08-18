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

#include "calculationworker.h"
#include "qstringlist.h"
#include <QtWidgets>

#include <QDebug>

struct QPairSecondComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> &one, const QPair<T1,T2> &two) const
    {
        return one.second > two.second;
    }
};

struct BruteForce {
  double p1 = 0;
  double p2 = 0;
  double p3 = 0;

  double err = 0;

  bool operator < (const BruteForce& r1) const {
      return (err < r1.err);
  }
};

struct BruteForceValues {
    BruteForce startingValueArray[1000];
};

BruteForceValues provisionalValues;

bool BruteSorter(BruteForce const& lhs, BruteForce const& rhs) {
    return lhs.err < rhs.err;
}

CalculationWorker::CalculationWorker(QList<QPair<QString, QString> > mJohnsonBickelResults, QList<bool> *mJonhsonBickelSelections, QList<QStringList> mStoredValues, CalculationSettings *calculationSettings, int processChecking)
{
    computationTypeLocal = calculationSettings->scriptName;

    mLocalJohnsonBickelResults = mJohnsonBickelResults;
    mLocalJonhsonBickelSelections = mJonhsonBickelSelections;
    mLocalStoredValues = mStoredValues;

    mFittingObject = new ModelSelection();

    runLocalExponential = calculationSettings->modelExponential;

    runLocalHyperbolic =  calculationSettings->modelHyperbolic;
    runLocalBetaDelta = calculationSettings->modelQuasiHyperbolic;
    runLocalMyersonGreen = calculationSettings->modelMyersonGreen;
    runLocalRachlin = calculationSettings->modelRachlin;
    runLocalRodriguezLogue = calculationSettings->modelRodriguezLogue;
    runLocalEbertPrelec = calculationSettings->modelEbertPrelec;
    runLocalBleicholdt = calculationSettings->modelBleichrodt;

    runLogarithmicResults = calculationSettings->logNormalParameters;

    boundRachlinModel = calculationSettings->cbRachlin;

    runLocalArea = calculationSettings->cbArea;

    processCheckingLocal = processChecking;
}

void CalculationWorker::startWork()
{
    emit workStarted();
}

void CalculationWorker::working()
{
    fitResults = new FitResults();
    QStringList tempList;

    for (int i = 0; i < mLocalStoredValues.count(); i++)
    {
        fitResults->Header = QString::number(i+1);

        if (processCheckingLocal == 1)
        {
            if (mLocalJohnsonBickelResults[i].first.contains("Fail", Qt::CaseInsensitive) || mLocalJohnsonBickelResults[i].second.contains("Fail", Qt::CaseInsensitive))
            {
                fitResults->Header = QString::number(i+1) + " (Dropped)";

                continue;
            }
        }
        else if (processCheckingLocal == 2)
        {
            if (!mLocalJonhsonBickelSelections->at(i))
            {
                fitResults->Header = QString::number(i+1) + " (Dropped)";

                continue;
            }
        }

        tempList = mLocalStoredValues[i];

        mFittingObject->SetX(tempList[0].toUtf8().constData());
        mFittingObject->SetY(tempList[1].toUtf8().constData());
        mFittingObject->mBicList.clear();

        if (runLocalExponential)
        {
            // Running brute mode
            if (true)
            {
                p1Span = abs(-12) + abs(12); // -12 to 12
                p1Step = p1Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 100; kLoop++)
                {
                    provisionalValues.startingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);

                    grandLoop++;
                }

                for(BruteForce & obj : provisionalValues.startingValueArray)
                {
                    obj.err = mFittingObject->getErrorExponential(obj.p1);
                }

                std::sort(provisionalValues.startingValueArray, provisionalValues.startingValueArray + 100);

                mFittingObject->FitExponential(QString("[%1]").arg(provisionalValues.startingValueArray[0].p1).toUtf8().constData());
            }
            else
            {
                mFittingObject->FitExponential("[-5]");
            }

            //mFittingObject->FitExponential("[-5]");

            fitResultExponential = new FitResult(ModelType::Exponential);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Exponential, mFittingObject->bicExponential));

                double lnK = (runLogarithmicResults) ? exp(mFittingObject->fitExponentialK) : mFittingObject->fitExponentialK;
                fitResultExponential->Params.append(QPair<QString, double>(QString("Exponential K"), lnK));
                fitResultExponential->RMS = mFittingObject->GetReport().rmserror;
                fitResultExponential->AIC = mFittingObject->aicExponential;
                fitResultExponential->BIC = mFittingObject->bicExponential;
                fitResultExponential->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
            else
            {
                fitResultExponential->Params.append(QPair<QString, double>(QString("Exponential K"), NULL));
                fitResultExponential->RMS = NULL;
                fitResultExponential->AIC = NULL;
                fitResultExponential->BIC = NULL;
                fitResultExponential->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
        }

        if (runLocalHyperbolic)
        {
            if (true)
            {
                p1Span = abs(-12) + abs(12); // -12 to 12
                p1Step = p1Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 100; kLoop++)
                {
                    provisionalValues.startingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);

                    grandLoop++;
                }

                for(BruteForce & obj : provisionalValues.startingValueArray)
                {
                    obj.err = mFittingObject->getErrorHyperbolic(obj.p1);
                }

                std::sort(provisionalValues.startingValueArray, provisionalValues.startingValueArray + 100);

                mFittingObject->FitHyperbolic(QString("[%1]").arg(provisionalValues.startingValueArray[0].p1).toUtf8().constData());
            }
            else
            {
                mFittingObject->FitHyperbolic("[-5]");
            }

            //mFittingObject->FitHyperbolic("[-5]");

            fitResultHyperbolic = new FitResult(ModelType::Hyperbolic);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Hyperbolic, mFittingObject->bicHyperbolic));

                double lnK = (runLogarithmicResults) ? exp(mFittingObject->fitHyperbolicK) : mFittingObject->fitHyperbolicK;

                fitResultHyperbolic->Params.append(QPair<QString, double>(QString("Hyperbolic K"), lnK));
                fitResultHyperbolic->RMS = mFittingObject->GetReport().rmserror;
                fitResultHyperbolic->AIC = mFittingObject->aicHyperbolic;
                fitResultHyperbolic->BIC = mFittingObject->bicHyperbolic;
                fitResultHyperbolic->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
            else
            {
                fitResultHyperbolic->Params.append(QPair<QString, double>(QString("Hyperbolic K"), NULL));
                fitResultHyperbolic->RMS = NULL;
                fitResultHyperbolic->AIC = NULL;
                fitResultHyperbolic->BIC = NULL;
                fitResultHyperbolic->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
        }

        if (runLocalBetaDelta)
        {
            if (true)
            {
                p1Span = 1; // 0 to 1
                p1Step = p1Span / 10;

                p2Span = 1;
                p2Step = p2Span / 10;

                grandLoop = 0;

                for (int bLoop = 0; bLoop < 10; bLoop++)
                {
                    for (int dLoop = 0; dLoop < 10; dLoop++)
                    {
                        provisionalValues.startingValueArray[grandLoop].p1 = ((bLoop + 1) * p1Step);
                        provisionalValues.startingValueArray[grandLoop].p2 = ((dLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.startingValueArray)
                {
                    obj.err = mFittingObject->getErrorQuasiHyperbolic(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.startingValueArray, provisionalValues.startingValueArray + 100);

                mFittingObject->FitQuasiHyperbolic(QString("[%1,%2]")
                                                   .arg(provisionalValues.startingValueArray[0].p1)
                                                   .arg(provisionalValues.startingValueArray[0].p2)
                                                   .toUtf8().constData());
            }
            else
            {
                mFittingObject->FitQuasiHyperbolic("[0.3, 0.3]");
            }

            //mFittingObject->FitQuasiHyperbolic("[0.3, 0.3]");

            fitResultBetaDelta = new FitResult(ModelType::BetaDelta);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::BetaDelta, mFittingObject->bicQuasiHyperbolic));

                fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Beta"), mFittingObject->fitQuasiHyperbolicBeta));
                fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Delta"), mFittingObject->fitQuasiHyperbolicDelta));
                fitResultBetaDelta->RMS = mFittingObject->GetReport().rmserror;
                fitResultBetaDelta->AIC = mFittingObject->aicQuasiHyperbolic;
                fitResultBetaDelta->BIC = mFittingObject->bicQuasiHyperbolic;
                fitResultBetaDelta->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
            else
            {
                fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Beta"), NULL));
                fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Delta"), NULL));
                fitResultBetaDelta->RMS = NULL;
                fitResultBetaDelta->AIC = NULL;
                fitResultBetaDelta->BIC = NULL;
                fitResultBetaDelta->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
        }

        if (runLocalMyersonGreen)
        {
            if (true)
            {
                p1Span = abs(-12) + abs(12); // -12 to 12
                p1Step = p1Span / 10;

                p2Span = 10; // .1 to 10
                p2Step = p2Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 10; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 100; sLoop++)
                    {
                        provisionalValues.startingValueArray[grandLoop].p1 = ((kLoop + 1) * p1Step);
                        provisionalValues.startingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.startingValueArray)
                {
                    obj.err = mFittingObject->getErrorGreenMyerson(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.startingValueArray, provisionalValues.startingValueArray + 1000);

                mFittingObject->FitMyerson(QString("[%1,%2]")
                                                   .arg(provisionalValues.startingValueArray[0].p1)
                                                   .arg(provisionalValues.startingValueArray[0].p2)
                                                   .toUtf8().constData());
            }
            else
            {
                mFittingObject->FitMyerson("[-5, 0.3]");
            }

            //mFittingObject->FitMyerson("[-5, 0.3]");

            fitResultGreenMyerson = new FitResult(ModelType::Myerson);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Myerson, mFittingObject->bicMyerson));

                fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson K"), mFittingObject->fitMyersonK));
                fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson S"), mFittingObject->fitMyersonS));
                fitResultGreenMyerson->RMS = mFittingObject->GetReport().rmserror;
                fitResultGreenMyerson->AIC = mFittingObject->aicMyerson;
                fitResultGreenMyerson->BIC = mFittingObject->bicMyerson;
                fitResultGreenMyerson->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
            else
            {
                fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson K"), NULL));
                fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson S"), NULL));
                fitResultGreenMyerson->RMS = NULL;
                fitResultGreenMyerson->AIC = NULL;
                fitResultGreenMyerson->BIC = NULL;
                fitResultGreenMyerson->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
        }

        if (runLocalRachlin)
        {
            if (true)
            {
                p1Span = abs(-12) + abs(12); // -12 to 12
                p1Step = p1Span / 10;

                p2Span = 10; // .1 to 10
                p2Step = p2Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 10; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 100; sLoop++)
                    {
                        provisionalValues.startingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);
                        provisionalValues.startingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.startingValueArray)
                {
                    obj.err = mFittingObject->getErrorGreenRachlin(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.startingValueArray, provisionalValues.startingValueArray + 1000);

                mFittingObject->FitRachlin(QString("[%1,%2]")
                                                   .arg(provisionalValues.startingValueArray[0].p1)
                                                   .arg(provisionalValues.startingValueArray[0].p2)
                                                   .toUtf8().constData());
            }
            else
            {
                mFittingObject->FitRachlin("[-5, 0.3]");
            }

            //mFittingObject->FitRachlin("[-5, 0.3]");

            fitResultRachlin = new FitResult(ModelType::Rachlin);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                if (boundRachlinModel && mFittingObject->GetParams()[1] > 1)
                {
                    fitResultRachlin = new FitResult(ModelType::Rachlin);

                    fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin K"), NULL));
                    fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin S"), NULL));
                    fitResultRachlin->RMS = NULL;
                    fitResultRachlin->AIC = NULL;
                    fitResultRachlin->BIC = NULL;
                    fitResultRachlin->BF = NULL;
                    fitResultRachlin->Probability = NULL;

                    fitResultRachlin->Status = "Exceeded Bounds";
                }
                else
                {
                    mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Rachlin, mFittingObject->bicRachlin));

                    fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin K"), mFittingObject->fitRachlinK));
                    fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin S"), mFittingObject->fitRachlinS));
                    fitResultRachlin->RMS = mFittingObject->GetReport().rmserror;
                    fitResultRachlin->AIC = mFittingObject->aicRachlin;
                    fitResultRachlin->BIC = mFittingObject->bicRachlin;
                    fitResultRachlin->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
                }
            }
            else
            {
                fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin K"), NULL));
                fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin S"), NULL));
                fitResultRachlin->RMS = NULL;
                fitResultRachlin->AIC = NULL;
                fitResultRachlin->BIC = NULL;
                fitResultRachlin->BF = NULL;
                fitResultRachlin->Probability = NULL;
                fitResultRachlin->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
        }

        if (runLocalRodriguezLogue)
        {
            if (true)
            {
                p1Span = abs(-12) + abs(12); // -12 to 12
                p1Step = p1Span / 10;

                p2Span = abs(-12) + abs(12); // -12 to 12
                p2Step = p2Span / 10;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 10; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 10; sLoop++)
                    {
                        provisionalValues.startingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);
                        provisionalValues.startingValueArray[grandLoop].p2 = 12 - ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.startingValueArray)
                {
                    obj.err = mFittingObject->getErrorRodriguezLogue(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.startingValueArray, provisionalValues.startingValueArray + 100);

                mFittingObject->FitRodriguezLogue(QString("[%1,%2]")
                                                   .arg(provisionalValues.startingValueArray[0].p1)
                                                   .arg(exp(provisionalValues.startingValueArray[0].p2))
                                                   .toUtf8().constData());
            }
            else
            {
                mFittingObject->FitRodriguezLogue("[-5, 0.3]");
            }

            //mFittingObject->FitRodriguezLogue("[-5, 0.3]");

            fitResultRodriguezLogue = new FitResult(ModelType::RodriguezLogue);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::RodriguezLogue, mFittingObject->bicRodriguezLogue));

                fitResultRodriguezLogue->Params.append(QPair<QString, double>(QString("Rodriguez-Logue K"), mFittingObject->fitRodriguezLogueK));
                fitResultRodriguezLogue->Params.append(QPair<QString, double>(QString("Rodriguez-Logue Beta"), mFittingObject->fitRodriguezLogueBeta));
                fitResultRodriguezLogue->RMS = mFittingObject->GetReport().rmserror;
                fitResultRodriguezLogue->AIC = mFittingObject->aicRodriguezLogue;
                fitResultRodriguezLogue->BIC = mFittingObject->bicRodriguezLogue;
                fitResultRodriguezLogue->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
            else
            {
                fitResultRodriguezLogue->Params.append(QPair<QString, double>(QString("Rodriguez-Logue K"), NULL));
                fitResultRodriguezLogue->Params.append(QPair<QString, double>(QString("Rodriguez-Logue Beta"), NULL));
                fitResultRodriguezLogue->RMS = NULL;
                fitResultRodriguezLogue->AIC = NULL;
                fitResultRodriguezLogue->BIC = NULL;
                fitResultRodriguezLogue->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
        }

        if (runLocalEbertPrelec)
        {
            if (true)
            {
                p1Span = abs(-12) + abs(12); // -12 to 12
                p1Step = p1Span / 10;

                p2Span = 10; // .1 to 10
                p2Step = p2Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 10; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 100; sLoop++)
                    {
                        provisionalValues.startingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);
                        provisionalValues.startingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.startingValueArray)
                {
                    obj.err = mFittingObject->getErrorEbertPrelec(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.startingValueArray, provisionalValues.startingValueArray + 1000);

                mFittingObject->FitEbertPrelec(QString("[%1,%2]")
                                                   .arg(provisionalValues.startingValueArray[0].p1)
                                                   .arg(provisionalValues.startingValueArray[0].p2)
                                                   .toUtf8().constData());
            }
            else
            {
                mFittingObject->FitEbertPrelec("[-5, 0.3]");
            }

            //mFittingObject->FitEbertPrelec("[-5.0, 0.5]");

            fitResultEbertPrelec = new FitResult(ModelType::EbertPrelec);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::EbertPrelec, mFittingObject->bicEbertPrelec));

                fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec K"), mFittingObject->fitEbertPrelecK));
                fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec S"), mFittingObject->fitEbertPrelecS));
                fitResultEbertPrelec->RMS = mFittingObject->GetReport().rmserror;
                fitResultEbertPrelec->AIC = mFittingObject->aicEbertPrelec;
                fitResultEbertPrelec->BIC = mFittingObject->bicEbertPrelec;
                fitResultEbertPrelec->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
            else
            {
                fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec K"), NULL));
                fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec S"), NULL));
                fitResultEbertPrelec->RMS = NULL;
                fitResultEbertPrelec->AIC = NULL;
                fitResultEbertPrelec->BIC = NULL;
                fitResultEbertPrelec->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
        }

        if (runLocalBleicholdt)
        {
            if (true)
            {
                p1Span = abs(-12) + abs(12); // -12 to 12
                p1Step = p1Span / 10;

                p2Span = 1; // .1 to 1
                p2Step = p2Span / 10;

                p3Span = 1; // .1 to 1
                p3Step = p3Span / 10;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 10; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 10; sLoop++)
                    {
                        for (int bLoop = 0; bLoop < 10; bLoop++)
                        {
                            provisionalValues.startingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);
                            provisionalValues.startingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);
                            provisionalValues.startingValueArray[grandLoop].p3 = ((bLoop + 1) * p3Step);

                            grandLoop++;
                        }
                   }
                }

                for(BruteForce & obj : provisionalValues.startingValueArray)
                {
                    obj.err = mFittingObject->getErrorBleichrodt(obj.p1, obj.p2, obj.p3);
                }

                std::sort(provisionalValues.startingValueArray, provisionalValues.startingValueArray + 1000);

                mFittingObject->FitBleichrodt(QString("[%1,%2,%3]")
                                                   .arg(provisionalValues.startingValueArray[0].p1)
                                                   .arg(provisionalValues.startingValueArray[0].p2)
                                                   .arg(provisionalValues.startingValueArray[0].p3)
                                                   .toUtf8().constData());
            }
            else
            {
                mFittingObject->FitBleichrodt("[-5.0, 0.5, 0.5]");
            }

            //mFittingObject->FitBleichrodt("[-5.0, 0.5, 0.5]");

            fitResultBleichrodt = new FitResult(ModelType::Beleichrodt);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Beleichrodt, mFittingObject->bicBleichrodt));

                fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt K"), mFittingObject->fitBleichrodtK));
                fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt S"), mFittingObject->fitBleichrodtS));
                fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt Beta"), mFittingObject->fitBleichrodtBeta));
                fitResultBleichrodt->RMS = mFittingObject->GetReport().rmserror;
                fitResultBleichrodt->AIC = mFittingObject->aicBleichrodt;
                fitResultBleichrodt->BIC = mFittingObject->bicBleichrodt;
                fitResultBleichrodt->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
            else
            {
                fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt K"), NULL));
                fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt S"), NULL));
                fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt Beta"), NULL));
                fitResultBleichrodt->RMS = NULL;
                fitResultBleichrodt->AIC = NULL;
                fitResultBleichrodt->BIC = NULL;
                fitResultBleichrodt->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
        }

        mFittingObject->FitNoise();
        mFittingObject->NoiseBIC = mFittingObject->bicNoise;

        fitResultNoise = new FitResult(ModelType::Noise);
        fitResultNoise->Params.append(QPair<QString, double>(QString("Noise mean"), mFittingObject->GetNoiseMean()));
        fitResultNoise->RMS = mFittingObject->NoiseRMSE;
        fitResultNoise->AIC = mFittingObject->aicNoise;
        fitResultNoise->BIC = mFittingObject->bicNoise;
        fitResultNoise->Status = QString("---");

        mFittingObject->PrepareProbabilities();

        qSort(mFittingObject->mProbList.begin(), mFittingObject->mProbList.end(), QPairSecondComparer());

        fitResultNoise->BF = mFittingObject->bfNoise;
        fitResultNoise->Probability = mFittingObject->probsNoise;
            fitResults->FittingResults.append(fitResultNoise);

        if (runLocalExponential)
        {
            fitResultExponential->BF = mFittingObject->bfExponential;
            fitResultExponential->Probability = mFittingObject->probsExponential;
                fitResults->FittingResults.append(fitResultExponential);
        }

        if (runLocalHyperbolic)
        {
            fitResultHyperbolic->BF = mFittingObject->bfHyperbolic;
            fitResultHyperbolic->Probability = mFittingObject->probsHyperbolic;
                fitResults->FittingResults.append(fitResultHyperbolic);
        }

        if (runLocalBetaDelta)
        {
            fitResultBetaDelta->BF = mFittingObject->bfQuasiHyperbolic;
            fitResultBetaDelta->Probability = mFittingObject->probsQuasiHyperbolic;
                fitResults->FittingResults.append(fitResultBetaDelta);
        }

        if (runLocalMyersonGreen)
        {
            fitResultGreenMyerson->BF = mFittingObject->bfMyerson;
            fitResultGreenMyerson->Probability = mFittingObject->probsMyerson;
                fitResults->FittingResults.append(fitResultGreenMyerson);
        }

        if (runLocalRachlin)
        {
            fitResultRachlin->BF = mFittingObject->bfRachlin;
            fitResultRachlin->Probability = mFittingObject->probsRachlin;
                fitResults->FittingResults.append(fitResultRachlin);
        }

        if (runLocalRodriguezLogue)
        {
            fitResultRodriguezLogue->BF = mFittingObject->bfRodriguezLogue;
            fitResultRodriguezLogue->Probability = mFittingObject->probsRodriguezLogue;
                fitResults->FittingResults.append(fitResultRodriguezLogue);
        }

        if (runLocalEbertPrelec)
        {
            fitResultEbertPrelec->BF = mFittingObject->bfEbertPrelec;
            fitResultEbertPrelec->Probability = mFittingObject->probsEbertPrelec;
                fitResults->FittingResults.append(fitResultEbertPrelec);
        }

        if (runLocalBleicholdt)
        {
            fitResultBleichrodt->BF = mFittingObject->bfBleichrodt;
            fitResultBleichrodt->Probability = mFittingObject->probsBleichrodt;
                fitResults->FittingResults.append(fitResultBleichrodt);
        }

        QString mModel;

        switch(mFittingObject->mProbList.first().first)
        {
            case ModelType::Noise:
                mModel = "Noise";
                break;

            case ModelType::Exponential:
                mModel = "Exponential";
                break;

            case ModelType::Hyperbolic:
                mModel = "Hyperbolic";
                break;

            case ModelType::BetaDelta:
                mModel = "Quasi-Hyperbolic";
                break;

            case ModelType::Myerson:
                mModel = "Myerson-Green";
                break;

            case ModelType::Rachlin:
                mModel = "Rachlin";
                break;

            case ModelType::RodriguezLogue:
                mModel = "Rodriguez-Logue";
                break;

            case ModelType::EbertPrelec:
                mModel = "Ebert-Prelec";
                break;

            case ModelType::Beleichrodt:
                mModel = "Beleichrodt";
                break;
        }

        fitResults->TopModel = mModel;
        fitResults->TopModelType = mFittingObject->mProbList.first().first;

        fitResults->ParticipantDelays = tempList[2];
        fitResults->ParticipantValues = tempList[3];

        fitResults->TopED50 = mFittingObject->getED50BestModel(mFittingObject->mProbList.first().first);

        if (runLocalArea)
        {
            fitResults->TopAUC = mFittingObject->getAUCBestModel(mFittingObject->mProbList.first().first);
            fitResults->TopAUCLog = mFittingObject->getLogAUCBestModel(mFittingObject->mProbList.first().first);
        }

        emit workingResult(*fitResults);
    }

    emit workFinished();
}

QString CalculationWorker::formatStringResult(double value, bool returnLogNormal)
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
