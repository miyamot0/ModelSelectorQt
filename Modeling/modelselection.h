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

#ifndef MODELSELECTION_H
#define MODELSELECTION_H

#include <QMap>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "interpolation.h"
#include "stdafx.h"
#include "optimization.h"

#include "Helpers/GeneticAlgorithms/evolutionfunctions.h"

#include "Models/fitresult.h"
#include "Models/calculationsettings.h"

#include "Libraries/differential-evolution/differentialevolution.h"

using namespace alglib;

class ModelSelection
{
public:
    ModelSelection();

    void SetX(const char *mString);
    void SetY(const char *mString);
    void SetStarts(const char *mString);
    void SetLowerUpperBounds(const char *mUpperString, const char *mLowerString);

    real_1d_array GetParams();
    lsfitstate GetState();
    ae_int_t GetInfo();
    lsfitreport GetReport();

    double GetNoiseMean();

    QString getED50BestModel(ModelType model);

    double getED50ep();
    double getED50parabolic();
    double getED50power();
    double getED50crdi();
    double getED50genhyp();

    FittingAlgorithm fittingAlgorithm;

    QString getAUCBestModel(ModelType model);
    QString getLogAUCBestModel(ModelType model);

    double getErrorExponential(double lnK, bool retainResiduals);
    double getErrorParabolic(double lnK, bool retainResiduals);
    double getErrorHyperbolic(double lnK, bool retainResiduals);
    double getErrorQuasiHyperbolic(double beta, double delta, bool retainResiduals);
    double getErrorGreenMyerson(double lnK, double s, bool retainResiduals);
    double getErrorRachlin(double lnK, double s, bool retainResiduals);
    double getErrorGeneralizedHyperbolic(double lnK, double beta, bool retainResiduals);
    double getErrorPower(double lnK, double lnS, bool retainResiduals);
    double getErrorEbertPrelec(double lnK, double s, bool retainResiduals);
    double getErrorBleichrodt(double lnK, double s, double beta, bool retainResiduals);

    void FitNoise();
    void FitExponential(const char *mStarts);
    void FitParabolic(const char *mStarts);
    void FitHyperbolic(const char *mStarts);
    void FitQuasiHyperbolic(const char *mStarts);
    void FitMyerson(const char *mStarts);
    void FitRachlin(const char *mStarts);
    void FitGeneralizedHyperbolic(const char *mStarts);
    void FitPower(const char *mStarts);
    void FitEbertPrelec(const char *mStarts);
    void FitBleichrodt(const char *mStarts);

    QList<QPair<ModelType, double>> mBicList;
    QList<QPair<ModelType, double>> mProbList;

    void PrepareProbabilities();

    /** AICs
     *
     */
    double aicNoise = -1;
    double aicHyperbolic = -1;
    double aicExponential = -1;
    double aicParabolic = -1;
    double aicQuasiHyperbolic = -1;
    double aicMyerson = -1;
    double aicRachlin = -1;
    double aicGeneralizedHyperbolic  = -1;
    double aicPower  = -1;
    double aicEbertPrelec  = -1;
    double aicBleichrodt  = -1;

    /** BICs
     *
     */
    double bicNoise = -1;
    double bicHyperbolic = -1;
    double bicExponential = -1;
    double bicParabolic = -1;
    double bicQuasiHyperbolic = -1;
    double bicMyerson = -1;
    double bicRachlin = -1;
    double bicGeneralizedHyperbolic = -1;
    double bicPower = -1;
    double bicEbertPrelec  = -1;
    double bicBleichrodt  = -1;

    /** Bayes Factors
      *
      */
    double bfNoise = -1;
    double bfHyperbolic = -1;
    double bfExponential = -1;
    double bfParabolic = -1;
    double bfQuasiHyperbolic = -1;
    double bfMyerson = -1;
    double bfRachlin = -1;
    double bfGeneralizedHyperbolic = -1;
    double bfPower = -1;
    double bfEbertPrelec  = -1;
    double bfBleichrodt  = -1;

