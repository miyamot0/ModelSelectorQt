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

   The Discounting Model Selector uses ALGLIB to provide access to mathematical methods.

   ====================================================================================

   ALGLIB 3.11.0 (source code generated 2017-05-11)
   Copyright (c) Sergey Bochkanov (ALGLIB project).

   >>> SOURCE LICENSE >>>
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation (www.fsf.org); either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   A copy of the GNU General Public License is available at
   http://www.fsf.org/licensing/licenses
   >>> END OF LICENSE >>>

  */

#include "modelselection.h"

#include <iostream>
#include "chartwindow.h"
#include "interpolation.h"

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "optimization.h"
#include "integration.h"

#include <QDebug>

using namespace alglib;

void exponential_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = exp(-exp(c[0])*x[0]);
}

void exponential_integration(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    y = exp(-exp(k)*x);
}

void exponential_integration_log10(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    y = exp(-exp(k)*pow(10,x));
}

void hyperbolic_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = pow((1+exp(c[0])*x[0]), -1);
}

void hyperbolic_integration(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    y = pow((1+exp(k)*x), -1);
}

void hyperbolic_integration_log10(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    y = pow((1+exp(k)*pow(10,x)), -1);
}

void generalized_hyperboloid_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = pow((1 + x[0] * exp(c[0])),(-exp(c[1]) / exp(c[0])));
}

void generalized_hyperboloid_integration(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double lnk = param->at(0);
    double beta = param->at(1);

    y = pow((1 + x * exp(lnk)),(-exp(beta) / exp(lnk)));
}

void generalized_hyperboloid_integration_log10(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double lnk = param->at(0);
    double beta = param->at(1);

    y = pow((1 + pow(10,x) * exp(lnk)),(-exp(beta) / exp(lnk)));
}

void quasi_hyperboloid_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = c[0] * pow(c[1], x[0]);
}

void quasi_hyperboloid_integration(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double b = param->at(0);
    double d = param->at(1);
    y = b * pow(d, x);
}

void quasi_hyperboloid_integration_log10(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double b = param->at(0);
    double d = param->at(1);
    y = b * pow(d, pow(10,x));
}

void hyperboloid_myerson_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = pow((1+exp(c[0])*x[0]), -c[1]);
}

void hyperboloid_myerson_integration(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = pow((1+exp(k)*x), -s);
}

void hyperboloid_myerson_integration_log10(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = pow((1+exp(k)*pow(10,x)), -s);
}

void hyperboloid_rachlin_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = pow((1+exp(c[0])*pow(x[0], c[1])), -1);
}

void hyperboloid_rachlin_integration(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = pow((1+exp(k)*pow(x, s)), -1);
}

void hyperboloid_rachlin_integration_log10(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = pow((1+exp(k)*pow(pow(10,x), s)), -1);
}

void ebert_prelec_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = exp(-pow((exp(c[0])*x[0]),c[1]));
}

void ebert_prelec_integration(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = exp(-pow((exp(k)*x), s));
}

void ebert_prelec_integration_log10(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = exp(-pow((exp(k)*pow(10,x)), s));
}

void bleichrodt_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = c[2] * exp(-exp(c[0])*pow(x[0],c[1]));
}

void bleichrodt_integration(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    double beta = param->at(2);

    y = beta * exp(-exp(k)*pow(x,s));
}

void bleichrodt_integration_log10(double x, double xminusa, double bminusx, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    double beta = param->at(2);

    y = beta * exp(-exp(k)*pow(pow(10,x),s));
}

void ModelSelection::SetX(const char *mString)
{
    x = mString;
}

void ModelSelection::SetY(const char *mString)
{
    y = mString;
}

void ModelSelection::SetStarts(const char *mString)
{
    c = mString;
}

void ModelSelection::SetLowerUpperBounds(const char *mUpperString, const char *mLowerString)
{
    bndu = mUpperString;
    bndl = mLowerString;
}

real_1d_array ModelSelection::GetParams()
{
    return c;
}

lsfitstate ModelSelection::GetState()
{
    return state;
}

ae_int_t ModelSelection::GetInfo()
{
    return info;
}

