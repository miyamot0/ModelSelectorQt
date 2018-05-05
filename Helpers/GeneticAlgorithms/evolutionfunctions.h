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

#ifndef EVOLUTIONFUNCTIONS_H
#define EVOLUTIONFUNCTIONS_H

#pragma once

#include <vector>
#include <QWidget>

#include "Libraries/differential-evolution/differentialevolution.h"

using namespace de;

class ExponentialModel : public IOptimizable
{
public:
    ExponentialModel(QVector<double> delays, QVector<double> values)
    {
        _delays = delays;
        _values = values;
    }

    void SetWeights(QString customWeights)
    {
        weights = SplitWeights(customWeights);
        isWeighted = true;
    }

    QList<double> SplitWeights(QString value)
    {
        QString temp = value.replace("[", "");
        QString temp2 = temp.replace("]", "");

        QStringList tempList = temp2.split(",");

        QList<double> returnList;

        double num;
        foreach (QString str, tempList) {
            num = str.toDouble();

            returnList << num;
        }

        return returnList;
    }

    double EvaluteCost(std::vector<double> inputs) const override
    {
        double val = 0.0;

        double lnk = inputs[0];

        double tempDelay, tempValue;

        double temp;

        for (int j = 0; j < _delays.length(); j++)
        {
            tempDelay = _delays[j];
            tempValue = _values[j];

            temp = tempValue - exp(-exp(lnk) * tempDelay);

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
        }

        val = val / (double) _delays.length();

        return val;
    }

    unsigned int NumberOfParameters() const override
    {
        return m_dim;
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(1);

        constr[0] = Constraints(-100, 100, true);

        return constr;
    }
private:
    unsigned int m_dim = 1;

    QVector<double> _delays;
    QVector<double> _values;

    QList<double> weights;
    bool isWeighted = false;
};

class HyperbolicModel : public IOptimizable
{
public:
    HyperbolicModel(QVector<double> delays, QVector<double> values)
    {
        _delays = delays;
        _values = values;
    }

    void SetWeights(QString customWeights)
    {
        weights = SplitWeights(customWeights);
        isWeighted = true;
    }

    QList<double> SplitWeights(QString value)
    {
        QString temp = value.replace("[", "");
        QString temp2 = temp.replace("]", "");

        QStringList tempList = temp2.split(",");

        QList<double> returnList;

        double num;
        foreach (QString str, tempList) {
            num = str.toDouble();

            returnList << num;
        }

        return returnList;
    }

    double EvaluteCost(std::vector<double> inputs) const override
    {
        double val = 0.0;

        double lnk = inputs[0];

        double tempDelay, tempValue;

        double temp;

        for (int j = 0; j < _delays.length(); j++)
        {
            tempDelay = _delays[j];
            tempValue = _values[j];

            temp = tempValue - pow((1+exp(lnk)*tempDelay), -1);

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
        }

        val = val / (double) _delays.length();

        return val;
    }

    unsigned int NumberOfParameters() const override
    {
        return m_dim;
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(1);

        constr[0] = Constraints(-100, 100, true);

        return constr;
    }
private:
    unsigned int m_dim = 1;

    QVector<double> _delays;
    QVector<double> _values;

    QList<double> weights;
    bool isWeighted = false;
};

class BetaDeltaModel : public IOptimizable
{
public:
    BetaDeltaModel(QVector<double> delays, QVector<double> values)
    {
        _delays = delays;
        _values = values;
    }

    void SetWeights(QString customWeights)
    {
        weights = SplitWeights(customWeights);
        isWeighted = true;
    }

    QList<double> SplitWeights(QString value)
    {
        QString temp = value.replace("[", "");
        QString temp2 = temp.replace("]", "");

        QStringList tempList = temp2.split(",");

        QList<double> returnList;

        double num;
        foreach (QString str, tempList) {
            num = str.toDouble();

            returnList << num;
        }

        return returnList;
    }

    double EvaluteCost(std::vector<double> inputs) const override
    {
        double val = 0.0;

        double beta = inputs[0];
        double delta = inputs[1];

        double tempDelay, tempValue;

        double temp;

        for (int j = 0; j < _delays.length(); j++)
        {
            tempDelay = _delays[j];
            tempValue = _values[j];

            temp = tempValue - (beta * pow(delta, tempDelay));

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
        }

        val = val / (double) _delays.length();

        return val;
    }

