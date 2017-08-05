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

/*CalculationWorker::CalculationWorker(QList<QPair<QString, QString> > mJohnsonBickelResults, QList<bool> *mJonhsonBickelSelections, QList<QStringList> mStoredValues, bool runHyperbolic, bool runExponential, bool runBetaDelta, bool runMyersonGreen, bool runRachlin, bool runLogResults, bool boundRachlin, QString computationType, int processChecking)*/
CalculationWorker::CalculationWorker(QList<QPair<QString, QString> > mJohnsonBickelResults, QList<bool> *mJonhsonBickelSelections, QList<QStringList> mStoredValues, CalculationSettings *calculationSettings, int processChecking)
{
    computationTypeLocal = calculationSettings->scriptName;

    mLocalJohnsonBickelResults = mJohnsonBickelResults;
    mLocalJonhsonBickelSelections = mJonhsonBickelSelections;
    mLocalStoredValues = mStoredValues;

    mFittingObject = new ModelSelection();

    runLocalHyperbolic =  calculationSettings->modelHyperbolic;
    runLocalExponential = calculationSettings->modelExponential;
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
    QStringList resultsList;
    QStringList tempList;

    for (int i = 0; i < mLocalStoredValues.count(); i++)
    {
        resultsList.clear();

        resultsList << QString::number(i+1);

        if (processCheckingLocal == 1)
        {
            if (mLocalJohnsonBickelResults[i].first.contains("Fail", Qt::CaseInsensitive) || mLocalJohnsonBickelResults[i].second.contains("Fail", Qt::CaseInsensitive))
            {
                resultsList[0] = resultsList[0] + " (Dropped)";

                emit workingResult(resultsList);

                continue;
            }
        }
        else if (processCheckingLocal == 2)
        {
            if (!mLocalJonhsonBickelSelections->at(i))
            {
                resultsList[0] = resultsList[0] + " (Dropped)";

                emit workingResult(resultsList);

                continue;
            }
        }

        tempList = mLocalStoredValues[i];

        mFittingObject->SetX(tempList[0].toUtf8().constData());
        mFittingObject->SetY(tempList[1].toUtf8().constData());
        mFittingObject->mBicList.clear();

        if (runLocalHyperbolic)
        {
            mFittingObject->FitHyperbolic("[0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Hyperbolic", mFittingObject->bicHyperbolic));
            }

            resultsList << formatStringResult(mFittingObject->fitHyperbolicK, runLogarithmicResults);
            resultsList << QString::number(mFittingObject->GetReport().rmserror);
            resultsList << QString::number(mFittingObject->bicHyperbolic);
            resultsList << QString::number(mFittingObject->aicHyperbolic);
            resultsList << "";
            resultsList << "";
            resultsList << mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
        }
        else
        {
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
        }

        if (runLocalExponential)
        {
            mFittingObject->FitExponential("[0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Exponential", mFittingObject->bicExponential));
            }

            resultsList << formatStringResult(mFittingObject->fitExponentialK, runLogarithmicResults);
            resultsList << QString::number(mFittingObject->GetReport().rmserror);
            resultsList << QString::number(mFittingObject->bicExponential);
            resultsList << QString::number(mFittingObject->aicExponential);
            resultsList << "";
            resultsList << "";
            resultsList << mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
        }
        else
        {
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
        }

        if (runLocalBetaDelta)
        {
            mFittingObject->FitQuasiHyperbolic("[0.3, 0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Beta Delta", mFittingObject->bicQuasiHyperbolic));
            }

            resultsList << QString::number(mFittingObject->fitQuasiHyperbolicBeta);
            resultsList << QString::number(mFittingObject->fitQuasiHyperbolicDelta);
            resultsList << QString::number(mFittingObject->GetReport().rmserror);
            resultsList << QString::number(mFittingObject->bicQuasiHyperbolic);
            resultsList << QString::number(mFittingObject->aicQuasiHyperbolic);
            resultsList << "";
            resultsList << "";
            resultsList << mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
        }
        else
        {
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
        }

        if (runLocalMyersonGreen)
        {
            mFittingObject->FitMyerson("[0.3, 0.3]");

            if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
            {
                mFittingObject->mBicList.append(QPair<QString, double>("Myerson Hyperbola", mFittingObject->bicMyerson));

                resultsList << formatStringResult(mFittingObject->fitMyersonK, runLogarithmicResults);
                resultsList << QString::number(mFittingObject->fitMyersonS);
                resultsList << QString::number(mFittingObject->GetReport().rmserror);
                resultsList << QString::number(mFittingObject->bicMyerson);
                resultsList << QString::number(mFittingObject->aicMyerson);
                resultsList << "";
                resultsList << "";
                resultsList << mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
            else
            {
                resultsList << "---";
                resultsList << "---";
                resultsList << "---";
                resultsList << "---";
                resultsList << "---";
                resultsList << "---";
                resultsList << "---";
                resultsList << "---";
            }
        }
        else
        {
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
        }

        if (runLocalRachlin)
        {
            mFittingObject->FitRachlin("[0.3, 0.3]");

            if (boundRachlinModel && mFittingObject->GetParams()[1] > 1)
            {
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
                resultsList << "Exceeded Bounds";
            }
            else
            {
                if ((int) mFittingObject->GetInfo() == 2 || (int) mFittingObject->GetInfo() == 5)
                {
                    mFittingObject->mBicList.append(QPair<QString, double>("Rachlin Hyperbola", mFittingObject->bicRachlin));
                }

                resultsList << formatStringResult(mFittingObject->fitRachlinK, runLogarithmicResults);
                resultsList << QString::number(mFittingObject->fitRachlinS);
                resultsList << QString::number(mFittingObject->GetReport().rmserror);
                resultsList << QString::number(mFittingObject->bicRachlin);
                resultsList << QString::number(mFittingObject->aicRachlin);
                resultsList << "";
                resultsList << "";
                resultsList << mFittingObject->formatStringResult((int) mFittingObject->GetInfo());
            }
        }
        else
        {
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
            resultsList << "---";
        }


        mFittingObject->FitNoise();
        mFittingObject->NoiseBIC = mFittingObject->bicNoise;

        resultsList << QString::number(mFittingObject->GetNoiseMean());
        resultsList << QString::number(mFittingObject->GetReport().rmserror);
        resultsList << QString::number(mFittingObject->bicNoise);
        resultsList << QString::number(mFittingObject->aicNoise);
        resultsList << "";
        resultsList << "";

        mFittingObject->PrepareProbabilities();

        qSort(mFittingObject->mProbList.begin(), mFittingObject->mProbList.end(), QPairSecondComparer());

        if (runLocalHyperbolic)
        {
            resultsList[5] = QString::number(mFittingObject->bfHyperbolic);
            resultsList[6] = QString::number(mFittingObject->probsHyperbolic);
        }

        if (runLocalExponential)
        {
            resultsList[12] = QString::number(mFittingObject->bfExponential);
            resultsList[13] = QString::number(mFittingObject->probsExponential);
        }

        if (runLocalBetaDelta)
        {
            resultsList[20] = QString::number(mFittingObject->bfQuasiHyperbolic);
            resultsList[21] = QString::number(mFittingObject->probsQuasiHyperbolic);
        }

        if (runLocalMyersonGreen)
        {
            resultsList[28] = QString::number(mFittingObject->bfMyerson);
            resultsList[29] = QString::number(mFittingObject->probsMyerson);
        }

        if (runLocalRachlin)
        {
            resultsList[36] = QString::number(mFittingObject->bfRachlin);
            resultsList[37] = QString::number(mFittingObject->probsRachlin);
        }

        resultsList[43] = QString::number(mFittingObject->bfNoise);

        resultsList[44] = QString::number(mFittingObject->probsNoise);

        QString model = mFittingObject->mProbList.first().first;

        resultsList << model;

        resultsList << tempList[2];
        resultsList << tempList[3];

        if (computationTypeLocal.contains("Area", Qt::CaseInsensitive))
        {
            resultsList << mFittingObject->getAUCBestModel(model);

            QStringList mAllAUC = mFittingObject->getAUCAllModels();

            for (int i=0; i<mAllAUC.length(); i++)
            {
                resultsList << mAllAUC[i];
            }
        }
        else
        {
            resultsList << mFittingObject->getED50BestModel(model);
        }

        emit workingResult(resultsList);
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