lsfitreport ModelSelection::GetReport()
{
    return rep;
}

double ModelSelection::GetNoiseMean()
{
    return AVE;
}

/** Calculate Noise BIC
 * @brief ModelSelection::FitNoise
 */
void ModelSelection::FitNoise()
{
    N = y.length();
    AVE = 0;

    for (int i = 0; i < N; i++)
    {
        AVE += (double) y[i];
    }

    AVE = AVE / N;
    SSR = 0;

    for (int i = 0; i < N; i++)
    {
        SSR += pow(((double) y[i] - AVE), 2);
    }

    S2 = SSR / N;

    L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

    DF = 2;

    aicNoise = (-2 * log(L)) + (2 * DF);
    bicNoise = -2 * log(L) + log(N) * DF;
}

/** Exponentials
 * @brief
 */
void ModelSelection::FitExponential(const char *mStarts)
{
    aicExponential = NULL;
    bicExponential = NULL;
    fitExponentialK = NULL;

    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, exponential_discounting);

    lsfitresults(state, info, c, rep);

    if ((int) info == 2 || (int) info == 5)
    {
        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = (exp(-exp( (double) c[0])* (double) x[i][0]));

            SSR += pow(((double) y[i] - holder), 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 2;

        aicExponential = (-2 * log(L)) + (2 * DF);
        bicExponential = -2 * log(L) + log(N) * DF;
        fitExponentialK = (double) c[0];
    }
}

/** Hyperbolics
 * @brief
 */
void ModelSelection::FitHyperbolic(const char *mStarts)
{
    aicHyperbolic = NULL;
    bicHyperbolic = NULL;
    fitHyperbolicK = NULL;

    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, hyperbolic_discounting);

    lsfitresults(state, info, c, rep);

    if ((int) info == 2 || (int) info == 5)
    {
        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = pow((1+exp( (double) c[0])* (double) x[i][0]), -1);
            SSR += pow(((double) y[i] - holder), 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 2;

        aicHyperbolic = (-2 * log(L)) + (2 * DF);
        bicHyperbolic = -2 * log(L) + log(N) * DF;
        fitHyperbolicK = (double) c[0];
    }
}

/** Beta delta
  *  @brief
  */
void ModelSelection::FitQuasiHyperbolic(const char *mStarts)
{
    aicQuasiHyperbolic = NULL;
    bicQuasiHyperbolic = NULL;
    fitQuasiHyperbolicBeta = NULL;
    fitQuasiHyperbolicDelta = NULL;

    SetStarts(mStarts);

    SetLowerUpperBounds("[1.0, 1.0]", "[0.0, 0.0]");

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetbc(state, bndl, bndu);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, quasi_hyperboloid_discounting);

    lsfitresults(state, info, c, rep);

    if ((int) info == 2 || (int) info == 5)
    {
        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = (double) c[0] * pow( (double) c[1], (double) x[i][0]);
            SSR += pow(((double) y[i] - holder), 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 3;

        aicQuasiHyperbolic = (-2 * log(L)) + (2 * DF);
        bicQuasiHyperbolic = -2 * log(L) + log(N) * DF;
        fitQuasiHyperbolicBeta = (double) c[0];
        fitQuasiHyperbolicDelta = (double) c[1];
    }
}

/** Hyperboloid Myerson
 * @brief
 */
void ModelSelection::FitMyerson(const char *mStarts)
{
    aicMyerson = NULL;
    bicMyerson = NULL;
    fitMyersonK = NULL;
    fitMyersonS = NULL;

    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, hyperboloid_myerson_discounting);

    lsfitresults(state, info, c, rep);

    if ((int) info == 2 || (int) info == 5)
    {
        N = y.length();

        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = pow((1+exp( (double) c[0])* (double) x[i][0]),  (double) -c[1]);
            SSR += pow(((double) y[i] - holder), 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 3;

        aicMyerson = (-2 * log(L)) + (2 * DF);
        bicMyerson = -2 * log(L) + log(N) * DF;
        fitMyersonK = (double) c[0];
        fitMyersonS = (double) c[1];
    }
}

