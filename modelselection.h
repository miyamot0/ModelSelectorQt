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
#include "interpolation.h"

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "optimization.h"

#define _USE_MATH_DEFINES

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
    QString formatStringResult(int value);
    QString getED50BestModel(QString model);
    QString getAUCBestModel(QString model);
    QStringList getAUCAllModels();

    void FitNoise();
    void FitExponential(const char *mStarts);
    void FitHyperbolic(const char *mStarts);
    void FitQuasiHyperbolic(const char *mStarts);
    void FitMyerson(const char *mStarts);
    void FitRachlin(const char *mStarts);
    void FitRodriguezLogue(const char *mStarts);
    void FitEbertPrelec(const char *mStarts);
    void FitBleichrodt(const char *mStarts);

    void PrepareProbabilities();

    double NoiseBIC = 0;

    QList<QPair<QString, double>> mBicList;
    QList<QPair<QString, double>> mProbList;

    /** AICs
     *
     */
    double aicNoise = -1;
    double aicHyperbolic = -1;
    double aicExponential = -1;
    double aicQuasiHyperbolic = -1;
    double aicMyerson = -1;
    double aicRachlin = -1;
    double aicRodriguezLogue  = -1;
    double aicEbertPrelec  = -1;
    double aicBleichrodt  = -1;

    /** BICs
     *
     */
    double bicNoise = -1;
    double bicHyperbolic = -1;
    double bicExponential = -1;
    double bicQuasiHyperbolic = -1;
    double bicMyerson = -1;
    double bicRachlin = -1;
    double bicRodriguezLogue = -1;
    double bicEbertPrelec  = -1;
    double bicBleichrodt  = -1;

    /** Bayes Factors
      *
      */
    double bfNoise = -1;
    double bfHyperbolic = -1;
    double bfExponential = -1;
    double bfQuasiHyperbolic = -1;
    double bfMyerson = -1;
    double bfRachlin = -1;
    double bfRodriguezLogue = -1;
    double bfEbertPrelec  = -1;
    double bfBleichrodt  = -1;

    /** Probs
      *
      */
    double probsNoise = -1;
    double probsHyperbolic = -1;
    double probsExponential = -1;
    double probsQuasiHyperbolic = -1;
    double probsMyerson = -1;
    double probsRachlin = -1;
    double probsRodriguezLogue = -1;
    double probsEbertPrelec  = -1;
    double probsBleichrodt  = -1;

    /** Fits
      *
      */
    double fitHyperbolicK = -1;
    double fitExponentialK = -1;
    double fitQuasiHyperbolicBeta = -1;
    double fitQuasiHyperbolicDelta = -1;
    double fitMyersonK = -1;
    double fitMyersonS = -1;
    double fitRachlinK = -1;
    double fitRachlinS = -1;
    double fitRodriguezLogueK = -1;
    double fitRodriguezLogueBeta = -1;
    double fitEbertPrelecK = -1;
    double fitEbertPrelecS = -1;
    double fitBleichrodtBeta = -1;
    double fitBleichrodtK = -1;
    double fitBleichrodtS = -1;

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
    double L = 0;
    double DF = 0;
    double PROJ = 0;
    double holder = 0;

    double sumBayesFactors = 0;
};

#endif // MODELSELECTION_H