    /** Probs
      *
      */
    double probsNoise = -1;
    double probsHyperbolic = -1;
    double probsExponential = -1;
    double probsParabolic = -1;
    double probsQuasiHyperbolic = -1;
    double probsMyerson = -1;
    double probsRachlin = -1;
    double probsGeneralizedHyperbolic = -1;
    double probsPower = -1;
    double probsEbertPrelec  = -1;
    double probsBleichrodt  = -1;

    /** RMSE
      *
      */
    double rmseNoise = -1;
    double rmseHyperbolic = -1;
    double rmseExponential = -1;
    double rmseParabolic = -1;
    double rmseQuasiHyperbolic = -1;
    double rmseMyerson = -1;
    double rmseRachlin = -1;
    double rmseGeneralizedHyperbolic = -1;
    double rmsePower = -1;
    double rmseEbertPrelec  = -1;
    double rmseBleichrodt  = -1;

    /** Fits
      *
      */
    double fitHyperbolicK = -1;
    double fitExponentialK = -1;
    double fitParabolicK = -1;
    double fitQuasiHyperbolicBeta = -1;
    double fitQuasiHyperbolicDelta = -1;
    double fitMyersonK = -1;
    double fitMyersonS = -1;
    double fitRachlinK = -1;
    double fitRachlinS = -1;
    double fitGeneralizedHyperbolicK = -1;
    double fitGeneralizedHyperbolicBeta = -1;
    double fitPowerK = -1;
    double fitPowerS = -1;
    double fitEbertPrelecK = -1;
    double fitEbertPrelecS = -1;
    double fitBleichrodtBeta = -1;
    double fitBleichrodtK = -1;
    double fitBleichrodtS = -1;

    /** Errors
      *
      */
    QList<double> ErrorResidual;

    double NoiseBIC = 0;
    double NoiseRMSE = 0;

    QString errorCode;
    int statusValue;

    QVector<double> GetXVector()
    {
        QVector<double> returnVector;

        for (int i = 0; i < x.rows(); i++)
        {
            returnVector.append(x[i][0]);
        }

        return returnVector;
    }

    QVector<double> GetYVector()
    {
        QVector<double> returnVector;

        for (int i = 0; i < y.length(); i++)
        {
            returnVector.append(y[i]);
        }

        return returnVector;
    }

    /**
     * @brief ScaleFactor
     * @param modelBic
     * @param noiseBic
     * @return
     */
    double ScaleFactor(double modelBic, double noiseBic)
    {
        return exp(-0.5 * (modelBic - noiseBic));
    }

    /**
     * @brief ModelSelection::formatStringResult
     * @param value
     * @return
     */
    QString formatStringResult(int value)
    {
        if (value == -7 || value == -8)
        {
            return QString("gradient verification failed");

        }
        else if (value == 2)
        {
            return QString("Success: relative step is no more than EpsX");

        }
        else if (value == 5)
        {
            return QString("MaxIts steps was taken");
        }
        else if (value == 7)
        {
            return QString("stopping conditions are too stringent, further improvement is impossible");
        }
        else
        {
            return QString("No notes supplied");
        }
    }

    /**
     * @brief ModelSelection::SetFittingAlgorithm
     * @param value
     */
    void SetFittingAlgorithm(FittingAlgorithm value)
    {
        fittingAlgorithm = value;
    }

    double InvIt(double value) const
    {
        return exp(value) / (exp(value) + 1);
    }

    double CalculateAIC()
    {
        return N + N * log(2 * M_PI) + N * log(SSR/N) + 2 * (DF);
    }

    double CalculateBIC()
    {
        return N + N * log(2 * M_PI) + N * log(SSR/N) + log(N) * (DF);
    }

private:
    real_2d_array x;
    real_1d_array y;
    real_1d_array c;

    real_1d_array bndl;
    real_1d_array bndu;

    ae_int_t maxits = 100;
    ae_int_t info;
    lsfitstate state;
    lsfitreport rep;

    double epsx = 0.000001;
    double diffstep = 0.00001;

    /** BIC Stuff
      *
      */
    double AVE = 0;
    double SSR = 0;
    double N = 0;
    double S2 = 0;
    double DF = 0;

    double sumBayesFactors = 0;
    double leastSquaresError = 0;

    int maximumIterationsDE = 1200;
    int populationSize = 100;

    QVector<double> xValues, yValues;

    std::vector<double> result;
};

#endif // MODELSELECTION_H