/** Hyperboloid Rachlin
  *  @brief
  */
void ModelSelection::FitRachlin(const char *mStarts)
{
    statusValue = NULL;
    aicRachlin = NULL;
    bicRachlin = NULL;
    fitRachlinK = NULL;
    fitRachlinS = NULL;

    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);

    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, hyperboloid_rachlin_discounting);

    lsfitresults(state, info, c, rep);

    if ((int) GetInfo() == 2 || (int) GetInfo() == 5)
    {
        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = pow((1+exp( (double) c[0])*pow( (double) x[i][0], (double) c[1])), -1);
            SSR += pow(((double) y[i] - holder), 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 3;

        aicRachlin = (-2 * log(L)) + (2 * DF);
        bicRachlin = -2 * log(L) + log(N) * DF;
        fitRachlinK = (double) c[0];
        fitRachlinS = (double) c[1];

        statusValue = (int) info;
    }
}

/** Rodriguez Logue Model
  *  @brief
  */
void ModelSelection::FitRodriguezLogue(const char *mStarts)
{
    aicRodriguezLogue = NULL;
    bicRodriguezLogue = NULL;
    fitRodriguezLogueK = NULL;
    fitRodriguezLogueBeta = NULL;

    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, generalized_hyperboloid_discounting);

    lsfitresults(state, info, c, rep);

    if ((int) GetInfo() == 2 || (int) GetInfo() == 5)
    {
        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = pow((1 + x[i][0] * exp(c[0])),(-exp(c[1]) / exp(c[0])));
            SSR += pow(((double) y[i] - holder), 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 3;

        aicRodriguezLogue = (-2 * log(L)) + (2 * DF);
        bicRodriguezLogue = -2 * log(L) + log(N) * DF;
        fitRodriguezLogueK = (double) c[0];
        fitRodriguezLogueBeta = (double) c[1];
    }
}

/** Ebert Model
  *  @brief
  */
void ModelSelection::FitEbertPrelec(const char *mStarts)
{
    aicEbertPrelec = NULL;
    bicEbertPrelec = NULL;
    fitEbertPrelecK = NULL;
    fitEbertPrelecS = NULL;

    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, ebert_prelec_discounting);

    lsfitresults(state, info, c, rep);

    if ((int) GetInfo() == 2 || (int) GetInfo() == 5)
    {
        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = exp(-pow((exp(c[0])*x[i][0]), c[1]));
            SSR += pow(((double) y[i] - holder), 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 3;

        aicEbertPrelec = (-2 * log(L)) + (2 * DF);
        bicEbertPrelec = -2 * log(L) + log(N) * DF;
        fitEbertPrelecK = (double) c[0];
        fitEbertPrelecS = (double) c[1];
    }
}

/** Bleichrodt Model
  *  @brief
  */
void ModelSelection::FitBleichrodt(const char *mStarts)
{
    aicBleichrodt = NULL;
    bicBleichrodt = NULL;
    fitBleichrodtK = NULL;
    fitBleichrodtS = NULL;
    fitBleichrodtBeta = NULL;

    SetStarts(mStarts);

    SetLowerUpperBounds("[Inf, Inf, 1.0]", "[-Inf, -Inf, 0.0]");

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetbc(state, bndl, bndu);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, bleichrodt_discounting);

    lsfitresults(state, info, c, rep);

    if ((int) GetInfo() == 2 || (int) GetInfo() == 5)
    {
        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = c[2] * exp(-exp(c[0])*pow(x[i][0], c[1]));
            SSR += pow(((double) y[i] - holder), 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 4;

        aicBleichrodt = (-2 * log(L)) + (2 * DF);
        bicBleichrodt = -2 * log(L) + log(N) * DF;
        fitBleichrodtK = (double) c[0];
        fitBleichrodtS = (double) c[1];
        fitBleichrodtBeta = (double) c[2];
    }
}

double ScaleFactor(double modelBic, double noiseBic)
{
    return exp(-0.5 * (modelBic - noiseBic));
}

QString ModelSelection::formatStringResult(int value)
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

