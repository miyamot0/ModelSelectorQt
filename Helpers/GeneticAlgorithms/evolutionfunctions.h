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

/**
 * @brief The BaseModel class
 *
 * Base model for providing structuring the DE fitting objects
 */
class BaseModel : public IOptimizable
{
public:
    /**
     * @brief BaseModel
     *
     * @param delays Delay value vector
     * @param values Value values vector
     */
    BaseModel(QVector<double> delays, QVector<double> values)
    {
        _delays = delays;
        _values = values;
    }

    void SetData(QVector<double> delays, QVector<double> values)
    {
        _delays = delays;
        _values = values;
    }

    /**
     * @brief CostFunction
     *
     * @param inputs Prospective parameters
     * @param delay point in time
     *
     * @return Subjective value at delay
     */
    virtual double CostFunction(std::vector<double> inputs, double delay) const = 0;

    /**
     * @brief EvaluteCost
     *
     * Return cost function by way of MSE, and inherited cost function
     *
     * @param inputs parameters
     * @return
     */
    double EvaluteCost(std::vector<double> inputs) const override
    {
        double val = 0.0;

        double tempDelay, tempValue;

        double temp;

        for (int j = 0; j < _delays.length(); j++)
        {
            tempDelay = _delays[j];
            tempValue = _values[j];

            temp = tempValue - CostFunction(inputs, tempDelay);

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

    /**
     * @brief SetWeights
     *
     * Residual weighting option (Experimental)
     *
     * @param customWeights
     */
    void SetWeights(QString customWeights)
    {
        weights = SplitWeights(customWeights);
        isWeighted = true;
    }

    /**
     * @brief SplitWeights
     *
     * Helper function for parsing weight vector (Experimental)
     *
     * @param value
     * @return
     */
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

    /**
     * @brief NumberOfParameters
     *
     * As says on the tin
     *
     * @return
     */
    unsigned int NumberOfParameters() const override
    {
        return m_dim;
    }

    /**
     * @brief defaultConstraints
     *
     * As says on the tin
     *
     * @return
     */
    Constraints DefaultConstraints() const
    {
        return Constraints(-100, 100, true);
    }

    double InvIt(double value) const
    {
        return exp(value) / (exp(value) + 1);
    }

    unsigned int m_dim = 1;

    QVector<double> _delays;
    QVector<double> _values;

    QList<double> weights;
    bool isWeighted = false;
};

class ExponentialModel : public BaseModel
{
public:
    ExponentialModel (QVector<double> delays, QVector<double> values) : BaseModel(delays, values)
    {
        _delays = delays;
        _values = values;

        m_dim = 1;
    }

    double CostFunction(std::vector<double> inputs, double delay) const override
    {
        return exp(-exp(inputs[0]) * delay);
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(1);

        constr[0] = DefaultConstraints();

        return constr;
    }
};

class ParabolicModel : public BaseModel
{
public:
    ParabolicModel (QVector<double> delays, QVector<double> values) : BaseModel(delays, values)
    {
        _delays = delays;
        _values = values;

        m_dim = 1;
    }

    double CostFunction(std::vector<double> inputs, double delay) const override
    {
        return (1.0 - (exp(inputs[0]) * pow(delay, 2)));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(1);

        constr[0] = DefaultConstraints();

        return constr;
    }
};

class HyperbolicModel : public BaseModel
{
public:
    HyperbolicModel (QVector<double> delays, QVector<double> values) : BaseModel(delays, values)
    {
        _delays = delays;
        _values = values;

        m_dim = 1;
    }

    double CostFunction(std::vector<double> inputs, double delay) const override
    {
        return pow((1+exp(inputs[0])*delay), -1);
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(1);

        constr[0] = DefaultConstraints();

        return constr;
    }
};

class BetaDeltaModel : public BaseModel
{
public:
    BetaDeltaModel (QVector<double> delays, QVector<double> values) : BaseModel(delays, values)
    {
        _delays = delays;
        _values = values;

        m_dim = 2;
    }

    double CostFunction(std::vector<double> inputs, double delay) const override
    {
        return (InvIt(inputs[0]) * pow(InvIt(inputs[1]), delay));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = Constraints(-1000, 500, true);
        constr[1] = Constraints(-1000, 500, true);

        return constr;
    }
};

class PowerModel : public BaseModel
{
public:
    PowerModel (QVector<double> delays, QVector<double> values) : BaseModel(delays, values)
    {
        _delays = delays;
        _values = values;

        m_dim = 2;
    }

    double CostFunction(std::vector<double> inputs, double delay) const override
    {
        return (1 - exp(inputs[0]) * pow(delay, exp(inputs[1])));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = DefaultConstraints();
        constr[1] = DefaultConstraints();

        return constr;
    }
};

class GreenMyersonModel : public BaseModel
{
public:
    GreenMyersonModel (QVector<double> delays, QVector<double> values) : BaseModel(delays, values)
    {
        _delays = delays;
        _values = values;

        m_dim = 2;
    }

    double CostFunction(std::vector<double> inputs, double delay) const override
    {
        return pow((1 + exp(inputs[0]) * delay), -exp(inputs[1]));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = DefaultConstraints();
        constr[1] = DefaultConstraints();

        return constr;
    }
};

class RachlinModel : public BaseModel
{
public:
    RachlinModel (QVector<double> delays, QVector<double> values) : BaseModel(delays, values)
    {
        _delays = delays;
        _values = values;

        m_dim = 2;
    }

    double CostFunction(std::vector<double> inputs, double delay) const override
    {
        return pow((1+exp((double) inputs[0])*pow(delay, exp(inputs[1]))), -1);
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = DefaultConstraints();
        constr[1] = DefaultConstraints();

        return constr;
    }
};

class EbertPrelecModel : public BaseModel
{
public:
    EbertPrelecModel (QVector<double> delays, QVector<double> values) : BaseModel(delays, values)
    {
        _delays = delays;
        _values = values;

        m_dim = 2;
    }

    double CostFunction(std::vector<double> inputs, double delay) const override
    {
        return exp(-pow((exp(inputs[0])*delay), exp(inputs[1])));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = Constraints(-10, 10, true);
        constr[1] = Constraints(-10, 10, true);

        return constr;
    }
};

class GeneralizedHyperbolicModel : public BaseModel
{
public:
    GeneralizedHyperbolicModel (QVector<double> delays, QVector<double> values) : BaseModel(delays, values)
    {
        _delays = delays;
        _values = values;

        m_dim = 2;
    }

    double CostFunction(std::vector<double> inputs, double delay) const override
    {
        return pow((1 + delay * exp(inputs[0])),(-(exp(inputs[1]) / exp(inputs[0]))));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = DefaultConstraints();;
        constr[1] = DefaultConstraints();;

        return constr;
    }
};

class BeleichrodtModel : public BaseModel
{
public:
    BeleichrodtModel (QVector<double> delays, QVector<double> values) : BaseModel(delays, values)
    {
        _delays = delays;
        _values = values;

        m_dim = 3;
    }

    double CostFunction(std::vector<double> inputs, double delay) const override
    {
        return (InvIt(inputs[2]) * exp(-exp(inputs[0]) * pow(delay, exp(inputs[1]))));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(3);

        constr[0] = DefaultConstraints();
        constr[1] = DefaultConstraints();
        constr[2] = DefaultConstraints();

        return constr;
    }
};

#endif // EVOLUTIONFUNCTIONS_H
