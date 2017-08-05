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
            mFittingObject->FitExponential("[0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Exponential", mFittingObject->bicExponential));
            }

            fitResult = new FitResult(ModelType::Exponential);

            double lnK = (runLogarithmicResults) ? exp(mFittingObject->fitExponentialK) : mFittingObject->fitExponentialK;
            fitResult->Params.append(QPair<QString, double>(QString("Exponential K"), lnK));
            fitResult->RMS = mFittingObject->GetReport().rmserror;
            fitResult->AIC = mFittingObject->aicExponential;
            fitResult->BIC = mFittingObject->bicExponential;
            fitResult->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());

            fitResults->FittingResults.append(fitResult);

        }

        if (runLocalHyperbolic)
        {
            mFittingObject->FitHyperbolic("[0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Hyperbolic", mFittingObject->bicHyperbolic));
            }

            fitResult = new FitResult(ModelType::Hyperbolic);

            double lnK = (runLogarithmicResults) ? exp(mFittingObject->fitHyperbolicK) : mFittingObject->fitHyperbolicK;
            fitResult->Params.append(QPair<QString, double>(QString("Hyperbolic K"), lnK));
            fitResult->RMS = mFittingObject->GetReport().rmserror;
            fitResult->AIC = mFittingObject->bicHyperbolic;
            fitResult->BIC = mFittingObject->aicHyperbolic;
            fitResult->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());

            fitResults->FittingResults.append(fitResult);
        }

        if (runLocalBetaDelta)
        {
            mFittingObject->FitQuasiHyperbolic("[0.3, 0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Beta Delta", mFittingObject->bicQuasiHyperbolic));
            }

            fitResult = new FitResult(ModelType::BetaDelta);

            fitResult->Params.append(QPair<QString, double>(QString("BetaDelta Beta"), mFittingObject->fitQuasiHyperbolicBeta));
            fitResult->Params.append(QPair<QString, double>(QString("BetaDelta Delta"), mFittingObject->fitQuasiHyperbolicDelta));
            fitResult->RMS = mFittingObject->GetReport().rmserror;
            fitResult->AIC = mFittingObject->bicQuasiHyperbolic;
            fitResult->BIC = mFittingObject->aicQuasiHyperbolic;
            fitResult->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());

            fitResults->FittingResults.append(fitResult);
        }

        if (runLocalMyersonGreen)
        {
            mFittingObject->FitMyerson("[0.3, 0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Myerson Hyperbola", mFittingObject->bicMyerson));
            }

            fitResult = new FitResult(ModelType::Myerson);

            fitResult->Params.append(QPair<QString, double>(QString("Myerson K"), mFittingObject->fitMyersonK));
            fitResult->Params.append(QPair<QString, double>(QString("Myerson S"), mFittingObject->fitMyersonS));
            fitResult->RMS = mFittingObject->GetReport().rmserror;
            fitResult->AIC = mFittingObject->bicMyerson;
            fitResult->BIC = mFittingObject->aicMyerson;
            fitResult->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());

            fitResults->FittingResults.append(fitResult);
        }

        if (runLocalRachlin)
        {
            mFittingObject->FitRachlin("[0.3, 0.3]");

            if (boundRachlinModel && mFittingObject->GetParams()[1] > 1)
            {
                fitResult = new FitResult(ModelType::Rachlin);

                fitResult->Status = "Exceeded Bounds";

                fitResults->FittingResults.append(fitResult);
            }
            else
            {
                if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
                {
                    mFittingObject->mBicList.append(QPair<QString, double>("Rachlin Hyperbola", mFittingObject->bicRachlin));
                }

                fitResult = new FitResult(ModelType::Rachlin);

                fitResult->Params.append(QPair<QString, double>(QString("Rachlin K"), mFittingObject->fitRachlinK));
                fitResult->Params.append(QPair<QString, double>(QString("Rachlin S"), mFittingObject->fitRachlinS));
                fitResult->RMS = mFittingObject->GetReport().rmserror;
                fitResult->AIC = mFittingObject->bicRachlin;
                fitResult->BIC = mFittingObject->aicRachlin;
                fitResult->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());

                fitResults->FittingResults.append(fitResult);
            }
        }

        mFittingObject->FitNoise();
        mFittingObject->NoiseBIC = mFittingObject->bicNoise;

        fitResult = new FitResult(ModelType::Noise);
        fitResult->Params.append(QPair<QString, double>(QString("Noise mean"), mFittingObject->GetNoiseMean()));
        fitResult->RMS = mFittingObject->GetReport().rmserror;
        fitResult->AIC = mFittingObject->aicExponential;
        fitResult->BIC = mFittingObject->bicExponential;
        fitResult->Status = mFittingObject->formatStringResult((int) mFittingObject->GetInfo());

        fitResults->FittingResults.append(fitResult);

        mFittingObject->PrepareProbabilities();

        qSort(mFittingObject->mProbList.begin(), mFittingObject->mProbList.end(), QPairSecondComparer());

        for (int i=0; i<fitResults->FittingResults.length(); i++)
        {
            if (fitResults->FittingResults[i]->Model == ModelType::Noise)
            {
                fitResults->FittingResults[i]->BF = mFittingObject->bfNoise;
                fitResults->FittingResults[i]->Probability = mFittingObject->probsNoise;
            }

            if (fitResults->FittingResults[i]->Model == ModelType::Exponential)
            {
                fitResults->FittingResults[i]->BF = mFittingObject->bfExponential;
                fitResults->FittingResults[i]->Probability = mFittingObject->probsExponential;
            }

            if (fitResults->FittingResults[i]->Model == ModelType::Hyperbolic)
            {
                fitResults->FittingResults[i]->BF = mFittingObject->bfHyperbolic;
                fitResults->FittingResults[i]->Probability = mFittingObject->probsHyperbolic;
            }

            if (fitResults->FittingResults[i]->Model == ModelType::BetaDelta)
            {
                fitResults->FittingResults[i]->BF = mFittingObject->bfQuasiHyperbolic;
                fitResults->FittingResults[i]->Probability = mFittingObject->probsQuasiHyperbolic;
            }

            if (fitResults->FittingResults[i]->Model == ModelType::Myerson)
            {
                fitResults->FittingResults[i]->BF = mFittingObject->bfMyerson;
                fitResults->FittingResults[i]->Probability = mFittingObject->probsMyerson;
            }

            if (fitResults->FittingResults[i]->Model == ModelType::Rachlin)
            {
                fitResults->FittingResults[i]->BF = mFittingObject->bfRachlin;
                fitResults->FittingResults[i]->Probability = mFittingObject->probsRachlin;
            }
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
