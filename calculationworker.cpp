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
            mFittingObject->FitExponential("[-5]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Exponential", mFittingObject->bicExponential));
            }

            fitResultExponential = new FitResult(ModelType::Exponential);

            double lnK = (runLogarithmicResults) ? exp(mFittingObject->fitExponentialK) : mFittingObject->fitExponentialK;
            fitResultExponential->Params.append(QPair<QString, double>(QString("Exponential K"), lnK));
            fitResultExponential->RMS = mFittingObject->GetReport().rmserror;
            fitResultExponential->AIC = mFittingObject->aicExponential;
            fitResultExponential->BIC = mFittingObject->bicExponential;
            fitResultExponential->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
        }

        if (runLocalHyperbolic)
        {
            mFittingObject->FitHyperbolic("[-5]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Hyperbolic", mFittingObject->bicHyperbolic));
            }

            fitResultHyperbolic = new FitResult(ModelType::Hyperbolic);

            double lnK = (runLogarithmicResults) ? exp(mFittingObject->fitHyperbolicK) : mFittingObject->fitHyperbolicK;
            fitResultHyperbolic->Params.append(QPair<QString, double>(QString("Hyperbolic K"), lnK));
            fitResultHyperbolic->RMS = mFittingObject->GetReport().rmserror;
            fitResultHyperbolic->AIC = mFittingObject->aicHyperbolic;
            fitResultHyperbolic->BIC = mFittingObject->bicHyperbolic;
            fitResultHyperbolic->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
        }

        if (runLocalBetaDelta)
        {
            mFittingObject->FitQuasiHyperbolic("[0.3, 0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Beta Delta", mFittingObject->bicQuasiHyperbolic));
            }

            fitResultBetaDelta = new FitResult(ModelType::BetaDelta);

            fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Beta"), mFittingObject->fitQuasiHyperbolicBeta));
            fitResultBetaDelta->Params.append(QPair<QString, double>(QString("BetaDelta Delta"), mFittingObject->fitQuasiHyperbolicDelta));
            fitResultBetaDelta->RMS = mFittingObject->GetReport().rmserror;
            fitResultBetaDelta->AIC = mFittingObject->aicQuasiHyperbolic;
            fitResultBetaDelta->BIC = mFittingObject->bicQuasiHyperbolic;
            fitResultBetaDelta->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
        }

        if (runLocalMyersonGreen)
        {
            mFittingObject->FitMyerson("[-5, 0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Myerson Hyperbola", mFittingObject->bicMyerson));
            }

            fitResultGreenMyerson = new FitResult(ModelType::Myerson);

            fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson K"), mFittingObject->fitMyersonK));
            fitResultGreenMyerson->Params.append(QPair<QString, double>(QString("Myerson S"), mFittingObject->fitMyersonS));
            fitResultGreenMyerson->RMS = mFittingObject->GetReport().rmserror;
            fitResultGreenMyerson->AIC = mFittingObject->aicMyerson;
            fitResultGreenMyerson->BIC = mFittingObject->bicMyerson;
            fitResultGreenMyerson->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
        }

        if (runLocalRachlin)
        {
            mFittingObject->FitRachlin("[-5, 0.3]");

            if (boundRachlinModel && mFittingObject->GetParams()[1] > 1)
            {
                fitResultRachlin = new FitResult(ModelType::Rachlin);

                fitResultRachlin->Status = "Exceeded Bounds";
            }
            else
            {
                if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
                {
                    mFittingObject->mBicList.append(QPair<QString, double>("Rachlin Hyperbola", mFittingObject->bicRachlin));
                }

                fitResultRachlin = new FitResult(ModelType::Rachlin);

                fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin K"), mFittingObject->fitRachlinK));
                fitResultRachlin->Params.append(QPair<QString, double>(QString("Rachlin S"), mFittingObject->fitRachlinS));
                fitResultRachlin->RMS = mFittingObject->GetReport().rmserror;
                fitResultRachlin->AIC = mFittingObject->aicRachlin;
                fitResultRachlin->BIC = mFittingObject->bicRachlin;
                fitResultRachlin->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
        }

        if (runLocalRodriguezLogue)
        {
            mFittingObject->FitRodriguezLogue("[-5, 0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Rodriguez-Logue", mFittingObject->bicRodriguezLogue));
            }

            fitResultRodriguezLogue = new FitResult(ModelType::RodriguezLogue);

            fitResultRodriguezLogue->Params.append(QPair<QString, double>(QString("Rodriguez-Logue K"), mFittingObject->fitRodriguezLogueK));
            fitResultRodriguezLogue->Params.append(QPair<QString, double>(QString("Rodriguez-Logue Beta"), mFittingObject->fitRodriguezLogueBeta));
            fitResultRodriguezLogue->RMS = mFittingObject->GetReport().rmserror;
            fitResultRodriguezLogue->AIC = mFittingObject->aicRodriguezLogue;
            fitResultRodriguezLogue->BIC = mFittingObject->bicRodriguezLogue;
            fitResultRodriguezLogue->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
        }

        if (runLocalEbertPrelec)
        {
            mFittingObject->FitEbertPrelec("[-5.0, 0.5]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Ebert-Prelec", mFittingObject->bicEbertPrelec));
            }

            fitResultEbertPrelec = new FitResult(ModelType::EbertPrelec);

            fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec K"), mFittingObject->fitEbertPrelecK));
            fitResultEbertPrelec->Params.append(QPair<QString, double>(QString("Ebert-Prelec S"), mFittingObject->fitEbertPrelecS));
            fitResultEbertPrelec->RMS = mFittingObject->GetReport().rmserror;
            fitResultEbertPrelec->AIC = mFittingObject->aicEbertPrelec;
            fitResultEbertPrelec->BIC = mFittingObject->bicEbertPrelec;
            fitResultEbertPrelec->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
        }

        if (runLocalBleicholdt)
        {
            mFittingObject->FitBleichrodt("[-5.0, 0.5, 0.5]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Bleichrodt", mFittingObject->bicBleichrodt));
            }

            fitResultBleichrodt = new FitResult(ModelType::Beleichrodt);

            fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt K"), mFittingObject->fitBleichrodtK));
            fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt S"), mFittingObject->fitBleichrodtS));
            fitResultBleichrodt->Params.append(QPair<QString, double>(QString("Bleichrodt Beta"), mFittingObject->fitBleichrodtBeta));
            fitResultBleichrodt->RMS = mFittingObject->GetReport().rmserror;
            fitResultBleichrodt->AIC = mFittingObject->aicBleichrodt;
            fitResultBleichrodt->BIC = mFittingObject->bicBleichrodt;
            fitResultBleichrodt->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
        }

        mFittingObject->FitNoise();
        mFittingObject->NoiseBIC = mFittingObject->bicNoise;

        fitResultNoise = new FitResult(ModelType::Noise);
        fitResultNoise->Params.append(QPair<QString, double>(QString("Noise mean"), mFittingObject->GetNoiseMean()));
        fitResultNoise->RMS = mFittingObject->GetReport().rmserror;
        fitResultNoise->AIC = mFittingObject->aicExponential;
        fitResultNoise->BIC = mFittingObject->bicExponential;
        fitResultNoise->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());

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

        QString model = mFittingObject->mProbList.first().first;

        fitResults->TopModel = model;

        fitResults->ParticipantDelays = tempList[2];
        fitResults->ParticipantValues = tempList[3];

        fitResults->TopED50 = mFittingObject->getED50BestModel(model);
        fitResults->TopAUC = mFittingObject->getAUCBestModel(model);

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