QString ModelSelection::getED50BestModel(ModelType model)
{
    double result;

    switch (model)
    {
    case ModelType::Noise:
        return QString("NA");

        break;

    case ModelType::Exponential:
        result = log(1/(exp(fitExponentialK)));
        return QString::number(result);

        break;

    case ModelType::Hyperbolic:
        result = log(1/(exp(fitHyperbolicK)));
        return QString::number(result);

        break;

    case ModelType::BetaDelta:
        result = log(log((1/(2*fitQuasiHyperbolicBeta)))/log(fitQuasiHyperbolicDelta));
        return QString::number(result);

        break;

    case ModelType::Myerson:
        result = log((pow(2, (1/fitMyersonS))-1)/exp(fitMyersonK));
        return QString::number(result);

        break;

    case ModelType::Rachlin:
        result = log( pow( (1/ (exp(fitRachlinK))), (1/fitRachlinS)));
        return QString::number(result);

        break;

    case ModelType::RodriguezLogue:
        result = getED50rodriguez();
        return QString::number(result);

        break;

    case ModelType::EbertPrelec:
        result = getED50ep();
        return QString::number(result);

        break;

    case ModelType::Beleichrodt:
        result = getED50crdi();
        return QString::number(result);

        break;

    default:

        return QString("---");

        break;

    }
}

double ModelSelection::getED50ep () {
    double lowDelay = 0;
    double highDelay = x[x.rows()-1][0]*10;

    while ((highDelay - lowDelay) > 0.001) {
      double lowEst = ChartWindow::ebert_prelec_plotting(fitEbertPrelecK, fitEbertPrelecS, lowDelay);
      double midEst = ChartWindow::ebert_prelec_plotting(fitEbertPrelecK, fitEbertPrelecS, (lowDelay+highDelay)/2);
      double highEst = ChartWindow::ebert_prelec_plotting(fitEbertPrelecK, fitEbertPrelecS, highDelay);

      if (lowEst > 0.5 && midEst > 0.5) {
        //Above 50% mark range
        lowDelay  = (lowDelay+highDelay)/2;
        highDelay = highDelay;

      } else if (highEst < 0.5 && midEst < 0.5) {
        //Below 50% mark range
        lowDelay  = lowDelay;
        highDelay = (lowDelay+highDelay)/2;

      }
    }

    double returnValue = log((lowDelay+highDelay)/2);

    return returnValue;
}

double ModelSelection::getED50crdi () {
    double lowDelay = 0;
    double highDelay = x[x.rows()-1][0]*10;

    while ((highDelay - lowDelay) > 0.001) {
      double lowEst = ChartWindow::bleichrodt_plotting(fitBleichrodtK, fitBleichrodtS, fitBleichrodtBeta, lowDelay);
      double midEst = ChartWindow::bleichrodt_plotting(fitBleichrodtK, fitBleichrodtS, fitBleichrodtBeta, (lowDelay+highDelay)/2);
      double highEst = ChartWindow::bleichrodt_plotting(fitBleichrodtK, fitBleichrodtS, fitBleichrodtBeta, highDelay);

      if (lowEst > 0.5 && midEst > 0.5) {
        //Above 50% mark range
        lowDelay  = (lowDelay+highDelay)/2;
        highDelay = highDelay;

      } else if (highEst < 0.5 && midEst < 0.5) {
        //Below 50% mark range
        lowDelay  = lowDelay;
        highDelay = (lowDelay+highDelay)/2;

      }
    }

    double returnValue = log((lowDelay+highDelay)/2);

    return returnValue;
}

double ModelSelection::getED50rodriguez () {
    double lowDelay = 0;
    double highDelay = x[x.rows()-1][0]*10;

    while ((highDelay - lowDelay) > 0.001) {
      double lowEst = ChartWindow::rodriguez_logue_plotting(fitRodriguezLogueK, fitRodriguezLogueBeta, lowDelay);
      double midEst = ChartWindow::rodriguez_logue_plotting(fitRodriguezLogueK, fitRodriguezLogueBeta, (lowDelay+highDelay)/2);
      double highEst = ChartWindow::rodriguez_logue_plotting(fitRodriguezLogueK, fitRodriguezLogueBeta, highDelay);

      if (lowEst > 0.5 && midEst > 0.5) {
        //Above 50% mark range
        lowDelay  = (lowDelay+highDelay)/2;
        highDelay = highDelay;

      } else if (highEst < 0.5 && midEst < 0.5) {
        //Below 50% mark range
        lowDelay  = lowDelay;
        highDelay = (lowDelay+highDelay)/2;

      }
    }

    double returnValue = log((lowDelay+highDelay)/2);

    return returnValue;
}

