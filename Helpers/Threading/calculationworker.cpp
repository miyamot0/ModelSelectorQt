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

CalculationWorker::CalculationWorker(QList<QPair<QString, QString> > mJohnsonBickelResults, QList<bool> *mJonhsonBickelSelections,
                                     QList<QStringList> mStoredValues, CalculationSettings *calculationSettings, int processChecking)
{
    settings = calculationSettings;

    mLocalJohnsonBickelResults = mJohnsonBickelResults;
    mLocalJonhsonBickelSelections = mJonhsonBickelSelections;
    mLocalStoredValues = mStoredValues;

    mFittingObject = new ModelSelection();
    mFittingObject->SetFittingAlgorithm(calculationSettings->settingsFitting);

    processCheckingLocal = processChecking;
}

void CalculationWorker::startWork()
{
    emit workStarted();
}

void CalculationWorker::working()
{
    fitResults = new FitResults();

    tempList.clear();

    for (int i = 0; i < mLocalStoredValues.count(); i++)
    {
        fitResults->Header = QString::number(i+1);

        if (processCheckingLocal == (int) SystematicCheckTags::RunPassing)
        {
            if (mLocalJohnsonBickelResults[i].first.contains("Fail", Qt::CaseInsensitive) ||
                mLocalJohnsonBickelResults[i].second.contains("Fail", Qt::CaseInsensitive))
            {
                fitResults->Header = QString::number(i+1) + " (Dropped)";

                continue;
            }
        }
        else if (processCheckingLocal == (int) SystematicCheckTags::RunSelected)
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

        if (settings->modelExponential)
        {
            if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mFittingObject->FitExponential(NULL);

                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Exponential, mFittingObject->bicExponential));

                double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitExponentialK) : mFittingObject->fitExponentialK;

                fitResultExponential = new FitResult(ModelType::Exponential);
                fitResultExponential->Params.append(QPair<QString, double>(QString("Exponential K"), lnK));

                fitResultExponential->RMS = mFittingObject->rmseExponential;
                fitResultExponential->AIC = mFittingObject->aicExponential;
                fitResultExponential->BIC = mFittingObject->bicExponential;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultExponential->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

                fitResultExponential->Status = "---";
            }
            else
            {
                if (!settings->cbBruteForce)
                {
                    p1Span = abs(-12) + abs(12); // -12 to 12
                    p1Step = p1Span / 100;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 100; kLoop++)
                    {
                        provisionalValues.oneParamStartingValueArray[grandLoop].p1 = 12 - ((kLoop + 1) * p1Step);

                        grandLoop++;
                    }

                    for (BruteForce & obj : provisionalValues.oneParamStartingValueArray)
                    {
                        obj.err = mFittingObject->getErrorExponential(obj.p1);
                    }

                    std::sort(provisionalValues.oneParamStartingValueArray, provisionalValues.oneParamStartingValueArray + 100);

                    mFittingObject->FitExponential(QString("[%1]").arg(provisionalValues.oneParamStartingValueArray[0].p1).toUtf8().constData());
                }
                else
                {
                    p1Span = abs(-100) + abs(100); // -100 to 100
                    p1Step = p1Span / 10000;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 10000; kLoop++)
                    {
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 20 - ((kLoop + 1) * p1Step);

                        grandLoop++;
                    }

                    for (BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
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

                    double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitExponentialK) : mFittingObject->fitExponentialK;

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
        }

        if (settings->modelParabolic)
        {
            if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mFittingObject->FitParabolic(NULL);

                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Parabolic, mFittingObject->bicParabolic));

                double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitParabolicK) : mFittingObject->fitParabolicK;

                fitResultParabolic = new FitResult(ModelType::Parabolic);
                fitResultParabolic->Params.append(QPair<QString, double>(QString("Parabolic K"), lnK));

                fitResultParabolic->RMS = mFittingObject->rmseParabolic;
                fitResultParabolic->AIC = mFittingObject->aicParabolic;
                fitResultParabolic->BIC = mFittingObject->bicParabolic;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultParabolic->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

                fitResultParabolic->Status = "---";
            }
            else
            {
                if (!settings->cbBruteForce)
                {
                    p1Span = abs(-30) + abs(30);
                    p1Step = p1Span / 100;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 100; kLoop++)
                    {
                        provisionalValues.oneParamStartingValueArray[grandLoop].p1 = 30 - ((kLoop + 1) * p1Step);

                        grandLoop++;
                    }

                    for (BruteForce & obj : provisionalValues.oneParamStartingValueArray)
                    {
                        obj.err = mFittingObject->getErrorParabolic(obj.p1);
                    }

                    std::sort(provisionalValues.oneParamStartingValueArray, provisionalValues.oneParamStartingValueArray + 100);

                    mFittingObject->FitParabolic(QString("[%1]").arg(provisionalValues.oneParamStartingValueArray[0].p1).toUtf8().constData());
                }
                else
                {
                    p1Span = abs(-100) + abs(100); // -100 to 100
                    p1Step = p1Span / 10000;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 10000; kLoop++)
                    {
                        provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 20 - ((kLoop + 1) * p1Step);

                        grandLoop++;
                    }

                    for (BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
                    {
                        obj.err = mFittingObject->getErrorParabolic(obj.p1);
                    }

                    std::sort(provisionalValues.smallBruteStartingValueArray, provisionalValues.smallBruteStartingValueArray + 10000);

                    mFittingObject->FitParabolic(QString("[%1]").arg(provisionalValues.smallBruteStartingValueArray[0].p1).toUtf8().constData());
                }

                fitResultParabolic = new FitResult(ModelType::Parabolic);

                if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
                {
                    mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Parabolic, mFittingObject->bicParabolic));

                    double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitParabolicK) : mFittingObject->fitParabolicK;

                    fitResultParabolic->Params.append(QPair<QString, double>(QString("Parabolic K"), lnK));
                    fitResultParabolic->RMS = mFittingObject->GetReport().rmserror;
                    fitResultParabolic->AIC = mFittingObject->aicParabolic;
                    fitResultParabolic->BIC = mFittingObject->bicParabolic;

                    for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                    {
                        fitResultParabolic->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                    }

                    fitResultParabolic->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
                }
                else
                {
                    fitResultParabolic->Params.append(QPair<QString, double>(QString("Parabolic K"), NULL));
                    fitResultParabolic->RMS = NULL;
                    fitResultParabolic->AIC = NULL;
                    fitResultParabolic->BIC = NULL;
                    fitResultParabolic->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
                }
            }
        }

        if (settings->modelHyperbolic)
        {
            if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mFittingObject->FitHyperbolic(NULL);

                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Hyperbolic, mFittingObject->bicHyperbolic));

                double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitHyperbolicK) : mFittingObject->fitHyperbolicK;

                fitResultHyperbolic = new FitResult(ModelType::Hyperbolic);
                fitResultHyperbolic->Params.append(QPair<QString, double>(QString("Hyperbolic K"), lnK));

                fitResultHyperbolic->RMS = mFittingObject->rmseHyperbolic;
                fitResultHyperbolic->AIC = mFittingObject->aicHyperbolic;
                fitResultHyperbolic->BIC = mFittingObject->bicHyperbolic;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultHyperbolic->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

                fitResultHyperbolic->Status = "---";

            }
            else
            {
                if (!settings->cbBruteForce)
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
                    p1Span = abs(-100) + abs(100); // -20 to 20
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

                    double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitHyperbolicK) : mFittingObject->fitHyperbolicK;

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
        }

        if (settings->modelQuasiHyperbolic)
        {
            if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mFittingObject->FitQuasiHyperbolic(NULL);

                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::BetaDelta, mFittingObject->bicQuasiHyperbolic));

                double delta = (settings->logNormalParameters) ? (1.0 - mFittingObject->fitQuasiHyperbolicDelta) : mFittingObject->fitQuasiHyperbolicDelta;

                fitResultBetaDelta = new FitResult(ModelType::BetaDelta);
                fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Beta"), mFittingObject->fitQuasiHyperbolicBeta));
                fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Delta"), delta));

                fitResultBetaDelta->RMS = mFittingObject->rmseQuasiHyperbolic;
                fitResultBetaDelta->AIC = mFittingObject->aicQuasiHyperbolic;
                fitResultBetaDelta->BIC = mFittingObject->bicQuasiHyperbolic;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultBetaDelta->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

                fitResultBetaDelta->Status = "---";
            }
            else
            {
                if (!settings->cbBruteForce)
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

                    double delta = (settings->logNormalParameters) ? (1.0 - mFittingObject->fitQuasiHyperbolicDelta) : mFittingObject->fitQuasiHyperbolicDelta;

                    fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Beta"), mFittingObject->fitQuasiHyperbolicBeta));
                    fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Delta"), delta));
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
        }

        if (settings->modelMyersonGreen)
        {
            if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mFittingObject->FitMyerson(NULL);

                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Myerson, mFittingObject->bicMyerson));

                fitResultGreenMyerson = new FitResult(ModelType::Myerson);

                double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitMyersonK) : mFittingObject->fitMyersonK;

                fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson K"), lnK));
                fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson S"), mFittingObject->fitMyersonS));

                fitResultGreenMyerson->RMS = mFittingObject->rmseMyerson;
                fitResultGreenMyerson->AIC = mFittingObject->aicMyerson;
                fitResultGreenMyerson->BIC = mFittingObject->bicMyerson;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultGreenMyerson->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

                fitResultGreenMyerson->Status = "---";

            }
            else
            {
                if (!settings->cbBruteForce)
                {
                    p1Span = abs(-12) + abs(12);
                    p1Step = (double) p1Span / 10;

                    p2Span = abs(-12) + abs(12);
                    p2Step = (double) p2Span / 100;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 10; kLoop++)
                    {
                        for (int sLoop = 0; sLoop < 100; sLoop++)
                        {
                            provisionalValues.twoParamStartingValueArray[grandLoop].p1 = 12.0 - (((double) kLoop + 1) * p1Step);
                            provisionalValues.twoParamStartingValueArray[grandLoop].p2 = 12.0 - (((double) sLoop + 1) * p2Step);

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
                                                       .arg(exp(provisionalValues.twoParamStartingValueArray[0].p2))
                                                       .toUtf8().constData());
                }
                else
                {
                    p1Span = abs(-100) + abs(100);
                    p1Step = (double) p1Span / 100;

                    p2Span = abs(-100) + abs(100);
                    p2Step = (double) p2Span / 100;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 100; kLoop++)
                    {
                        for (int sLoop = 0; sLoop < 100; sLoop++)
                        {
                            provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 100.0 - (((double) kLoop + 1) * p1Step);
                            provisionalValues.smallBruteStartingValueArray[grandLoop].p2 = 100.0 - (((double) sLoop + 1) * p2Step);

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
                                                       .arg(exp(provisionalValues.smallBruteStartingValueArray[0].p2))
                                                       .toUtf8().constData());
                }

                fitResultGreenMyerson = new FitResult(ModelType::Myerson);

                if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
                {
                    mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Myerson, mFittingObject->bicMyerson));

                    double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitMyersonK) : mFittingObject->fitMyersonK;

                    fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson K"), lnK));
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
        }

        if (settings->modelRachlin)
        {
            if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mFittingObject->FitRachlin(NULL);

                fitResultRachlin = new FitResult(ModelType::Rachlin);

                if (settings->cbRachlin && exp(mFittingObject->fitRachlinS) > 1)
                {
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

                    double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitRachlinK) : mFittingObject->fitRachlinK;

                    fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin K"), lnK));
                    fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin S"), mFittingObject->fitRachlinS));

                    fitResultRachlin->RMS = mFittingObject->rmseRachlin;

                    fitResultRachlin->AIC = mFittingObject->aicRachlin;
                    fitResultRachlin->BIC = mFittingObject->bicRachlin;

                    for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                    {
                        fitResultRachlin->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                    }

                    fitResultRachlin->Status = "---";
                }
            }
            else
            {
                if (!settings->cbBruteForce)
                {
                    p1Span = abs(-12) + abs(12);
                    p1Step = (double) p1Span / 10.0;

                    p2Span = abs(-20) + abs(20);
                    p2Step = (double) p2Span / 100.0;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 10; kLoop++)
                    {
                        for (int sLoop = 0; sLoop < 100; sLoop++)
                        {
                            provisionalValues.twoParamStartingValueArray[grandLoop].p1 = 12.0 - (((double) kLoop + 1) * p1Step);
                            provisionalValues.twoParamStartingValueArray[grandLoop].p2 = 20.0 - (((double) sLoop + 1) * p2Step);

                            grandLoop++;
                        }
                    }

                    for(BruteForce & obj : provisionalValues.twoParamStartingValueArray)
                    {
                        obj.err = mFittingObject->getErrorRachlin(obj.p1, obj.p2);
                    }

                    std::sort(provisionalValues.twoParamStartingValueArray, provisionalValues.twoParamStartingValueArray + 1000);

                    mFittingObject->FitRachlin(QString("[%1,%2]")
                                                       .arg(provisionalValues.twoParamStartingValueArray[0].p1)
                                                       .arg(exp(provisionalValues.twoParamStartingValueArray[0].p2))
                                                       .toUtf8().constData());
                }
                else
                {
                    p1Span = abs(-100) + abs(100);
                    p1Step = (double) p1Span / 100.0;

                    p2Span = abs(-100) + abs(100);
                    p2Step = (double) p2Span / 100.0;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 100; kLoop++)
                    {
                        for (int sLoop = 0; sLoop < 100; sLoop++)
                        {
                            provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 100 - (((double) kLoop + 1) * p1Step);
                            provisionalValues.smallBruteStartingValueArray[grandLoop].p2 = 100 - (((double) sLoop + 1) * p2Step);

                            grandLoop++;
                        }
                    }

                    for(BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
                    {
                        obj.err = mFittingObject->getErrorRachlin(obj.p1, obj.p2);
                    }

                    std::sort(provisionalValues.smallBruteStartingValueArray, provisionalValues.smallBruteStartingValueArray + 10000);

                    mFittingObject->FitRachlin(QString("[%1,%2]")
                                                       .arg(provisionalValues.smallBruteStartingValueArray[0].p1)
                                                       .arg(exp(provisionalValues.smallBruteStartingValueArray[0].p2))
                                                       .toUtf8().constData());
                }

                fitResultRachlin = new FitResult(ModelType::Rachlin);

                if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
                {
                    if (settings->cbRachlin && mFittingObject->GetParams()[1] > 1)
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

                        double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitRachlinK) : mFittingObject->fitRachlinK;

                        fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin K"), lnK));
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
        }

        if (settings->modelGeneralizedHyperbolic)
        {
            if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mFittingObject->FitGeneralizedHyperbolic(NULL);

                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::GeneralizedHyperbolic, mFittingObject->bicGeneralizedHyperbolic));

                fitResultGeneralizedHyperbolic = new FitResult(ModelType::GeneralizedHyperbolic);

                double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitGeneralizedHyperbolicK) : mFittingObject->fitGeneralizedHyperbolicK;
                double lnB = (settings->logNormalParameters) ? exp(mFittingObject->fitGeneralizedHyperbolicBeta) : mFittingObject->fitGeneralizedHyperbolicBeta;

                fitResultGeneralizedHyperbolic->Params.append(QPair<QString, double>(QString("Generalized-Hyperbolic K"), lnK));
                fitResultGeneralizedHyperbolic->Params.append(QPair<QString, double>(QString("Generalized-Hyperbolic Beta"), lnB));

                fitResultGeneralizedHyperbolic->RMS = mFittingObject->rmseGeneralizedHyperbolic;
                fitResultGeneralizedHyperbolic->AIC = mFittingObject->aicGeneralizedHyperbolic;
                fitResultGeneralizedHyperbolic->BIC = mFittingObject->bicGeneralizedHyperbolic;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultGeneralizedHyperbolic->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

                fitResultGeneralizedHyperbolic->Status = "---";
            }
            else
            {
                if (!settings->cbBruteForce)
                {
                    p1Span = abs(-12) + abs(12); // -12 to 12
                    p1Step = (double) p1Span / 10;

                    p2Span = abs(-12) + abs(12); // -12 to 12
                    p2Step = (double) p2Span / 100;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 10; kLoop++)
                    {
                        for (int sLoop = 0; sLoop < 100; sLoop++)
                        {
                            provisionalValues.twoParamStartingValueArray[grandLoop].p1 = 12 - (((double) kLoop + 1) * p1Step);
                            provisionalValues.twoParamStartingValueArray[grandLoop].p2 = 12 - (((double) sLoop + 1) * p2Step);

                            grandLoop++;
                        }
                    }

                    for(BruteForce & obj : provisionalValues.twoParamStartingValueArray)
                    {
                        obj.err = mFittingObject->getErrorGeneralizedHyperbolic(obj.p1, obj.p2);
                    }

                    std::sort(provisionalValues.twoParamStartingValueArray, provisionalValues.twoParamStartingValueArray + 1000);

                    mFittingObject->FitGeneralizedHyperbolic(QString("[%1,%2]")
                                                       .arg(provisionalValues.twoParamStartingValueArray[0].p1)
                                                       .arg(exp(provisionalValues.twoParamStartingValueArray[0].p2))
                                                       .toUtf8().constData());
                }
                else
                {
                    p1Span = abs(-100) + abs(100);
                    p1Step = (double) p1Span / 100;

                    p2Span = abs(-100) + abs(100);
                    p2Step = (double) p2Span / 100;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 100; kLoop++)
                    {
                        for (int sLoop = 0; sLoop < 100; sLoop++)
                        {
                            provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 100 - (((double) kLoop + 1) * p1Step);
                            provisionalValues.smallBruteStartingValueArray[grandLoop].p2 = 100 - (((double) sLoop + 1) * p2Step);

                            grandLoop++;
                        }
                    }

                    for(BruteForce & obj : provisionalValues.smallBruteStartingValueArray)
                    {
                        obj.err = mFittingObject->getErrorGeneralizedHyperbolic(obj.p1, obj.p2);
                    }

                    std::sort(provisionalValues.smallBruteStartingValueArray, provisionalValues.smallBruteStartingValueArray + 10000);

                    mFittingObject->FitGeneralizedHyperbolic(QString("[%1,%2]")
                                                       .arg(provisionalValues.smallBruteStartingValueArray[0].p1)
                                                       .arg(exp(provisionalValues.smallBruteStartingValueArray[0].p2))
                                                       .toUtf8().constData());
                }

                fitResultGeneralizedHyperbolic = new FitResult(ModelType::GeneralizedHyperbolic);

                if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
                {
                    mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::GeneralizedHyperbolic, mFittingObject->bicGeneralizedHyperbolic));

                    double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitGeneralizedHyperbolicK) : mFittingObject->fitGeneralizedHyperbolicK;
                    double lnB = (settings->logNormalParameters) ? exp(mFittingObject->fitGeneralizedHyperbolicBeta) : mFittingObject->fitGeneralizedHyperbolicBeta;

                    fitResultGeneralizedHyperbolic->Params.append(QPair<QString, double>(QString("Generalized-Hyperbolic K"), lnK));
                    fitResultGeneralizedHyperbolic->Params.append(QPair<QString, double>(QString("Generalized-Hyperbolic Beta"), lnB));
                    fitResultGeneralizedHyperbolic->RMS = mFittingObject->GetReport().rmserror;
                    fitResultGeneralizedHyperbolic->AIC = mFittingObject->aicGeneralizedHyperbolic;
                    fitResultGeneralizedHyperbolic->BIC = mFittingObject->bicGeneralizedHyperbolic;

                    for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                    {
                        fitResultGeneralizedHyperbolic->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                    }

                    fitResultGeneralizedHyperbolic->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
                }
                else
                {
                    fitResultGeneralizedHyperbolic->Params.append(QPair<QString, double>(QString("Generalized-Hyperbolic K"), NULL));
                    fitResultGeneralizedHyperbolic->Params.append(QPair<QString, double>(QString("Generalized-Hyperbolic Beta"), NULL));
                    fitResultGeneralizedHyperbolic->RMS = NULL;
                    fitResultGeneralizedHyperbolic->AIC = NULL;
                    fitResultGeneralizedHyperbolic->BIC = NULL;
                    fitResultGeneralizedHyperbolic->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
                }
            }
        }

        if (settings->modelEbertPrelec)
        {
            if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mFittingObject->FitEbertPrelec(NULL);

                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::EbertPrelec, mFittingObject->bicEbertPrelec));

                fitResultEbertPrelec = new FitResult(ModelType::EbertPrelec);

                double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitEbertPrelecK) : mFittingObject->fitEbertPrelecK;

                fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec K"), lnK));
                fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec S"), mFittingObject->fitEbertPrelecS));

                fitResultEbertPrelec->RMS = mFittingObject->rmseEbertPrelec;

                fitResultEbertPrelec->AIC = mFittingObject->aicEbertPrelec;
                fitResultEbertPrelec->BIC = mFittingObject->bicEbertPrelec;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultEbertPrelec->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

                fitResultEbertPrelec->Status = "---";
            }
            else
            {
                if (!settings->cbBruteForce)
                {
                    p1Span = abs(-12) + abs(12); // -12 to 12
                    p1Step = (double) p1Span / 10;

                    p2Span = abs(-12) + abs(12); // -12 to 12
                    p2Step = (double) p2Span / 100;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 10; kLoop++)
                    {
                        for (int sLoop = 0; sLoop < 100; sLoop++)
                        {
                            provisionalValues.twoParamStartingValueArray[grandLoop].p1 = 12 - (((double) kLoop + 1) * p1Step);
                            provisionalValues.twoParamStartingValueArray[grandLoop].p2 = 12 - (((double) sLoop + 1) * p2Step);

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
                                                       .arg(exp(provisionalValues.twoParamStartingValueArray[0].p2))
                                                       .toUtf8().constData());
                }
                else
                {
                    p1Span = abs(-100) + abs(100);
                    p1Step = (double) p1Span / 100;

                    p2Span = abs(-100) + abs(100);
                    p2Step = (double) p2Span / 100;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 100; kLoop++)
                    {
                        for (int sLoop = 0; sLoop < 100; sLoop++)
                        {
                            provisionalValues.smallBruteStartingValueArray[grandLoop].p1 = 100 - (((double) kLoop + 1) * p1Step);
                            provisionalValues.smallBruteStartingValueArray[grandLoop].p2 = 100 - (((double) sLoop + 1) * p2Step);

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
                                                       .arg(exp(provisionalValues.smallBruteStartingValueArray[0].p2))
                                                       .toUtf8().constData());
                }

                fitResultEbertPrelec = new FitResult(ModelType::EbertPrelec);

                if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
                {
                    mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::EbertPrelec, mFittingObject->bicEbertPrelec));

                    double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitEbertPrelecK) : mFittingObject->fitEbertPrelecK;

                    fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec K"), lnK));
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
        }

        if (settings->modelBleichrodt)
        {
            if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mFittingObject->FitBleichrodt(NULL);

                mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Beleichrodt, mFittingObject->bicBleichrodt));

                fitResultBleichrodt = new FitResult(ModelType::Beleichrodt);

                double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitBleichrodtK) : mFittingObject->fitBleichrodtK;

                fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt K"), lnK));
                fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt S"), mFittingObject->fitBleichrodtS));
                fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt Beta"), mFittingObject->fitBleichrodtBeta));

                fitResultBleichrodt->RMS = mFittingObject->rmseBleichrodt;
                fitResultBleichrodt->AIC = mFittingObject->aicBleichrodt;
                fitResultBleichrodt->BIC = mFittingObject->bicBleichrodt;

                for (int v = 0; v < mFittingObject->ErrorResidual.count(); v++)
                {
                    fitResultBleichrodt->ErrPar.append(mFittingObject->ErrorResidual.at(v));
                }

                fitResultBleichrodt->Status = "---";
            }
            else
            {
                if (!settings->cbBruteForce)
                {
                    p1Span = abs(-12) + abs(12);
                    p1Step = (double) p1Span / 10;

                    p2Span = abs(-12) + abs(12);
                    p2Step = (double) p2Span / 10;

                    p3Span = 1; // .1 to 1
                    p3Step = (double) p3Span / 10;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 10; kLoop++)
                    {
                        for (int sLoop = 0; sLoop < 10; sLoop++)
                        {
                            for (int bLoop = 0; bLoop < 10; bLoop++)
                            {
                                provisionalValues.threeParamStartingValueArray[grandLoop].p1 = 12 - (((double) kLoop + 1) * p1Step);
                                provisionalValues.threeParamStartingValueArray[grandLoop].p2 = 12 - (((double) sLoop + 1) * p2Step);
                                provisionalValues.threeParamStartingValueArray[grandLoop].p3 = (((double) bLoop + 1) * p3Step);

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
                                                       .arg(exp(provisionalValues.threeParamStartingValueArray[0].p2))
                                                       .arg(provisionalValues.threeParamStartingValueArray[0].p3)
                                                       .toUtf8().constData());
                }
                else
                {
                    p1Span = abs(-100) + abs(100);
                    p1Step = (double) p1Span / 100;

                    p2Span = abs(-100) + abs(100);
                    p2Step = (double) p2Span / 100;

                    p3Span = 1;
                    p3Step = (double) p3Span / 100;

                    grandLoop = 0;

                    for (int kLoop = 0; kLoop < 100; kLoop++)
                    {
                        for (int sLoop = 0; sLoop < 100; sLoop++)
                        {
                            for (int bLoop = 0; bLoop < 100; bLoop++)
                            {
                                provisionalValues.largeBruteStartingValueArray[grandLoop].p1 = 100 - (((double) kLoop + 1) * p1Step);
                                provisionalValues.largeBruteStartingValueArray[grandLoop].p2 = 100 - (((double) sLoop + 1) * p2Step);
                                provisionalValues.largeBruteStartingValueArray[grandLoop].p3 = (((double) bLoop + 1) * p3Step);

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
                                                       .arg(exp(provisionalValues.largeBruteStartingValueArray[0].p2))
                                                       .arg(provisionalValues.largeBruteStartingValueArray[0].p3)
                                                       .toUtf8().constData());
                }

                fitResultBleichrodt = new FitResult(ModelType::Beleichrodt);

                if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
                {
                    mFittingObject->mBicList.append(QPair<ModelType, double>(ModelType::Beleichrodt, mFittingObject->bicBleichrodt));

                    double lnK = (settings->logNormalParameters) ? exp(mFittingObject->fitBleichrodtK) : mFittingObject->fitBleichrodtK;

                    fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt K"), lnK));
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

        fitResults->FittingResults.clear();

        fitResultNoise->Status = QString("---");

        mFittingObject->PrepareProbabilities();

        std::sort(mFittingObject->mProbList.begin(),
                  mFittingObject->mProbList.end(),
                  QPairSecondComparer());

        fitResultNoise->BF = mFittingObject->bfNoise;
        fitResultNoise->Probability = mFittingObject->probsNoise;
            fitResults->FittingResults.append(fitResultNoise);

        if (settings->modelExponential)
        {
            fitResultExponential->BF = mFittingObject->bfExponential;
            fitResultExponential->Probability = mFittingObject->probsExponential;
                fitResults->FittingResults.append(fitResultExponential);
        }

        if (settings->modelParabolic)
        {
            fitResultParabolic->BF = mFittingObject->bfParabolic;
            fitResultParabolic->Probability = mFittingObject->probsParabolic;
                fitResults->FittingResults.append(fitResultParabolic);
        }

        if (settings->modelHyperbolic)
        {
            fitResultHyperbolic->BF = mFittingObject->bfHyperbolic;
            fitResultHyperbolic->Probability = mFittingObject->probsHyperbolic;
                fitResults->FittingResults.append(fitResultHyperbolic);
        }

        if (settings->modelQuasiHyperbolic)
        {
            fitResultBetaDelta->BF = mFittingObject->bfQuasiHyperbolic;
            fitResultBetaDelta->Probability = mFittingObject->probsQuasiHyperbolic;
                fitResults->FittingResults.append(fitResultBetaDelta);
        }

        if (settings->modelMyersonGreen)
        {
            fitResultGreenMyerson->BF = mFittingObject->bfMyerson;
            fitResultGreenMyerson->Probability = mFittingObject->probsMyerson;
                fitResults->FittingResults.append(fitResultGreenMyerson);
        }

        if (settings->modelRachlin)
        {
            fitResultRachlin->BF = mFittingObject->bfRachlin;
            fitResultRachlin->Probability = mFittingObject->probsRachlin;
                fitResults->FittingResults.append(fitResultRachlin);
        }

        if (settings->modelGeneralizedHyperbolic)
        {
            fitResultGeneralizedHyperbolic->BF = mFittingObject->bfGeneralizedHyperbolic;
            fitResultGeneralizedHyperbolic->Probability = mFittingObject->probsGeneralizedHyperbolic;
                fitResults->FittingResults.append(fitResultGeneralizedHyperbolic);
        }

        if (settings->modelEbertPrelec)
        {
            fitResultEbertPrelec->BF = mFittingObject->bfEbertPrelec;
            fitResultEbertPrelec->Probability = mFittingObject->probsEbertPrelec;
                fitResults->FittingResults.append(fitResultEbertPrelec);
        }

        if (settings->modelBleichrodt)
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

            case ModelType::Parabolic:
                mModel = "Parabolic";
                mTopErrPar = fitResultParabolic->ErrPar;
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

            case ModelType::GeneralizedHyperbolic:
                mModel = "Generalized-Hyperbolic";
                mTopErrPar = fitResultGeneralizedHyperbolic->ErrPar;
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

        if (mModel.contains("Generalized-Hyperbolic", Qt::CaseInsensitive))
        {
            if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mModel.append(" (de)");
            }
            else if (settings->settingsFitting == FittingAlgorithm::Function)
            {
                mModel.append(" (f)");
            }
            else
            {
                mModel.append(" (fg)");
            }
        }
        else if (!mModel.contains("Noise", Qt::CaseInsensitive))
        {
            if (settings->settingsFitting == FittingAlgorithm::Function)
            {
                mModel.append(" (f)");
            }
            else if (settings->settingsFitting == FittingAlgorithm::FunctionGradient)
            {
                mModel.append(" (fg)");
            }
            else if (settings->settingsFitting == FittingAlgorithm::FunctionGradientHessian)
            {
                mModel.append(" (fgh)");
            }
            else if (settings->settingsFitting == FittingAlgorithm::DifferentialEvolution)
            {
                mModel.append(" (de)");
            }
        }

        fitResults->TopModel = mModel;
        fitResults->TopModelType = mFittingObject->mProbList.first().first;
        fitResults->TopErrPar = mTopErrPar;

        fitResults->ParticipantDelays = tempList[2];
        fitResults->ParticipantValues = tempList[3];

        fitResults->TopED50 = mFittingObject->getED50BestModel(mFittingObject->mProbList.first().first);

        if (settings->cbArea)
        {
            fitResults->TopAUC = mFittingObject->getAUCBestModel(mFittingObject->mProbList.first().first);
            fitResults->TopAUCLog = mFittingObject->getLogAUCBestModel(mFittingObject->mProbList.first().first);
        }

        emit workingResult(*fitResults);
    }

    emit workFinished();
}