    unsigned int NumberOfParameters() const override
    {
        return m_dim;
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = Constraints(0.001, 1, true);
        constr[1] = Constraints(0.9, 1, true);

        return constr;
    }
private:
    unsigned int m_dim = 2;

    QVector<double> _delays;
    QVector<double> _values;

    QList<double> weights;
    bool isWeighted = false;
};

class GreenMyersonModel : public IOptimizable
{
public:
    GreenMyersonModel(QVector<double> delays, QVector<double> values)
    {
        _delays = delays;
        _values = values;
    }

    void SetWeights(QString customWeights)
    {
        weights = SplitWeights(customWeights);
        isWeighted = true;
    }

    QList<double> SplitWeights(QString value)
    {
        QString temp = value.replace("[", "");
        QString temp2 = temp.replace("]", "");

        QStringList tempList = temp2.split(",");

        QList<double> returnList;

        double num;
        foreach (QString str, tempList) {
            num = str.toDouble();

            returnList << num;
        }

        return returnList;
    }

    double EvaluteCost(std::vector<double> inputs) const override
    {
        assert(inputs.size() == m_dim);

        double val = 0.0;

        double k = inputs[0];
        double s = exp(inputs[1]);

        double tempDelay, tempValue;

        double temp;

        for (int j = 0; j < _delays.length(); j++)
        {
            tempDelay = _delays[j];
            tempValue = _values[j];

            temp = tempValue - pow((1 + exp(k) * tempDelay), -s);

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
        }

        val = val / (double) _delays.length();

        return val;
    }

    unsigned int NumberOfParameters() const override
    {
        return m_dim;
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = Constraints(-10, 10, true);
        constr[1] = Constraints(-10, 10, true);

        return constr;
    }
private:
    unsigned int m_dim = 2;

    QVector<double> _delays;
    QVector<double> _values;

    QList<double> weights;
    bool isWeighted = false;
};

class RachlinModel : public IOptimizable
{
public:
    RachlinModel(QVector<double> delays, QVector<double> values)
    {
        _delays = delays;
        _values = values;
    }

    void SetWeights(QString customWeights)
    {
        weights = SplitWeights(customWeights);
        isWeighted = true;
    }

    QList<double> SplitWeights(QString value)
    {
        QString temp = value.replace("[", "");
        QString temp2 = temp.replace("]", "");

        QStringList tempList = temp2.split(",");

        QList<double> returnList;

        double num;
        foreach (QString str, tempList) {
            num = str.toDouble();

            returnList << num;
        }

        return returnList;
    }

    double EvaluteCost(std::vector<double> inputs) const override
    {
        assert(inputs.size() == m_dim);

        double val = 0.0;

        double k = inputs[0];
        double s = exp(inputs[1]);

        double tempDelay, tempValue;

        double temp;

        for (int j = 0; j < _delays.length(); j++)
        {
            tempDelay = _delays[j];
            tempValue = _values[j];

            temp = tempValue - pow((1+exp(k)*pow(tempDelay, s)), -1);

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
        }

        val = val / (double) _delays.length();

        return val;
    }

    unsigned int NumberOfParameters() const override
    {
        return m_dim;
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = Constraints(-10, 10, true);
        constr[1] = Constraints(-10, 10, true);

        return constr;
    }
private:
    unsigned int m_dim = 2;

    QVector<double> _delays;
    QVector<double> _values;

    QList<double> weights;
    bool isWeighted = false;
};

class EbertPrelecModel : public IOptimizable
{
public:
    EbertPrelecModel(QVector<double> delays, QVector<double> values)
    {
        _delays = delays;
        _values = values;
    }

    void SetWeights(QString customWeights)
    {
        weights = SplitWeights(customWeights);
        isWeighted = true;
    }

    QList<double> SplitWeights(QString value)
    {
        QString temp = value.replace("[", "");
        QString temp2 = temp.replace("]", "");

        QStringList tempList = temp2.split(",");

        QList<double> returnList;

        double num;
        foreach (QString str, tempList) {
            num = str.toDouble();

            returnList << num;
        }

        return returnList;
    }