QString ModelSelection::getAUCBestModel(ModelType model)
{
    double result;
    double a = x[0][0];
    double b = x[x.rows() - 1][0];

    QList<double> mParams;
    autogkstate s;
    double v;
    autogkreport rep;

    switch (model)
    {
    case ModelType::Noise:
        return QString::number(GetNoiseMean());

        break;

    case ModelType::Exponential:
        mParams << fitExponentialK;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, exponential_integration, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::Hyperbolic:
        mParams << fitHyperbolicK;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, hyperbolic_integration, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::BetaDelta:
        mParams << fitQuasiHyperbolicBeta;
        mParams << fitQuasiHyperbolicDelta;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, quasi_hyperboloid_integration, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::Myerson:
        mParams << fitMyersonK;
        mParams << fitMyersonS;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, hyperboloid_myerson_integration, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::Rachlin:
        mParams << fitRachlinK;
        mParams << fitRachlinS;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, hyperboloid_rachlin_integration, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::RodriguezLogue:
        mParams << fitRodriguezLogueK;
        mParams << fitRodriguezLogueBeta;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, generalized_hyperboloid_integration, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::EbertPrelec:
        mParams << fitEbertPrelecK;
        mParams << fitEbertPrelecS;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, ebert_prelec_integration, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::Beleichrodt:
        mParams << fitBleichrodtK;
        mParams << fitBleichrodtS;
        mParams << fitBleichrodtBeta;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, bleichrodt_integration, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    default:

        return QString("---");

        break;

    }
}

QString ModelSelection::getLogAUCBestModel(ModelType model)
{
    double result;
    double a, b;

    if (x[0][0] == 0)
    {
        a = log10(x[0][0] + 1);
        b = log10(x[x.rows() - 1][0] + 1);
    }
    else
    {
        a = log10(x[0][0]);
        b = log10(x[x.rows() - 1][0]);
    }

    QList<double> mParams;
    autogkstate s;
    double v;
    autogkreport rep;

    switch (model)
    {
    case ModelType::Noise:
        return QString::number(GetNoiseMean());

        break;

    case ModelType::Exponential:
        mParams << fitExponentialK;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, exponential_integration_log10, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::Hyperbolic:
        mParams << fitHyperbolicK;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, hyperbolic_integration_log10, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::BetaDelta:
        mParams << fitQuasiHyperbolicBeta;
        mParams << fitQuasiHyperbolicDelta;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, quasi_hyperboloid_integration_log10, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::Myerson:
        mParams << fitMyersonK;
        mParams << fitMyersonS;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, hyperboloid_myerson_integration_log10, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::Rachlin:
        mParams << fitRachlinK;
        mParams << fitRachlinS;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, hyperboloid_rachlin_integration_log10, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::RodriguezLogue:
        mParams << fitRodriguezLogueK;
        mParams << fitRodriguezLogueBeta;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, generalized_hyperboloid_integration_log10, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::EbertPrelec:
        mParams << fitEbertPrelecK;
        mParams << fitEbertPrelecS;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, ebert_prelec_integration_log10, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    case ModelType::Beleichrodt:
        mParams << fitBleichrodtK;
        mParams << fitBleichrodtS;
        mParams << fitBleichrodtBeta;

        autogksmooth(a, b, s);
        alglib::autogkintegrate(s, bleichrodt_integration_log10, &mParams);
        autogkresults(s, v, rep);

        result = double(v) / (b - a);

        return QString::number(result);

        break;

    default:

        return QString("---");

        break;

    }
}

