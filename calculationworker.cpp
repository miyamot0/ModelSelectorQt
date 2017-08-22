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

#include <QtWidgets>

#include "calculationworker.h"
#include "qstringlist.h"

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

    runBruteForce = calculationSettings->cbBruteForce;

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
            if (!runBruteForce)
            {
                p1Span = abs(-12) + abs(12); // -12 to 12
                p1Step = p1Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 100; kLoop++)
                {
                    provisionalValues.oneParamStartingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);

                    grandLoop++;
                }

                for(BruteForce & obj : provisionalValues.oneParamStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorExponential(obj.p1);
                }

                std::sort(provisionalValues.oneParamStartingValueArray, provisionalValues.oneParamStartingValueArray + 100);

                mFittingObject->FitExponential(QString("[%1]").arg(provisionalValues.oneParamStartingValueArray[0].p1).toUtf8().constData());
            }
            else
            {
                p1Span = abs(-20) + abs(20); // -20 to 20
                p1Step = p1Span / 10000;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 10000; kLoop++)
                {
                    provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 20 - ((kLoop + 1) * p1Step);

                    grandLoop++;
                }

                for(BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorExponential(obj.p1);
                }

                std::sort(provisionalValues.smallBruteStartingValueArray, provisionalValues.smallBruteStartingValueArray + 10000);

                mFittingObject->FitExponential(QString("[%1]").arg(provisionalValues.smallBruteStartingValueArray[0].p1).toUtf8().constData());
            }

            fitResultExponential = new FitResult(ModelType::Exponential);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Exponential, mFittingObject->bicExponential));

                double lnK = (runLogarithmicResults) ? exp(mFittingObject->fitExponentialK) : mFittingObject->fitExponentialK;
                fitResultExponential->Params.append(QPair<QString, double>(QString("Exponential K"), lnK));
                fitResultExponential->RMS = mFittingObject->GetReport().rmserror;
                fitResultExponential->AIC = mFittingObject->aicExponential;
                fitResultExponential->BIC = mFittingObject->bicExponential;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultExponential->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

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
            if (!runBruteForce)
            {
                p1Span = abs(-12) + abs(12); // -12 to 12
                p1Step = p1Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 100; kLoop++)
                {
                    provisionalValues.oneParamStartingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);

                    grandLoop++;
                }

                for(BruteForce & obj : provisionalValues.oneParamStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorHyperbolic(obj.p1);
                }

                std::sort(provisionalValues.oneParamStartingValueArray, provisionalValues.oneParamStartingValueArray + 100);

                mFittingObject->FitHyperbolic(QString("[%1]").arg(provisionalValues.oneParamStartingValueArray[0].p1).toUtf8().constData());
            }
            else
            {
                p1Span = abs(-20) + abs(20); // -20 to 20
                p1Step = p1Span / 10000;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 10000; kLoop++)
                {
                    provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 20 - ((kLoop + 1) * p1Step);

                    grandLoop++;
                }

                for(BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorHyperbolic(obj.p1);
                }

                std::sort(provisionalValues.smallBruteStartingValueArray, provisionalValues.smallBruteStartingValueArray + 10000);

                mFittingObject->FitHyperbolic(QString("[%1]").arg(provisionalValues.smallBruteStartingValueArray[0].p1).toUtf8().constData());
            }

            fitResultHyperbolic = new FitResult(ModelType::Hyperbolic);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Hyperbolic, mFittingObject->bicHyperbolic));

                double lnK = (runLogarithmicResults) ? exp(mFittingObject->fitHyperbolicK) : mFittingObject->fitHyperbolicK;

                fitResultHyperbolic->Params.append(QPair<QString, double>(QString("Hyperbolic K"), lnK));
                fitResultHyperbolic->RMS = mFittingObject->GetReport().rmserror;
                fitResultHyperbolic->AIC = mFittingObject->aicHyperbolic;
                fitResultHyperbolic->BIC = mFittingObject->bicHyperbolic;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultHyperbolic->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

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
            if (!runBruteForce)
            {
                p1Span = 1; // 0 to 1
                p1Step = p1Span / 10;

                p2Span = 1;
                p2Step = p2Span / 100;

                grandLoop = 0;

                for (int bLoop = 0; bLoop < 10; bLoop++)
                {
                    for (int dLoop = 0; dLoop < 100; dLoop++)
                    {
                        provisionalValues.twoParamStartingValueArray[grandLoop].p1 = ((bLoop + 1) * p1Step);
                        provisionalValues.twoParamStartingValueArray[grandLoop].p2 = ((dLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.twoParamStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorQuasiHyperbolic(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.twoParamStartingValueArray, provisionalValues.twoParamStartingValueArray + 1000);

                mFittingObject->FitQuasiHyperbolic(QString("[%1,%2]")
                                                   .arg(provisionalValues.twoParamStartingValueArray[0].p1)
                                                   .arg(provisionalValues.twoParamStartingValueArray[0].p2)
                                                   .toUtf8().constData());
            }
            else
            {
                p1Span = 1; // 0 to 1
                p1Step = p1Span / 100;

                p2Span = 1;
                p2Step = p2Span / 100;

                grandLoop = 0;

                for (int bLoop = 0; bLoop < 100; bLoop++)
                {
                    for (int dLoop = 0; dLoop < 100; dLoop++)
                    {
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = ((bLoop + 1) * p1Step);
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p2 = ((dLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorQuasiHyperbolic(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.smallBruteStartingValueArray, provisionalValues.smallBruteStartingValueArray + 10000);

                mFittingObject->FitQuasiHyperbolic(QString("[%1,%2]")
                                                   .arg(provisionalValues.smallBruteStartingValueArray[0].p1)
                                                   .arg(provisionalValues.smallBruteStartingValueArray[0].p2)
                                                   .toUtf8().constData());
            }

            fitResultBetaDelta = new FitResult(ModelType::BetaDelta);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::BetaDelta, mFittingObject->bicQuasiHyperbolic));

                fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Beta"), mFittingObject->fitQuasiHyperbolicBeta));
                fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Delta"), mFittingObject->fitQuasiHyperbolicDelta));
                fitResultBetaDelta->RMS = mFittingObject->GetReport().rmserror;
                fitResultBetaDelta->AIC = mFittingObject->aicQuasiHyperbolic;
                fitResultBetaDelta->BIC = mFittingObject->bicQuasiHyperbolic;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultBetaDelta->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

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
            if (!runBruteForce)
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
                        provisionalValues.twoParamStartingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);
                        provisionalValues.twoParamStartingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.twoParamStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorGreenMyerson(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.twoParamStartingValueArray, provisionalValues.twoParamStartingValueArray + 1000);

                mFittingObject->FitMyerson(QString("[%1,%2]")
                                                   .arg(provisionalValues.twoParamStartingValueArray[0].p1)
                                                   .arg(provisionalValues.twoParamStartingValueArray[0].p2)
                                                   .toUtf8().constData());
            }
            else
            {
                p1Span = abs(-20) + abs(20); // -20 to 20
                p1Step = p1Span / 100;

                p2Span = 10; // .1 to 10
                p2Step = p2Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 100; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 100; sLoop++)
                    {
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 20 - ((kLoop + 1) * p1Step);
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorGreenMyerson(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.smallBruteStartingValueArray, provisionalValues.smallBruteStartingValueArray + 10000);

                mFittingObject->FitMyerson(QString("[%1,%2]")
                                                   .arg(provisionalValues.smallBruteStartingValueArray[0].p1)
                                                   .arg(provisionalValues.smallBruteStartingValueArray[0].p2)
                                                   .toUtf8().constData());
            }

            fitResultGreenMyerson = new FitResult(ModelType::Myerson);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Myerson, mFittingObject->bicMyerson));

                fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson K"), mFittingObject->fitMyersonK));
                fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson S"), mFittingObject->fitMyersonS));
                fitResultGreenMyerson->RMS = mFittingObject->GetReport().rmserror;
                fitResultGreenMyerson->AIC = mFittingObject->aicMyerson;
                fitResultGreenMyerson->BIC = mFittingObject->bicMyerson;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultGreenMyerson->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

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
            if (!runBruteForce)
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
                        provisionalValues.twoParamStartingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);
                        provisionalValues.twoParamStartingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.twoParamStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorGreenRachlin(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.twoParamStartingValueArray, provisionalValues.twoParamStartingValueArray + 1000);

                mFittingObject->FitRachlin(QString("[%1,%2]")
                                                   .arg(provisionalValues.twoParamStartingValueArray[0].p1)
                                                   .arg(provisionalValues.twoParamStartingValueArray[0].p2)
                                                   .toUtf8().constData());
            }
            else
            {
                p1Span = abs(-20) + abs(20); // -20 to 20
                p1Step = p1Span / 100;

                p2Span = 10; // .1 to 10
                p2Step = p2Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 100; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 100; sLoop++)
                    {
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 20 - ((kLoop + 1) * p1Step);
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorGreenRachlin(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.smallBruteStartingValueArray, provisionalValues.smallBruteStartingValueArray + 10000);

                mFittingObject->FitRachlin(QString("[%1,%2]")
                                                   .arg(provisionalValues.smallBruteStartingValueArray[0].p1)
                                                   .arg(provisionalValues.smallBruteStartingValueArray[0].p2)
                                                   .toUtf8().constData());
            }

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

                    for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                    {
                        fitResultRachlin->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                    }

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
            if (!runBruteForce)
            {
                p1Span = abs(-12) + abs(12); // -12 to 12
                p1Step = p1Span / 10;

                p2Span = abs(-12) + abs(12); // -12 to 12
                p2Step = p2Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 10; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 100; sLoop++)
                    {
                        provisionalValues.twoParamStartingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);
                        provisionalValues.twoParamStartingValueArray[grandLoop].p2 = 12 - ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.twoParamStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorRodriguezLogue(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.twoParamStartingValueArray, provisionalValues.twoParamStartingValueArray + 1000);

                mFittingObject->FitRodriguezLogue(QString("[%1,%2]")
                                                   .arg(provisionalValues.twoParamStartingValueArray[0].p1)
                                                   .arg(exp(provisionalValues.twoParamStartingValueArray[0].p2))
                                                   .toUtf8().constData());
            }
            else
            {
                p1Span = abs(-20) + abs(20); // -20 to 20
                p1Step = p1Span / 100;

                p2Span = abs(-20) + abs(20); // -20 to 20
                p2Step = p2Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 100; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 100; sLoop++)
                    {
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 20 - ((kLoop + 1) * p1Step);
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p2 = 20 - ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorRodriguezLogue(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.smallBruteStartingValueArray, provisionalValues.smallBruteStartingValueArray + 10000);

                mFittingObject->FitRodriguezLogue(QString("[%1,%2]")
                                                   .arg(provisionalValues.smallBruteStartingValueArray[0].p1)
                                                   .arg(exp(provisionalValues.smallBruteStartingValueArray[0].p2))
                                                   .toUtf8().constData());
            }

            fitResultRodriguezLogue = new FitResult(ModelType::RodriguezLogue);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::RodriguezLogue, mFittingObject->bicRodriguezLogue));

                fitResultRodriguezLogue->Params.append(QPair<QString, double>(QString("Rodriguez-Logue K"), mFittingObject->fitRodriguezLogueK));
                fitResultRodriguezLogue->Params.append(QPair<QString, double>(QString("Rodriguez-Logue Beta"), mFittingObject->fitRodriguezLogueBeta));
                fitResultRodriguezLogue->RMS = mFittingObject->GetReport().rmserror;
                fitResultRodriguezLogue->AIC = mFittingObject->aicRodriguezLogue;
                fitResultRodriguezLogue->BIC = mFittingObject->bicRodriguezLogue;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultRodriguezLogue->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

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
            if (!runBruteForce)
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
                        provisionalValues.twoParamStartingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);
                        provisionalValues.twoParamStartingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.twoParamStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorEbertPrelec(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.twoParamStartingValueArray, provisionalValues.twoParamStartingValueArray + 1000);

                mFittingObject->FitEbertPrelec(QString("[%1,%2]")
                                                   .arg(provisionalValues.twoParamStartingValueArray[0].p1)
                                                   .arg(provisionalValues.twoParamStartingValueArray[0].p2)
                                                   .toUtf8().constData());
            }
            else
            {
                p1Span = abs(-20) + abs(20); // -20 to 20
                p1Step = p1Span / 100;

                p2Span = 10; // .1 to 10
                p2Step = p2Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 100; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 100; sLoop++)
                    {
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 20 - ((kLoop + 1) * p1Step);
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);

                        grandLoop++;
                    }
                }

                for(BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorEbertPrelec(obj.p1, obj.p2);
                }

                std::sort(provisionalValues.smallBruteStartingValueArray, provisionalValues.smallBruteStartingValueArray + 10000);

                mFittingObject->FitEbertPrelec(QString("[%1,%2]")
                                                   .arg(provisionalValues.smallBruteStartingValueArray[0].p1)
                                                   .arg(provisionalValues.smallBruteStartingValueArray[0].p2)
                                                   .toUtf8().constData());
            }

            fitResultEbertPrelec = new FitResult(ModelType::EbertPrelec);

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::EbertPrelec, mFittingObject->bicEbertPrelec));

                fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec K"), mFittingObject->fitEbertPrelecK));
                fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec S"), mFittingObject->fitEbertPrelecS));
                fitResultEbertPrelec->RMS = mFittingObject->GetReport().rmserror;
                fitResultEbertPrelec->AIC = mFittingObject->aicEbertPrelec;
                fitResultEbertPrelec->BIC = mFittingObject->bicEbertPrelec;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultEbertPrelec->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

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
            if (!runBruteForce)
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
                            provisionalValues.threeParamStartingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);
                            provisionalValues.threeParamStartingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);
                            provisionalValues.threeParamStartingValueArray[grandLoop].p3 = ((bLoop + 1) * p3Step);

                            grandLoop++;
                        }
                   }
                }

                for(BruteForce & obj : provisionalValues.threeParamStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorBleichrodt(obj.p1, obj.p2, obj.p3);
                }

                std::sort(provisionalValues.threeParamStartingValueArray, provisionalValues.threeParamStartingValueArray + 1000);

                mFittingObject->FitBleichrodt(QString("[%1,%2,%3]")
                                                   .arg(provisionalValues.threeParamStartingValueArray[0].p1)
                                                   .arg(provisionalValues.threeParamStartingValueArray[0].p2)
                                                   .arg(provisionalValues.threeParamStartingValueArray[0].p3)
                                                   .toUtf8().constData());
            }
            else
            {
                p1Span = abs(-20) + abs(20); // -20 to 20
                p1Step = p1Span / 100;

                p2Span = 1; // .1 to 10
                p2Step = p2Span / 100;

                p3Span = 1; // .1 to 10
                p3Step = p3Span / 100;

                grandLoop = 0;

                for (int kLoop = 0; kLoop < 100; kLoop++)
                {
                    for (int sLoop = 0; sLoop < 100; sLoop++)
                    {
                        for (int bLoop = 0; bLoop < 100; bLoop++)
                        {
                            provisionalValues.largeBruteStartingValueArray[grandLoop].p1 = 20 - ((kLoop + 1) * p1Step);
                            provisionalValues.largeBruteStartingValueArray[grandLoop].p2 = ((sLoop + 1) * p2Step);
                            provisionalValues.largeBruteStartingValueArray[grandLoop].p3 = ((bLoop + 1) * p3Step);

                            grandLoop++;
                        }
                   }
                }

                for(BruteForce & obj : provisionalValues.largeBruteStartingValueArray)
                {
                    obj.err = mFittingObject->getErrorBleichrodt(obj.p1, obj.p2, obj.p3);
                }

                std::sort(provisionalValues.largeBruteStartingValueArray, provisionalValues.largeBruteStartingValueArray + 1000000);

                mFittingObject->FitBleichrodt(QString("[%1,%2,%3]")
                                                   .arg(provisionalValues.largeBruteStartingValueArray[0].p1)
                                                   .arg(provisionalValues.largeBruteStartingValueArray[0].p2)
                                                   .arg(provisionalValues.largeBruteStartingValueArray[0].p3)
                                                   .toUtf8().constData());
            }

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

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultBleichrodt->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

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

        for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
        {
            fitResultNoise->ErrPar.append(mFittingObject->ErrorResidual[v]);
        }

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
        QList<double> mTopErrPar;

        switch(mFittingObject->mProbList.first().first)
        {
            case ModelType::Noise:
                mModel = "Noise";
                mTopErrPar = fitResultNoise->ErrPar;
                break;

            case ModelType::Exponential:
                mModel = "Exponential";
                mTopErrPar = fitResultExponential->ErrPar;
                break;

            case ModelType::Hyperbolic:
                mModel = "Hyperbolic";
                mTopErrPar = fitResultHyperbolic->ErrPar;
                break;

            case ModelType::BetaDelta:
                mModel = "Quasi-Hyperbolic";
                mTopErrPar = fitResultBetaDelta->ErrPar;
                break;

            case ModelType::Myerson:
                mModel = "Myerson-Green";
                mTopErrPar = fitResultGreenMyerson->ErrPar;
                break;

            case ModelType::Rachlin:
                mModel = "Rachlin";
                mTopErrPar = fitResultRachlin->ErrPar;
                break;

            case ModelType::RodriguezLogue:
                mModel = "Rodriguez-Logue";
                mTopErrPar = fitResultRodriguezLogue->ErrPar;
                break;

            case ModelType::EbertPrelec:
                mModel = "Ebert-Prelec";
                mTopErrPar = fitResultEbertPrelec->ErrPar;
                break;

            case ModelType::Beleichrodt:
                mModel = "Beleichrodt";
                mTopErrPar = fitResultBleichrodt->ErrPar;
                break;
        }

        fitResults->TopModel = mModel;
        fitResults->TopModelType = mFittingObject->mProbList.first().first;
        fitResults->TopErrPar = mTopErrPar;

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
