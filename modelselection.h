#ifndef MODELSELECTION_H
#define MODELSELECTION_H

#include <QDebug>
#include <QMap>
#include <iostream>
#include "interpolation.h"

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "optimization.h"

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

    void FitNoise();
    void FitExponential(const char *mStarts);
    void FitHyperbolic(const char *mStarts);
    void FitQuasiHyperbolic(const char *mStarts);
    void FitMyerson(const char *mStarts);
    void FitRachlin(const char *mStarts);

    void PrepareProbabilities();

    double NoiseBIC = 0;

    QList<QPair<QString, double>> mBicList;
    QList<QPair<QString, double>> mProbList;

    /** BICs
     *
     */
    double bicNoise = -1;
    double bicHyperbolic = -1;
    double bicExponential = -1;
    double bicQuasiHyperbolic = -1;
    double bicMyerson = -1;
    double bicRachlin = -1;

    /** Bayes Factors
      *
      */
    double bfNoise = -1;
    double bfHyperbolic = -1;
    double bfExponential = -1;
    double bfQuasiHyperbolic = -1;
    double bfMyerson = -1;
    double bfRachlin = -1;

    /** Probs
      *
      */
    double probsNoise = -1;
    double probsHyperbolic = -1;
    double probsExponential = -1;
    double probsQuasiHyperbolic = -1;
    double probsMyerson = -1;
    double probsRachlin = -1;

private:
    real_2d_array x;
    real_1d_array y;
    real_1d_array c;

    real_1d_array bndl;
    real_1d_array bndu;

    ae_int_t maxits = 50;
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