void ModelSelection::PrepareProbabilities()
{
    bfNoise = ScaleFactor(NoiseBIC, NoiseBIC);
    bfExponential = NULL;
    bfHyperbolic = NULL;
    bfQuasiHyperbolic = NULL;
    bfMyerson = NULL;
    bfRachlin = NULL;
    bfRodriguezLogue = NULL;
    bfEbertPrelec = NULL;
    bfBleichrodt = NULL;

    sumBayesFactors = 0;

    /** Set up BF's
      */
    for (int i=0; i<mBicList.length(); i++)
    {
        if (mBicList[i].first == ModelType::Exponential)
        {
            bfExponential = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfExponential;
        }
        else if (mBicList[i].first == ModelType::Hyperbolic)
        {
            bfHyperbolic = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfHyperbolic;
        }
        else if (mBicList[i].first == ModelType::BetaDelta)
        {
            bfQuasiHyperbolic = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfQuasiHyperbolic;
        }
        else if (mBicList[i].first == ModelType::Myerson)
        {
            bfMyerson = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfMyerson;
        }
        else if (mBicList[i].first == ModelType::Rachlin)
        {
            bfRachlin = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfRachlin;
        }
        else if (mBicList[i].first == ModelType::RodriguezLogue)
        {
            bfRodriguezLogue = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfRodriguezLogue;
        }
        else if (mBicList[i].first == ModelType::EbertPrelec)
        {
            bfEbertPrelec = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfEbertPrelec;
        }
        else if (mBicList[i].first == ModelType::Beleichrodt)
        {
            bfBleichrodt = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfBleichrodt;
        }
    }

    probsNoise = bfNoise/sumBayesFactors;
    probsHyperbolic = NULL;
    probsExponential = NULL;
    probsQuasiHyperbolic = NULL;
    probsMyerson = NULL;
    probsRachlin = NULL;
    probsRodriguezLogue = NULL;
    probsEbertPrelec = NULL;

    mProbList.clear();
    mProbList.append(QPair<ModelType, double>(ModelType::Noise, probsNoise));

    for (int i=0; i<mBicList.length(); i++)
    {
        if (mBicList[i].first == ModelType::Exponential)
        {
            probsExponential = bfExponential/sumBayesFactors;
            mProbList.append(QPair<ModelType, double>(ModelType::Exponential, probsExponential));
        }
        else if (mBicList[i].first == ModelType::Hyperbolic)
        {
            probsHyperbolic = bfHyperbolic/sumBayesFactors;
            mProbList.append(QPair<ModelType, double>(ModelType::Hyperbolic, probsHyperbolic));
        }
        else if (mBicList[i].first == ModelType::BetaDelta)
        {
            probsQuasiHyperbolic = bfQuasiHyperbolic/sumBayesFactors;
            mProbList.append(QPair<ModelType, double>(ModelType::BetaDelta, probsQuasiHyperbolic));
        }
        else if (mBicList[i].first == ModelType::Myerson)
        {
            probsMyerson = bfMyerson/sumBayesFactors;
            mProbList.append(QPair<ModelType, double>(ModelType::Myerson, probsMyerson));
        }
        else if (mBicList[i].first == ModelType::Rachlin)
        {
            probsRachlin = bfRachlin/sumBayesFactors;
            mProbList.append(QPair<ModelType, double>(ModelType::Rachlin, probsRachlin));
        }
        else if (mBicList[i].first == ModelType::RodriguezLogue)
        {
            probsRodriguezLogue = bfRodriguezLogue/sumBayesFactors;
            mProbList.append(QPair<ModelType, double>(ModelType::RodriguezLogue, probsRodriguezLogue));
        }
        else if (mBicList[i].first == ModelType::EbertPrelec)
        {
            probsEbertPrelec = bfEbertPrelec/sumBayesFactors;
            mProbList.append(QPair<ModelType, double>(ModelType::EbertPrelec, probsEbertPrelec));
        }
        else if (mBicList[i].first == ModelType::Beleichrodt)
        {
            probsBleichrodt = bfBleichrodt/sumBayesFactors;
            mProbList.append(QPair<ModelType, double>(ModelType::Beleichrodt, probsBleichrodt));
        }
    }
}

ModelSelection::ModelSelection()
{

}
