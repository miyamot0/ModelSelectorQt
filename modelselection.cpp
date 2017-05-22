#include "modelselection.h"

#include <QDebug>
//#include <cassert>
#include <iostream>
#include "interpolation.h"

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <QtMath>
#include "optimization.h"

using namespace alglib;

void exponential_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = exp(-exp(c[0])*x[0]);
}

void hyperbolic_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = pow((1+exp(c[0])*x[0]), -1);
}

void hyperboloid_myerson(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = pow((1+exp(c[0])*x[0]), -c[1]);
}

void hyperboloid_rachlin(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = pow((1+exp(c[0])*pow(x[0], c[1])), -1);
}

void quasi_hyperboloid(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = c[0] * pow(c[1], x[0]);
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

double ModelSelection::GetBIC()
{
    return BIC;
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
        SSR += qPow(((double) y[i] - AVE), 2);
    }

    S2 = SSR / N;

    L = qPow((1.0 / qSqrt(2 * M_PI * S2)), N) * qExp(-SSR / (S2 * 2.0));

    DF = 2;

    BIC = -2 * qLn(L) + qLn(N) * DF;
}

/** Exponentials
 * @brief
 */
void ModelSelection::FitExponential(const char *mStarts)
{
    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, exponential_discounting);

    lsfitresults(state, info, c, rep);

    N = y.length();
    SSR = 0;

    for (int i = 0; i < N; i++)
    {
        holder = (exp(-exp( (double) c[0])* (double) x[i][0]));

        SSR += qPow(((double) y[i] - holder), 2);
    }

    S2 = SSR / N;

    L = qPow((1.0 / qSqrt(2 * M_PI * S2)), N) * qExp(-SSR / (S2 * 2.0));

    DF = 2;

    BIC = -2 * qLn(L) + qLn(N) * DF;
}

/** Hyperbolics
 * @brief
 */
void ModelSelection::FitHyperbolic(const char *mStarts)
{
    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, hyperbolic_discounting);

    lsfitresults(state, info, c, rep);

    N = y.length();
    SSR = 0;

    for (int i = 0; i < N; i++)
    {
        holder = pow((1+exp( (double) c[0])* (double) x[i][0]), -1);
        SSR += qPow(((double) y[i] - holder), 2);
    }

    S2 = SSR / N;

    L = qPow((1.0 / qSqrt(2 * M_PI * S2)), N) * qExp(-SSR / (S2 * 2.0));

    DF = 2;

    BIC = -2 * qLn(L) + qLn(N) * DF;
}

/** Hyperboloid Myerson
 * @brief
 */
void ModelSelection::FitMyerson(const char *mStarts)
{
    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, hyperboloid_myerson);

    lsfitresults(state, info, c, rep);

    N = y.length();
    SSR = 0;

    for (int i = 0; i < N; i++)
    {
        holder = pow((1+exp( (double) c[0])* (double) x[i][0]),  (double) -c[1]);
        SSR += qPow(((double) y[i] - holder), 2);
    }

    S2 = SSR / N;

    L = qPow((1.0 / qSqrt(2 * M_PI * S2)), N) * qExp(-SSR / (S2 * 2.0));

    DF = 3;

    BIC = -2 * qLn(L) + qLn(N) * DF;
}

/** Beta delta
  *  @brief
  */
void ModelSelection::FitQuasiHyperbolic(const char *mStarts)
{
    SetStarts(mStarts);
    SetLowerUpperBounds("[1.0, 1.0]", "[0.0, 0.0]");

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetbc(state, bndl, bndu);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, quasi_hyperboloid);

    lsfitresults(state, info, c, rep);

    N = y.length();
    SSR = 0;

    for (int i = 0; i < N; i++)
    {
        holder = (double) c[0] * pow( (double) c[1], (double) x[i][0]);
        SSR += qPow(((double) y[i] - holder), 2);
    }

    S2 = SSR / N;

    L = qPow((1.0 / qSqrt(2 * M_PI * S2)), N) * qExp(-SSR / (S2 * 2.0));

    DF = 3;

    BIC = -2 * qLn(L) + qLn(N) * DF;
}

/** Hyperboloid Rachlin
  *  @brief
  */
void ModelSelection::FitRachlin(const char *mStarts)
{
    SetStarts(mStarts);

    lsfitcreatef(x, y, c, diffstep, state);
    lsfitsetcond(state, epsx, maxits);

    alglib::lsfitfit(state, hyperboloid_rachlin);

    lsfitresults(state, info, c, rep);

    N = y.length();
    SSR = 0;

    for (int i = 0; i < N; i++)
    {
        holder = pow((1+exp( (double) c[0])*pow( (double) x[i][0], (double) c[1])), -1);
        SSR += qPow(((double) y[i] - holder), 2);
    }

    S2 = SSR / N;

    L = qPow((1.0 / qSqrt(2 * M_PI * S2)), N) * qExp(-SSR / (S2 * 2.0));

    DF = 3;

    BIC = -2 * qLn(L) + qLn(N) * DF;
}

double ScaleFactor(double modelBic, double noiseBic)
{
    return qExp(-0.5 * (modelBic - noiseBic));
}

void ModelSelection::PrepareProbabilities()
{
    bfNoise = ScaleFactor(NoiseBIC, NoiseBIC);
    bfExponential = -1;
    bfHyperbolic = -1;
    bfQuasiHyperbolic = -1;
    bfMyerson = -1;
    bfRachlin = -1;

    sumBayesFactors = 0;

    /** Set up BF's
      */
    for (int i=0; i<mBicList.length(); i++)
    {
        QString mModel(mBicList[i].first);

        if (mModel.contains("Exponential", Qt::CaseInsensitive))
        {
            bfExponential = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfExponential;
        }
        else if (mModel.contains("Hyperbolic", Qt::CaseInsensitive))
        {
            bfHyperbolic = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfHyperbolic;
        }
        else if (mModel.contains("Beta", Qt::CaseInsensitive))
        {
            bfQuasiHyperbolic = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfQuasiHyperbolic;
        }
        else if (mModel.contains("Myerson", Qt::CaseInsensitive))
        {
            bfMyerson = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfMyerson;
        }
        else if (mModel.contains("Rachlin", Qt::CaseInsensitive))
        {
            bfRachlin = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfRachlin;
        }
    }

    mProbList.clear();
    mProbList.append(QPair<QString, double>("Noise Model", bfNoise/sumBayesFactors));

    for (int i=0; i<mBicList.length(); i++)
    {
        QString mModel(mBicList[i].first);

        if (mModel.contains("Exponential", Qt::CaseInsensitive))
        {
            mProbList.append(QPair<QString, double>("Exponential Model", bfExponential/sumBayesFactors));
        }
        else if (mModel.contains("Hyperbolic", Qt::CaseInsensitive))
        {
            mProbList.append(QPair<QString, double>("Hyperbolic Model", bfHyperbolic/sumBayesFactors));
        }
        else if (mModel.contains("Beta", Qt::CaseInsensitive))
        {
            mProbList.append(QPair<QString, double>("Beta Delta Model", bfQuasiHyperbolic/sumBayesFactors));
        }
        else if (mModel.contains("Myerson", Qt::CaseInsensitive))
        {
            mProbList.append(QPair<QString, double>("Myerson Model", bfMyerson/sumBayesFactors));
        }
        else if (mModel.contains("Rachlin", Qt::CaseInsensitive))
        {
            mProbList.append(QPair<QString, double>("Rachlin Model", bfRachlin/sumBayesFactors));
        }
    }
}

ModelSelection::ModelSelection()
{

}
