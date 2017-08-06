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

#ifndef FITRESULT_H
#define FITRESULT_H

#include <QObject>

enum class ModelType {
    Noise,
    Exponential,
    Hyperbolic,
    RodriguezLogue,
    BetaDelta,
    Myerson,
    Rachlin,
    EbertPrelec,
    Beleichrodt
};

class FitResult
{
public:
    FitResult(ModelType mModel);

    ModelType Model;

    QList<QPair<QString, double>> Params;

    double RMS;
    double BIC;
    double AIC;
    double BF;
    double Probability;

    int Rank;

    QString Status;
};

#endif // FITRESULT_H