    double EvaluteCost(std::vector<double> inputs) const override
    {
        assert(inputs.size() == m_dim);

        double val = 0.0;

        double k = inputs[0];
        double s = exp(inputs[1]);

        double tempDelay, tempValue;

        double temp;

        for (int j = 0; j < _delays.length(); j++)
        {
            tempDelay = _delays[j];
            tempValue = _values[j];

            temp = tempValue - exp(-pow((exp(k)*tempDelay),s));

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
        }

        val = val / (double) _delays.length();

        return val;
    }

    unsigned int NumberOfParameters() const override
    {
        return m_dim;
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = Constraints(-10, 10, true);
        constr[1] = Constraints(-10, 10, true);

        return constr;
    }
private:
    unsigned int m_dim = 2;

    QVector<double> _delays;
    QVector<double> _values;

    QList<double> weights;
    bool isWeighted = false;
};

class GeneralizedHyperbolicModel : public IOptimizable
{
public:
    GeneralizedHyperbolicModel(QVector<double> delays, QVector<double> values)
    {
        _delays = delays;
        _values = values;
    }

    void SetWeights(QString customWeights)
    {
        weights = SplitWeights(customWeights);
        isWeighted = true;
    }

    QList<double> SplitWeights(QString value)
    {
        QString temp = value.replace("[", "");
        QString temp2 = temp.replace("]", "");

        QStringList tempList = temp2.split(",");

        QList<double> returnList;

        double num;
        foreach (QString str, tempList) {
            num = str.toDouble();

            returnList << num;
        }

        return returnList;
    }

    double EvaluteCost(std::vector<double> inputs) const override
    {
        assert(inputs.size() == m_dim);

        double val = 0.0;

        double k = inputs[0];
        double s = exp(inputs[1]);

        double tempDelay, tempValue;

        double temp;

        for (int j = 0; j < _delays.length(); j++)
        {
            tempDelay = _delays[j];
            tempValue = _values[j];

            temp = tempValue - pow((1 + tempDelay * exp(k)),(-s / exp(k)));

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
        }

        val = val / (double) _delays.length();

        return val;
    }

    unsigned int NumberOfParameters() const override
    {
        return m_dim;
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = Constraints(-10, 10, true);
        constr[1] = Constraints(-10, 10, true);

        return constr;
    }
private:
    unsigned int m_dim = 2;

    QVector<double> _delays;
    QVector<double> _values;

    QList<double> weights;
    bool isWeighted = false;
};

class BeleichrodtModel : public IOptimizable
{
public:
    BeleichrodtModel(QVector<double> delays, QVector<double> values)
    {
        _delays = delays;
        _values = values;
    }

    void SetWeights(QString customWeights)
    {
        weights = SplitWeights(customWeights);
        isWeighted = true;
    }

    QList<double> SplitWeights(QString value)
    {
        QString temp = value.replace("[", "");
        QString temp2 = temp.replace("]", "");

        QStringList tempList = temp2.split(",");

        QList<double> returnList;

        double num;
        foreach (QString str, tempList) {
            num = str.toDouble();

            returnList << num;
        }

        return returnList;
    }

    double EvaluteCost(std::vector<double> inputs) const override
    {
        assert(inputs.size() == m_dim);

        double val = 0.0;

        double k = inputs[0];
        double s = exp(inputs[1]);
        double b = inputs[2];

        double tempDelay, tempValue;

        double temp;

        for (int j = 0; j < _delays.length(); j++)
        {
            tempDelay = _delays[j];
            tempValue = _values[j];

            temp = tempValue - (b * exp(-exp(k) * pow(tempDelay, s)));

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
        }

        val = val / (double) _delays.length();

        return val;
    }

    unsigned int NumberOfParameters() const override
    {
        return m_dim;
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(3);

        constr[0] = Constraints(-10, 10, true);
        constr[1] = Constraints(-10, 10, true);
        constr[2] = Constraints(0, 1, true);

        return constr;
    }
private:
    unsigned int m_dim = 3;

    QVector<double> _delays;
    QVector<double> _values;

    QList<double> weights;
    bool isWeighted = false;
};

#endif // EVOLUTIONFUNCTIONS_H
