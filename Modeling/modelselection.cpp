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
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "interpolation.h"
#include "optimization.h"
#include "integration.h"
#include "stdafx.h"

#include "Controls/chartwindow.h"

using namespace alglib;

/**
 * @brief exponential_discounting
 * @param c
 * @param x
 * @param func
 */
void exponential_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = exp(-exp(c[0])*x[0]);
}

/**
 * @brief exponential_discounting_grad
 * @param c
 * @param x
 * @param func
 * @param grad
 */
void exponential_discounting_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = exp(-exp(c[0])*x[0]);
    grad[0] = -(exp(-exp(c[0]) * x[0]) * (exp(c[0]) * x[0]));
}

/**
 * @brief exponential_discounting_hessian
 * @param c
 * @param x
 * @param func
 * @param grad
 * @param hess
 */
void exponential_discounting_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = exp(-exp(c[0])*x[0]);
    grad[0] = -(exp(-exp(c[0]) * x[0]) * (exp(c[0]) * x[0]));
    hess[0][0] = -(exp(-exp(c[0]) * x[0]) * (exp(c[0]) * x[0]) - exp(-exp(c[0]) * x[0]) * (exp(c[0]) * x[0]) * (exp(c[0]) * x[0]));
}

/**
 * @brief exponential_integration
 * @param x
 * @param y
 * @param ptr
 */
void exponential_integration(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    y = exp(-exp(k)*x);
}

/**
 * @brief exponential_integration_log10
 * @param x
 * @param y
 * @param ptr
 */
void exponential_integration_log10(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    y = exp(-exp(k)*pow(10,x));
}

/**
 * @brief parabolic_discounting
 * @param c
 * @param x
 * @param func
 */
void parabolic_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = 1.0 - (exp(c[0]) * pow(x[0], 2));
}

/**
 * @brief parabolic_discounting_grad
 * @param c
 * @param x
 * @param func
 * @param grad
 */
void parabolic_discounting_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = 1.0 - (exp(c[0]) * pow(x[0], 2));
    grad[0] = -(exp(c[0]) * pow(x[0],2));
}

/**
 * @brief parabolic_discounting_hessian
 * @param c
 * @param x
 * @param func
 * @param grad
 * @param hess
 */
void parabolic_discounting_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = 1.0 - (exp(c[0]) * pow(x[0], 2));
    grad[0] = -(exp(c[0]) * pow(x[0],2));
    hess[0][0] = -(exp(c[0]) * pow(x[0],2));
}

/**
 * @brief parabolic_integration
 * @param x
 * @param y
 * @param ptr
 */
void parabolic_integration(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    y = 1.0 - (exp(k) * pow(x, 2));
}

/**
 * @brief parabolic_integration_log10
 * @param x
 * @param y
 * @param ptr
 */
void parabolic_integration_log10(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    y = 1.0 - (exp(k) * pow(pow(10,x), 2));
}

/**
 * @brief hyperbolic_discounting
 * @param c
 * @param x
 * @param func
 */
void hyperbolic_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = pow((1+exp(c[0])*x[0]), -1);
}

/**
 * @brief hyperbolic_discounting_grad
 * @param c
 * @param x
 * @param func
 * @param grad
 */
void hyperbolic_discounting_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = pow((1+exp(c[0])*x[0]), -1);
    grad[0] = pow((1 + exp(c[0]) * x[0]),((-1) - 1)) * ((-1) * (exp(c[0]) * x[0]));
}

/**
 * @brief hyperbolic_discounting_hessian
 * @param c
 * @param x
 * @param func
 * @param grad
 * @param hess
 */
void hyperbolic_discounting_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = pow((1+exp(c[0])*x[0]), -1);
    grad[0] = pow((1 + exp(c[0]) * x[0]),((-1) - 1)) * ((-1) * (exp(c[0]) * x[0]));
    hess[0][0] = pow((1 + exp(c[0]) * x[0]),(((-1) - 1) - 1)) * (((-1) - 1) * (exp(c[0]) * x[0])) *
            ((-1) * (exp(c[0]) * x[0])) + pow((1 + exp(c[0]) * x[0]),((-1) - 1)) * ((-1) * (exp(c[0]) * x[0]));
}

/**
 * @brief hyperbolic_integration
 * @param x
 * @param y
 * @param ptr
 */
void hyperbolic_integration(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    y = pow((1+exp(k)*x), -1);
}

/**
 * @brief hyperbolic_integration_log10
 * @param x
 * @param y
 * @param ptr
 */
void hyperbolic_integration_log10(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    y = pow((1+exp(k)*pow(10,x)), -1);
}

/**
 * @brief generalized_hyperboloid_discounting
 * @param c
 * @param x
 * @param func
 */
void generalized_hyperboloid_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = pow((1 + x[0] * exp(c[0])),(-exp(c[1]) / exp(c[0])));
}

/**
 * @brief generalized_hyperboloid_discounting_grad
 * @param c
 * @param x
 * @param func
 * @param grad
 */
void generalized_hyperboloid_discounting_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = pow((1 + x[0] * exp(c[0])),(-exp(c[1]) / exp(c[0])));

    grad[0] = pow((1 + x[0] * exp(c[0])), ((-exp(c[1])/exp(c[0])) - 1)) *
              ((-exp(c[1])/exp(c[0])) * (x[0] * exp(c[0]))) +
              pow((1 + x[0] * exp(c[0])), (-exp(c[1])/exp(c[0]))) *
              (log((1 + x[0] * exp(c[0]))) * (exp(c[1]) * exp(c[0])/pow(exp(c[0]),2)));

    grad[1] = -(pow((1 + x[0] * exp(c[0])), (-exp(c[1])/exp(c[0]))) *
               (log((1 + x[0] * exp(c[0]))) * (exp(c[1])/exp(c[0]))));
}

/**
 * @brief generalized_hyperboloid_discounting_hessian
 * @param c
 * @param x
 * @param func
 * @param grad
 * @param hess
 */
void generalized_hyperboloid_discounting_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = pow((1 + x[0] * exp(c[0])),(-exp(c[1]) / exp(c[0])));

    grad[0] = pow((1 + x[0] * exp(c[0])),((-exp(c[1])/exp(c[0])) - 1)) *
            ((-exp(c[1])/exp(c[0])) * (x[0] * exp(c[0]))) +
            pow((1 + x[0] * exp(c[0])), (-exp(c[1])/exp(c[0]))) *
            (log((1 + x[0] * exp(c[0]))) * (exp(c[1]) * exp(c[0])/pow(exp(c[0]),2)));

    grad[1] = -(pow((1 + x[0] * exp(c[0])),(-exp(c[1])/exp(c[0]))) *
            (log((1 + x[0] * exp(c[0]))) *
            (exp(c[1])/exp(c[0]))));

    hess[0][0] = (pow((1 + x[0] * exp(c[0])),(((-exp(c[2])/exp(c[0])) - 1) - 1)) *
            (((-exp(c[2])/exp(c[0])) - 1) * (x[0] * exp(c[0]))) +
            pow((1 + x[0] * exp(c[0])),((-exp(c[2])/exp(c[0])) - 1)) *
            (log((1 + x[0] * exp(c[0]))) * (exp(c[2]) * exp(c[0])/pow(exp(c[0]),2)))) *
            ((-exp(c[2])/exp(c[0])) * (x[0] * exp(c[0]))) +
            pow((1 + x[0] * exp(c[0])),((-exp(c[2])/exp(c[0])) - 1)) *
            (exp(c[2]) * exp(c[0])/pow(exp(c[0]),2) *
                (x[0] * exp(c[0])) + (-exp(c[2])/exp(c[0])) * (x[0] * exp(c[0]))) +
            ((pow((1 + x[0] * exp(c[0])),((-exp(c[2])/exp(c[0])) - 1)) *
            ((-exp(c[2])/exp(c[0])) * (x[0] * exp(c[0]))) +
            pow((1 + x[0] * exp(c[0])),(-exp(c[2])/exp(c[0]))) *
            (log((1 + x[0] * exp(c[0]))) * (exp(c[2]) * exp(c[0])/pow(exp(c[0]),2)))) *
            (log((1 + x[0] * exp(c[0]))) * (exp(c[2]) * exp(c[0])/pow(exp(c[0]),2))) +
            pow((1 + x[0] * exp(c[0])),(-exp(c[2])/exp(c[0]))) *
            (x[0] * exp(c[0])/(1 + x[0] * exp(c[0])) *
            (exp(c[2]) * exp(c[0])/pow(exp(c[0]),2)) +
            log((1 + x[0] * exp(c[0]))) *
            (exp(c[2]) * exp(c[0])/pow(exp(c[0]),2) - exp(c[2]) * exp(c[0]) *
            (2 * (exp(c[0]) * exp(c[0])))/pow((pow(exp(c[0]),2)),2))));

    hess[0][1] = -((pow((1 + x[0] * exp(c[0])),((-exp(c[1])/exp(c[0])) - 1)) *
                    ((-exp(c[1])/exp(c[0])) * (x[0] * exp(c[0]))) +
                    pow((1 + x[0] * exp(c[0])),(-exp(c[1])/exp(c[0]))) *
                    (log((1 + x[0] * exp(c[0]))) * (exp(c[1]) * exp(c[0])/pow(exp(c[0]),2)))) *
                    (log((1 + x[0] * exp(c[0]))) * (exp(c[1])/exp(c[0]))) +
                    pow((1 + x[0] * exp(c[0])),(-exp(c[1])/exp(c[0]))) *
                    (x[0] * exp(c[0])/(1 + x[0] * exp(c[0])) *
                    (exp(c[1])/exp(c[0])) -
                    log((1 + x[0] * exp(c[0]))) *
                    (exp(c[1]) * exp(c[0])/pow(exp(c[0]),2))));

    hess[1][0] = pow((1 + x[0] * exp(c[0])),(-exp(c[1])/exp(c[0]))) *
                 (log((1 + x[0] * exp(c[0]))) * (exp(c[1]) * exp(c[0])/pow(exp(c[0]),2))) -
                 pow((1 + x[0] * exp(c[0])),(-exp(c[1])/exp(c[0]))) *
                 (log((1 + x[0] * exp(c[0]))) * (exp(c[1])/exp(c[0]))) *
                 (log((1 + x[0] * exp(c[0]))) * (exp(c[1]) * exp(c[0])/pow(exp(c[0]),2))) -
                 (
                    pow((1 + x[0] * exp(c[0])),((-exp(c[1])/exp(c[0])) - 1)) *
                    (exp(c[1])/exp(c[0]) * (x[0] * exp(c[0]))) +
                    pow((1 + x[0] * exp(c[0])),((-exp(c[1])/exp(c[0])) - 1)) *
                    (log((1 + x[0] * exp(c[0]))) * (exp(c[1])/exp(c[0]))) *
                    ((-exp(c[1])/exp(c[0])) * (x[0] * exp(c[0])))
                );

    hess[1][1] = -(pow((1 + x[0] * exp(c[0])),(-exp(c[1])/exp(c[0]))) *
                (log((1 + x[0] * exp(c[0]))) * (exp(c[1])/exp(c[0]))) -
                pow((1 + x[0] * exp(c[0])),(-exp(c[1])/exp(c[0]))) *
                (log((1 + x[0] * exp(c[0]))) * (exp(c[1])/exp(c[0]))) *
                (log((1 + x[0] * exp(c[0]))) * (exp(c[1])/exp(c[0]))));
}

/**
 * @brief generalized_hyperboloid_integration
 * @param x
 * @param y
 * @param ptr
 */
void generalized_hyperboloid_integration(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double lnk = param->at(0);
    double beta = param->at(1);

    y = pow((1 + x * exp(lnk)),(-exp(beta) / exp(lnk)));
}

/**
 * @brief generalized_hyperboloid_integration_log10
 * @param x
 * @param y
 * @param ptr
 */
void generalized_hyperboloid_integration_log10(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double lnk = param->at(0);
    double beta = param->at(1);

    y = pow((1 + pow(10,x) * exp(lnk)),(-exp(beta) / exp(lnk)));
}

/**
 * @brief quasi_hyperboloid_discounting
 * @param c
 * @param x
 * @param func
 */
void quasi_hyperboloid_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = c[0] * pow(c[1], x[0]);
}

/**
 * @brief quasi_hyperboloid_discounting_grad
 * @param c
 * @param x
 * @param func
 * @param grad
 */
void quasi_hyperboloid_discounting_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = c[0] * pow(c[1], x[0]);

    grad[0] = pow(c[1], x[0]);
    grad[1] = c[0] * (pow(c[1],(x[0] - 1)) * x[0]);
}

/**
 * @brief quasi_hyperboloid_discounting_hessian
 * @param c
 * @param x
 * @param func
 * @param grad
 * @param hess
 */
void quasi_hyperboloid_discounting_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = c[0] * pow(c[1], x[0]);

    grad[0] = pow(c[1], x[0]);
    grad[1] = c[0] * (pow(c[1],(x[0] - 1)) * x[0]);

    hess[0][0] = 0;
    hess[0][1] = pow(c[1], (x[0] - 1)) * x[0];

    hess[1][0] = (pow(c[1], (x[0] - 1)) * x[0]);
    hess[1][1] = c[0] * (pow(c[1],((x[0] - 1) - 1)) * (x[0] - 1) * x[0]);
}

/**
 * @brief quasi_hyperboloid_integration
 * @param x
 * @param y
 * @param ptr
 */
void quasi_hyperboloid_integration(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double b = param->at(0);
    double d = param->at(1);
    y = b * pow(d, x);
}

/**
 * @brief quasi_hyperboloid_integration_log10
 * @param x
 * @param y
 * @param ptr
 */
void quasi_hyperboloid_integration_log10(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double b = param->at(0);
    double d = param->at(1);
    y = b * pow(d, pow(10,x));
}

/**
 * @brief hyperboloid_myerson_discounting
 * @param c
 * @param x
 * @param func
 */
void hyperboloid_myerson_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = pow((1+exp(c[0])*x[0]), -c[1]);
}

/**
 * @brief hyperboloid_myerson_discounting_grad
 * @param c
 * @param x
 * @param func
 * @param grad
 */
void hyperboloid_myerson_discounting_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = pow((1+exp(c[0])*x[0]), -c[1]);

    grad[0] = pow((1 + exp(c[0]) * x[0]),((-c[1]) - 1)) * ((-c[1]) * (exp(c[0]) * x[0]));
    grad[1] = -(pow((1 + exp(c[0]) * x[0]),(-c[1])) * log((1 + exp(c[0]) * x[0])));
}

/**
 * @brief hyperboloid_myerson_discounting_hessian
 * @param c
 * @param x
 * @param func
 * @param grad
 * @param hess
 */
void hyperboloid_myerson_discounting_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = pow((1+exp(c[0])*x[0]), -c[1]);

    grad[0] = pow((1 + exp(c[0]) * x[0]),((-c[1]) - 1)) * ((-c[1]) * (exp(c[0]) * x[0]));
    grad[1] = -(pow((1 + exp(c[0]) * x[0]),(-c[1])) * log((1 + exp(c[0]) * x[0])));

    hess[0][0] = pow((1 + exp(c[0]) * x[0]),(((-c[1]) - 1) - 1)) *
            (((-c[1]) - 1) * (exp(c[0]) * x[0])) *
            ((-c[1]) * (exp(c[0]) * x[0])) +
            pow((1 + exp(c[0]) * x[0]),((-c[1]) - 1)) *
            ((-c[1]) * (exp(c[0]) * x[0]));

    hess[0][1] = -(pow((1 + exp(c[0]) * x[0]),((-c[1]) - 1)) *
                   ((-c[1]) * (exp(c[0]) * x[0])) * log((1 + exp(c[0]) * x[0])) +
                   pow((1 + exp(c[0]) * x[0]),(-c[1])) * (exp(c[0]) * x[0]/(1 + exp(c[0]) * x[0])));

    hess[1][0] = -(pow((1 + exp(c[0]) * x[0]),((-c[1]) - 1)) *
                   (exp(c[0]) * x[0]) + pow((1 + exp(c[0]) * x[0]),((-c[1]) - 1)) *
                   log((1 + exp(c[0]) * x[0])) * ((-c[1]) * (exp(c[0]) * x[0])));

    hess[1][1] = pow((1 + exp(c[0]) * x[0]),(-c[1])) * log((1 + exp(c[0]) * x[0])) * log((1 + exp(c[0]) * x[0]));
}

/**
 * @brief hyperboloid_myerson_integration
 * @param x
 * @param y
 * @param ptr
 */
void hyperboloid_myerson_integration(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = pow((1+exp(k)*x), -s);
}

/**
 * @brief hyperboloid_myerson_integration_log10
 * @param x
 * @param y
 * @param ptr
 */
void hyperboloid_myerson_integration_log10(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = pow((1+exp(k)*pow(10,x)), -s);
}

/**
 * @brief hyperboloid_rachlin_discounting
 * @param c
 * @param x
 * @param func
 */
void hyperboloid_rachlin_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = pow((1+exp(c[0])*pow(x[0], c[1])), -1);
}

/**
 * @brief hyperboloid_rachlin_discounting_grad
 * @param c
 * @param x
 * @param func
 * @param grad
 */
void hyperboloid_rachlin_discounting_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = pow((1+exp(c[0])*pow(x[0], c[1])), -1);

    grad[0] = pow((1 + exp(c[0]) * (pow(x[0], c[1]))),((-1) - 1)) * ((-1) * (exp(c[0]) * (pow(x[0], c[1]))));
    grad[1] = pow((1 + exp(c[0]) * (pow(x[0], c[1]))),((-1) - 1)) * ((-1) * (exp(c[0]) * (pow(x[0], c[1]) * log(x[0]))));
}

/**
 * @brief hyperboloid_rachlin_discounting_hessian
 * @param c
 * @param x
 * @param func
 * @param grad
 * @param hess
 */
void hyperboloid_rachlin_discounting_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = pow((1+exp(c[0])*pow(x[0], c[1])), -1);

    grad[0] = pow((1 + exp(c[0]) * (pow(x[0], c[1]))),((-1) - 1)) * ((-1) * (exp(c[0]) * (pow(x[0], c[1]))));
    grad[1] = pow((1 + exp(c[0]) * (pow(x[0], c[1]))),((-1) - 1)) * ((-1) * (exp(c[0]) * (pow(x[0], c[1]) * log(x[0]))));

    hess[0][0] = pow((1 + exp(c[0]) * (pow(x[0],c[1]))),(((-1) - 1) - 1)) *
            (((-1) - 1) * (exp(c[0]) * (pow(x[0],c[1])))) *
            ((-1) * (exp(c[0]) * (pow(x[0],c[1])))) +
            pow((1 + exp(c[0]) * (pow(x[0],c[1]))),((-1) - 1)) *
            ((-1) * (exp(c[0]) * (pow(x[0],c[1]))));

    hess[0][1] = pow((1 + exp(c[0]) * (pow(x[0],c[1]))),(((-1) - 1) - 1)) *
            (((-1) - 1) * (exp(c[0]) * (pow(x[0],c[1])))) *
            ((-1) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0])))) +
            pow((1 + exp(c[0]) * (pow(x[0],c[1]))),((-1) - 1)) *
            ((-1) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0]))));

    hess[1][0] = pow((1 + exp(c[0]) * (pow(x[0],c[1]))),(((-1) - 1) - 1)) *
            (((-1) - 1) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0])))) *
            ((-1) * (exp(c[0]) * (pow(x[0],c[1])))) +
            pow((1 + exp(c[0]) * (pow(x[0],c[1]))),((-1) - 1)) *
            ((-1) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0]))));

    hess[1][1] = pow((1 + exp(c[0]) * (pow(x[0],c[1]))),(((-1) - 1) - 1)) *
            (((-1) - 1) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0])))) *
            ((-1) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0])))) +
            pow((1 + exp(c[0]) * (pow(x[0],c[1]))),((-1) - 1)) *
            ((-1) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0]) * log(x[0]))));
}

/**
 * @brief hyperboloid_rachlin_integration
 * @param x
 * @param y
 * @param ptr
 */
void hyperboloid_rachlin_integration(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = pow((1+exp(k)*pow(x, s)), -1);
}

/**
 * @brief hyperboloid_rachlin_integration_log10
 * @param x
 * @param y
 * @param ptr
 */
void hyperboloid_rachlin_integration_log10(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = pow((1+exp(k)*pow(pow(10,x), s)), -1);
}

/**
 * @brief ebert_prelec_discounting
 * @param c
 * @param x
 * @param func
 */
void ebert_prelec_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = exp(-pow((exp(c[0])*x[0]),c[1]));
}

/**
 * @brief ebert_prelec_discounting_grad
 * @param c
 * @param x
 * @param func
 * @param grad
 */
void ebert_prelec_discounting_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = exp(-pow((exp(c[0])*x[0]), c[1]));

    grad[0] = -(exp(-pow((exp(c[0]) * x[0]), c[1])) * (pow((exp(c[0]) * x[0]), (c[1] - 1)) * (c[1] * (exp(c[0]) * x[0]))));
    grad[1] = -(exp(-pow((exp(c[0]) * x[0]), c[1])) * (pow((exp(c[0]) * x[0]), c[1]) * log((exp(c[0]) * x[0]))));
}

/**
 * @brief ebert_prelec_discounting_hessian
 * @param c
 * @param x
 * @param func
 * @param grad
 * @param hess
 */
void ebert_prelec_discounting_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = exp(-pow((exp(c[0])*x[0]), c[1]));

    grad[0] = -(exp(-pow((exp(c[0]) * x[0]), c[1])) * (pow((exp(c[0]) * x[0]), (c[1] - 1)) * (c[1] * (exp(c[0]) * x[0]))));
    grad[1] = -(exp(-pow((exp(c[0]) * x[0]), c[1])) * (pow((exp(c[0]) * x[0]), c[1]) * log((exp(c[0]) * x[0]))));

    hess[0][0] = -(exp(-pow((exp(c[0]) * x[0]),c[1])) *
                   (pow((exp(c[0]) * x[0]),((c[1] - 1) - 1)) *
                    ((c[1] - 1) * (exp(c[0]) * x[0])) *
                    (c[1] * (exp(c[0]) * x[0])) + pow((exp(c[0]) * x[0]),(c[1] - 1)) *
                    (c[1] * (exp(c[0]) * x[0]))) - exp(-pow((exp(c[0]) * x[0]),c[1])) *
                   (pow((exp(c[0]) * x[0]),(c[1] - 1)) * (c[1] * (exp(c[0]) * x[0]))) *
                   (pow((exp(c[0]) * x[0]),(c[1] - 1)) * (c[1] * (exp(c[0]) * x[0]))));

    hess[0][1] = -(exp(-pow((exp(c[0]) * x[0]), c[1])) *
                   (pow((exp(c[0]) * x[0]),(c[1] - 1)) *
                  (c[1] * (exp(c[0]) * x[0])) * log((exp(c[0]) * x[0])) +
                  pow((exp(c[0]) * x[0]),c[1]) * (exp(c[0]) * x[0]/(exp(c[0]) * x[0]))) -
                   exp(-pow((exp(c[0]) * x[0]),c[1])) * (pow((exp(c[0]) * x[0]),(c[1] - 1)) *
                   (c[1] * (exp(c[0]) * x[0]))) * (pow((exp(c[0]) * x[0]),c[1]) * log((exp(c[0]) * x[0]))));

    hess[1][0] = -(exp(-pow((exp(c[0]) * x[0]),c[1])) * (pow((exp(c[0]) * x[0]),(c[1] - 1)) *
                 log((exp(c[0]) * x[0])) * (c[1] * (exp(c[0]) * x[0])) +
                 pow((exp(c[0]) * x[0]),(c[1] - 1)) * (exp(c[0]) * x[0])) -
                   exp(-pow((exp(c[0]) * x[0]),c[1])) * (pow((exp(c[0]) * x[0]),c[1]) * log((exp(c[0]) * x[0]))) *
                   (pow((exp(c[0]) * x[0]),(c[1] - 1)) * (c[1] * (exp(c[0]) * x[0]))));

    hess[1][1] = -(exp(-pow((exp(c[0]) * x[0]),c[1])) * (pow((exp(c[0]) * x[0]),c[1]) *
                 log((exp(c[0]) * x[0])) * log((exp(c[0]) * x[0]))) - exp(-pow((exp(c[0]) * x[0]),c[1])) *
                   (pow((exp(c[0]) * x[0]),c[1]) * log((exp(c[0]) * x[0])))
                   * (pow((exp(c[0]) * x[0]),c[1]) * log((exp(c[0]) * x[0]))));
}

/**
 * @brief ebert_prelec_integration
 * @param x
 * @param y
 * @param ptr
 */
void ebert_prelec_integration(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = exp(-pow((exp(k)*x), s));
}

/**
 * @brief ebert_prelec_integration_log10
 * @param x
 * @param y
 * @param ptr
 */
void ebert_prelec_integration_log10(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    y = exp(-pow((exp(k)*pow(10,x)), s));
}

/**
 * @brief bleichrodt_discounting
 * @param c
 * @param x
 * @param func
 */
void bleichrodt_discounting(const real_1d_array &c, const real_1d_array &x, double &func, void *)
{
    func = c[2] * exp(-exp(c[0])*pow(x[0],c[1]));
}

/**
 * @brief bleichrodt_discounting_grad
 * @param c
 * @param x
 * @param func
 * @param grad
 */
void bleichrodt_discounting_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *)
{
    func = c[2] * exp(-exp(c[0])*pow(x[0],c[1]));
    grad[0] = -(c[2] * (exp(-exp(c[0]) * pow(x[0], c[1])) * (exp(c[0]) * pow(x[0],c[1]))));
    grad[1] = -(c[2] * (exp(-exp(c[0]) * pow(x[0], c[1])) * (exp(c[0]) * (pow(x[0], c[1]) * log(x[0])))));
    grad[2] = exp(-exp(c[0]) * pow(x[0], c[1]));
}

/**
 * @brief bleichrodt_discounting_hessian
 * @param c
 * @param x
 * @param func
 * @param grad
 * @param hess
 */
void bleichrodt_discounting_hessian(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *)
{
    func = c[2] * exp(-exp(c[0])*pow(x[0],c[1]));

    grad[0] = -(c[2] * (exp(-exp(c[0]) * pow(x[0], c[1])) * (exp(c[0]) * pow(x[0],c[1]))));
    grad[1] = -(c[2] * (exp(-exp(c[0]) * pow(x[0], c[1])) * (exp(c[0]) * (pow(x[0], c[1]) * log(x[0])))));
    grad[2] = exp(-exp(c[0]) * pow(x[0], c[1]));

    hess[0][0] = -(c[2] * (exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) *
            pow(x[0],c[1])) - exp(-exp(c[0]) * pow(x[0],c[1])) *
            (exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * pow(x[0],c[1]))));

    hess[0][1] = -(c[2] * (exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0]))) -
                           exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) *
                           (pow(x[0],c[1]) * log(x[0])))));

    hess[0][2] = -(exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * pow(x[0],c[1])));

    hess[1][0] = -(c[2] * (exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0]))) -
                           exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0]))) * (exp(c[0]) * pow(x[0],c[1]))));

    hess[1][1] = -(c[2] * (exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0]) *
                 log(x[0]))) - exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0]))) *
                 (exp(c[0]) * (pow(x[0],c[1]) * log(x[0])))));

    hess[1][2] = -(exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0]))));

    hess[2][0] = -(exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * pow(x[0],c[1])));

    hess[2][1] = -(exp(-exp(c[0]) * pow(x[0],c[1])) * (exp(c[0]) * (pow(x[0],c[1]) * log(x[0]))));

    hess[2][2] = 0;
}

/**
 * @brief bleichrodt_integration
 * @param x
 * @param y
 * @param ptr
 */
void bleichrodt_integration(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    double beta = param->at(2);

    y = beta * exp(-exp(k)*pow(x,s));
}

/**
 * @brief bleichrodt_integration_log10
 * @param x
 * @param y
 * @param ptr
 */
void bleichrodt_integration_log10(double x, double, double, double &y, void *ptr)
{
    QList<double> *param = (QList<double> *) ptr;
    double k = param->at(0);
    double s = param->at(1);
    double beta = param->at(2);

    y = beta * exp(-exp(k)*pow(pow(10,x),s));
}

/**
 * @brief ModelSelection::SetX
 * @param mString
 */
void ModelSelection::SetX(const char *mString)
{
    x = mString;
}

/**
 * @brief ModelSelection::SetY
 * @param mString
 */
void ModelSelection::SetY(const char *mString)
{
    y = mString;
}

/**
 * @brief ModelSelection::SetStarts
 * @param mString
 */
void ModelSelection::SetStarts(const char *mString)
{
    c = mString;
}

/**
 * @brief ModelSelection::SetLowerUpperBounds
 * @param mUpperString
 * @param mLowerString
 */
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
    ErrorResidual.clear();

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
        ErrorResidual.append(((double) y[i] - AVE));
        SSR += pow(ErrorResidual[i], 2);
    }

    S2 = SSR / N;

    L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

    DF = 2;

    aicNoise = (-2 * log(L)) + (2 * DF);
    bicNoise = -2 * log(L) + log(N) * DF;
    NoiseRMSE = sqrt(S2);
}

/** Exponentials
 * @brief
 */
void ModelSelection::FitExponential(const char *mStarts)
{
    ErrorResidual.clear();

    aicExponential = NULL;
    bicExponential = NULL;
    fitExponentialK = NULL;

    if (fittingAlgorithm == FittingAlgorithm::DifferentialEvolution)
    {
        QVector<double> xValues = GetXVector();
        QVector<double> yValues = GetYVector();

        ExponentialModel exponentialFunction(xValues, yValues);

        de::DifferentialEvolution de(exponentialFunction, 100);

        de.Optimize(1000, false);

        std::vector<double> result = de.GetBestAgent();

        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = (exp(-exp( (double) result[0])* (double) x[i][0]));

            ErrorResidual.append(((double) y[i] - holder));

            SSR += pow(ErrorResidual[i], 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 2;

        aicExponential = (-2 * log(L)) + (2 * DF);
        bicExponential = -2 * log(L) + log(N) * DF;
        fitExponentialK = (double) result[0];

        if (SSR > 0)
        {
            rmseExponential = sqrt(SSR/(double) N);
        }
    }
    else
    {
        SetStarts(mStarts);

        if (fittingAlgorithm == FittingAlgorithm::Function)
        {
            lsfitcreatef(x,
                         y,
                         c,
                         diffstep,
                         state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             exponential_discounting);

        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
        {
            lsfitcreatefg(x,
                          y,
                          c,
                          true,
                          state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             exponential_discounting,
                             exponential_discounting_grad);
        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
        {
            lsfitcreatefgh(x,
                           y,
                           c,
                           state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             exponential_discounting,
                             exponential_discounting_grad,
                             exponential_discounting_hessian);

        }

        lsfitresults(state, info, c, rep);

        if ((int) info == 2 || (int) info == 5)
        {
            N = y.length();
            SSR = 0;

            for (int i = 0; i < N; i++)
            {
                holder = (exp(-exp( (double) c[0])* (double) x[i][0]));

                ErrorResidual.append(((double) y[i] - holder));

                SSR += pow(ErrorResidual[i], 2);
            }

            S2 = SSR / N;

            L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

            DF = 2;

            aicExponential = (-2 * log(L)) + (2 * DF);
            bicExponential = -2 * log(L) + log(N) * DF;
            fitExponentialK = (double) c[0];
        }
    }
}

/** Exponentials
 * @brief
 */
void ModelSelection::FitParabolic(const char *mStarts)
{
    ErrorResidual.clear();

    aicParabolic = NULL;
    bicParabolic = NULL;
    fitParabolicK = NULL;

    if (fittingAlgorithm == FittingAlgorithm::DifferentialEvolution)
    {
        QVector<double> xValues = GetXVector();
        QVector<double> yValues = GetYVector();

        ParabolicModel parabolicFunction(xValues, yValues);

        de::DifferentialEvolution de(parabolicFunction, 100);

        de.Optimize(1000, false);

        std::vector<double> result = de.GetBestAgent();

        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = 1.0 - (exp((double) c[0]) * pow(x[i][0], 2));

            ErrorResidual.append(((double) y[i] - holder));

            SSR += pow(ErrorResidual[i], 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 2;

        aicExponential = (-2 * log(L)) + (2 * DF);
        bicExponential = -2 * log(L) + log(N) * DF;
        fitExponentialK = (double) result[0];

        if (SSR > 0)
        {
            rmseExponential = sqrt(SSR/(double) N);
        }
    }
    else
    {
        SetStarts(mStarts);

        if (fittingAlgorithm == FittingAlgorithm::Function)
        {
            lsfitcreatef(x,
                         y,
                         c,
                         diffstep,
                         state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             parabolic_discounting);

        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
        {
            lsfitcreatefg(x,
                          y,
                          c,
                          true,
                          state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             parabolic_discounting,
                             parabolic_discounting_grad);
        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
        {
            lsfitcreatefgh(x,
                           y,
                           c,
                           state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             parabolic_discounting,
                             parabolic_discounting_grad,
                             parabolic_discounting_hessian);

        }

        lsfitresults(state, info, c, rep);

        if ((int) info == 2 || (int) info == 5)
        {
            N = y.length();
            SSR = 0;

            for (int i = 0; i < N; i++)
            {
                holder = 1.0 - (exp((double) c[0]) * pow(x[i][0], 2));

                ErrorResidual.append(((double) y[i] - holder));

                SSR += pow(ErrorResidual[i], 2);
            }

            S2 = SSR / N;

            L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

            DF = 2;

            aicParabolic = (-2 * log(L)) + (2 * DF);
            bicParabolic = -2 * log(L) + log(N) * DF;
            fitParabolicK = (double) c[0];
        }
    }
}

/** Hyperbolics
 * @brief
 */
void ModelSelection::FitHyperbolic(const char *mStarts)
{
    ErrorResidual.clear();

    aicHyperbolic = NULL;
    bicHyperbolic = NULL;
    fitHyperbolicK = NULL;

    if (fittingAlgorithm == FittingAlgorithm::DifferentialEvolution)
    {
        QVector<double> xValues = GetXVector();
        QVector<double> yValues = GetYVector();

        HyperbolicModel hyperbolicFunction(xValues, yValues);

        de::DifferentialEvolution de(hyperbolicFunction, 100);

        de.Optimize(1000, false);

        std::vector<double> result = de.GetBestAgent();

        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = pow((1+exp( (double) result[0])* (double) x[i][0]), -1);

            ErrorResidual.append(((double) y[i] - holder));

            SSR += pow(ErrorResidual[i], 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 2;

        aicHyperbolic = (-2 * log(L)) + (2 * DF);
        bicHyperbolic = -2 * log(L) + log(N) * DF;
        fitHyperbolicK = (double) result[0];

        if (SSR > 0)
        {
            rmseHyperbolic = sqrt(SSR/(double) N);
        }
    }
    else
    {
        SetStarts(mStarts);

        if (fittingAlgorithm == FittingAlgorithm::Function)
        {
            lsfitcreatef(x,
                         y,
                         c,
                         diffstep,
                         state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             hyperbolic_discounting);

        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
        {
            lsfitcreatefg(x,
                          y,
                          c,
                          true,
                          state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             hyperbolic_discounting,
                             hyperbolic_discounting_grad);
        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
        {
            lsfitcreatefgh(x,
                           y,
                           c,
                           state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             hyperbolic_discounting,
                             hyperbolic_discounting_grad,
                             hyperbolic_discounting_hessian);
        }

        lsfitresults(state, info, c, rep);

        if ((int) info == 2 || (int) info == 5)
        {
            N = y.length();
            SSR = 0;

            for (int i = 0; i < N; i++)
            {
                holder = pow((1+exp( (double) c[0])* (double) x[i][0]), -1);

                ErrorResidual.append(((double) y[i] - holder));

                SSR += pow(ErrorResidual[i], 2);
            }

            S2 = SSR / N;

            L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

            DF = 2;

            aicHyperbolic = (-2 * log(L)) + (2 * DF);
            bicHyperbolic = -2 * log(L) + log(N) * DF;
            fitHyperbolicK = (double) c[0];
        }
    }
}

/** Beta delta
  *  @brief
  */
void ModelSelection::FitQuasiHyperbolic(const char *mStarts)
{
    ErrorResidual.clear();

    aicQuasiHyperbolic = NULL;
    bicQuasiHyperbolic = NULL;
    fitQuasiHyperbolicBeta = NULL;
    fitQuasiHyperbolicDelta = NULL;

    if (fittingAlgorithm == FittingAlgorithm::DifferentialEvolution)
    {
        QVector<double> xValues = GetXVector();
        QVector<double> yValues = GetYVector();

        BetaDeltaModel quasiHyperbolicFunction(xValues, yValues);

        de::DifferentialEvolution de(quasiHyperbolicFunction, 100);

        de.Optimize(1000, false);

        std::vector<double> result = de.GetBestAgent();

        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = (double) result[0] * pow( (double) result[1], (double) x[i][0]);

            ErrorResidual.append(((double) y[i] - holder));

            SSR += pow(ErrorResidual[i], 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 3;

        aicQuasiHyperbolic = (-2 * log(L)) + (2 * DF);
        bicQuasiHyperbolic = -2 * log(L) + log(N) * DF;
        fitQuasiHyperbolicBeta = (double) result[0];
        fitQuasiHyperbolicDelta = (double) result[1];

        if (SSR > 0)
        {
            rmseQuasiHyperbolic = sqrt(SSR/(double) N);
        }
    }
    else
    {
        SetStarts(mStarts);

        SetLowerUpperBounds("[0.9999999999999999, 0.9999999999999999]",
                            "[0.0000000000000001, 0.0000000000000001]");

        if (fittingAlgorithm == FittingAlgorithm::Function)
        {
            lsfitcreatef(x,
                         y,
                         c,
                         diffstep,
                         state);

            lsfitsetbc(state,
                       bndl,
                       bndu);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             quasi_hyperboloid_discounting);

        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
        {
            lsfitcreatefg(x,
                          y,
                          c,
                          true,
                          state);

            lsfitsetbc(state,
                       bndl,
                       bndu);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             quasi_hyperboloid_discounting,
                             quasi_hyperboloid_discounting_grad);
        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
        {
            lsfitcreatefgh(x,
                           y,
                           c,
                           state);

            lsfitsetbc(state,
                       bndl,
                       bndu);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             quasi_hyperboloid_discounting,
                             quasi_hyperboloid_discounting_grad,
                             quasi_hyperboloid_discounting_hessian);
        }

        lsfitresults(state, info, c, rep);

        if ((int) info == 2 || (int) info == 5)
        {
            N = y.length();
            SSR = 0;

            for (int i = 0; i < N; i++)
            {
                holder = (double) c[0] * pow( (double) c[1], (double) x[i][0]);

                ErrorResidual.append(((double) y[i] - holder));

                SSR += pow(ErrorResidual[i], 2);
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
}

/** Hyperboloid Myerson
 * @brief
 */
void ModelSelection::FitMyerson(const char *mStarts)
{
    ErrorResidual.clear();

    aicMyerson = NULL;
    bicMyerson = NULL;
    fitMyersonK = NULL;
    fitMyersonS = NULL;

    if (fittingAlgorithm == FittingAlgorithm::DifferentialEvolution)
    {
        QVector<double> xValues = GetXVector();
        QVector<double> yValues = GetYVector();

        GreenMyersonModel greenMyersonFunction(xValues, yValues);

        de::DifferentialEvolution de(greenMyersonFunction, 100);

        de.Optimize(1000, false);

        std::vector<double> result = de.GetBestAgent();

        N = y.length();

        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = pow((1+exp( (double) result[0])* (double) x[i][0]),  (double) -exp(result[1]));

            ErrorResidual.append(((double) y[i] - holder));

            SSR += pow(ErrorResidual[i], 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 3;

        aicMyerson = (-2 * log(L)) + (2 * DF);
        bicMyerson = -2 * log(L) + log(N) * DF;
        fitMyersonK = (double) result[0];
        fitMyersonS = (double) exp(result[1]);

        if (SSR > 0)
        {
            rmseMyerson = sqrt(SSR/(double) N);
        }
    }
    else
    {
        SetStarts(mStarts);

        if (fittingAlgorithm == FittingAlgorithm::Function)
        {
            lsfitcreatef(x,
                         y,
                         c,
                         diffstep,
                         state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             hyperboloid_myerson_discounting);

        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
        {
            lsfitcreatefg(x,
                          y,
                          c,
                          true,
                          state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             hyperboloid_myerson_discounting,
                             hyperboloid_myerson_discounting_grad);
        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
        {
            lsfitcreatefgh(x,
                           y,
                           c,
                           state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             hyperboloid_myerson_discounting,
                             hyperboloid_myerson_discounting_grad,
                             hyperboloid_myerson_discounting_hessian);
        }

        lsfitresults(state, info, c, rep);

        if ((int) info == 2 || (int) info == 5)
        {
            N = y.length();

            SSR = 0;

            for (int i = 0; i < N; i++)
            {
                holder = pow((1+exp( (double) c[0])* (double) x[i][0]),  (double) -c[1]);

                ErrorResidual.append(((double) y[i] - holder));

                SSR += pow(ErrorResidual[i], 2);
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
}

/** Hyperboloid Rachlin
  *  @brief
  */
void ModelSelection::FitRachlin(const char *mStarts)
{
    ErrorResidual.clear();

    statusValue = NULL;
    aicRachlin = NULL;
    bicRachlin = NULL;
    fitRachlinK = NULL;
    fitRachlinS = NULL;

    if (fittingAlgorithm == FittingAlgorithm::DifferentialEvolution)
    {
        QVector<double> xValues = GetXVector();
        QVector<double> yValues = GetYVector();

        RachlinModel rachlinFunction(xValues, yValues);

        de::DifferentialEvolution de(rachlinFunction, 100);

        de.Optimize(1000, false);

        std::vector<double> result = de.GetBestAgent();

        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = pow((1+exp((double) result[0])*pow((double) x[i][0], exp(result[1]))), -1);

            ErrorResidual.append(((double) y[i] - holder));

            SSR += pow(ErrorResidual[i], 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 3;

        aicRachlin = (-2 * log(L)) + (2 * DF);
        bicRachlin = -2 * log(L) + log(N) * DF;
        fitRachlinK = (double) result[0];
        fitRachlinS = exp(result[1]);

        statusValue = -1;

        if (SSR > 0)
        {
            rmseRachlin = sqrt(SSR/(double) N);
        }
    }
    else
    {
        SetStarts(mStarts);

        if (fittingAlgorithm == FittingAlgorithm::Function)
        {
            lsfitcreatef(x,
                         y,
                         c,
                         diffstep,
                         state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             hyperboloid_rachlin_discounting);

        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
        {
            lsfitcreatefg(x,
                          y,
                          c,
                          true,
                          state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             hyperboloid_rachlin_discounting,
                             hyperboloid_rachlin_discounting_grad);
        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
        {
            lsfitcreatefgh(x,
                           y,
                           c,
                           state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             hyperboloid_rachlin_discounting,
                             hyperboloid_rachlin_discounting_grad,
                             hyperboloid_rachlin_discounting_hessian);
        }

        lsfitresults(state, info, c, rep);

        if ((int) GetInfo() == 2 || (int) GetInfo() == 5)
        {
            N = y.length();
            SSR = 0;

            for (int i = 0; i < N; i++)
            {
                holder = pow((1+exp( (double) c[0])*pow( (double) x[i][0], (double) c[1])), -1);

                ErrorResidual.append(((double) y[i] - holder));

                SSR += pow(ErrorResidual[i], 2);
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
}

/** Lewenstein & Prelec Model
  *  @brief
  */
void ModelSelection::FitGeneralizedHyperbolic(const char *mStarts)
{
    ErrorResidual.clear();

    aicGeneralizedHyperbolic = NULL;
    bicGeneralizedHyperbolic = NULL;
    fitGeneralizedHyperbolicK = NULL;
    fitGeneralizedHyperbolicBeta = NULL;

    if (fittingAlgorithm == FittingAlgorithm::DifferentialEvolution)
    {
        QVector<double> xValues = GetXVector();
        QVector<double> yValues = GetYVector();

        GeneralizedHyperbolicModel generalizedHyperbolicFunction(xValues, yValues);

        de::DifferentialEvolution de(generalizedHyperbolicFunction, 100);

        de.Optimize(1000, false);

        std::vector<double> result = de.GetBestAgent();

        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = pow((1 + x[i][0] * exp(result[0])),(-exp(result[1]) / exp(result[0])));

            ErrorResidual.append(((double) y[i] - holder));

            SSR += pow(ErrorResidual[i], 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 3;

        aicGeneralizedHyperbolic = (-2 * log(L)) + (2 * DF);
        bicGeneralizedHyperbolic = -2 * log(L) + log(N) * DF;
        fitGeneralizedHyperbolicK = (double) result[0];
        fitGeneralizedHyperbolicBeta = (double) result[1];

        if (SSR > 0)
        {
            rmseGeneralizedHyperbolic = sqrt(SSR/(double) N);
        }
    }
    else
    {
        SetStarts(mStarts);

        if (fittingAlgorithm == FittingAlgorithm::Function)
        {
            lsfitcreatef(x,
                         y,
                         c,
                         diffstep,
                         state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             generalized_hyperboloid_discounting);

        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient || fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
        {
            lsfitcreatefg(x,
                          y,
                          c,
                          true,
                          state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             generalized_hyperboloid_discounting,
                             generalized_hyperboloid_discounting_grad);
        }
        /*
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
        {
            lsfitcreatefgh(x,
                           y,
                           c,
                           state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             generalized_hyperboloid_discounting,
                             generalized_hyperboloid_discounting_grad,
                             generalized_hyperboloid_discounting_hessian);
        }
        */

        lsfitresults(state, info, c, rep);

        if ((int) GetInfo() == 2 || (int) GetInfo() == 5)
        {
            N = y.length();
            SSR = 0;

            for (int i = 0; i < N; i++)
            {
                holder = pow((1 + x[i][0] * exp(c[0])),(-exp(c[1]) / exp(c[0])));

                ErrorResidual.append(((double) y[i] - holder));

                SSR += pow(ErrorResidual[i], 2);
            }

            S2 = SSR / N;

            L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

            DF = 3;

            aicGeneralizedHyperbolic = (-2 * log(L)) + (2 * DF);
            bicGeneralizedHyperbolic = -2 * log(L) + log(N) * DF;
            fitGeneralizedHyperbolicK = (double) c[0];
            fitGeneralizedHyperbolicBeta = (double) c[1];
        }
    }
}

/** Ebert Model
  *  @brief
  */
void ModelSelection::FitEbertPrelec(const char *mStarts)
{
    ErrorResidual.clear();

    aicEbertPrelec = NULL;
    bicEbertPrelec = NULL;
    fitEbertPrelecK = NULL;
    fitEbertPrelecS = NULL;

    if (fittingAlgorithm == FittingAlgorithm::DifferentialEvolution)
    {
        QVector<double> xValues = GetXVector();
        QVector<double> yValues = GetYVector();

        EbertPrelecModel ebertPrelecFunction(xValues, yValues);

        de::DifferentialEvolution de(ebertPrelecFunction, 100);

        de.Optimize(1000, false);

        std::vector<double> result = de.GetBestAgent();

        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = exp(-pow((exp(result[0])*x[i][0]), exp(result[1])));

            ErrorResidual.append(((double) y[i] - holder));

            SSR += pow(ErrorResidual[i], 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 3;

        aicEbertPrelec = (-2 * log(L)) + (2 * DF);
        bicEbertPrelec = -2 * log(L) + log(N) * DF;
        fitEbertPrelecK = (double) result[0];
        fitEbertPrelecS = (double) exp(result[1]);

        if (SSR > 0)
        {
            rmseEbertPrelec = sqrt(SSR/(double) N);
        }
    }
    else
    {
        SetStarts(mStarts);

        if (fittingAlgorithm == FittingAlgorithm::Function)
        {
            lsfitcreatef(x,
                         y,
                         c,
                         diffstep,
                         state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             ebert_prelec_discounting);

        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
        {
            lsfitcreatefg(x,
                          y,
                          c,
                          true,
                          state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             ebert_prelec_discounting,
                             ebert_prelec_discounting_grad);
        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
        {
            lsfitcreatefgh(x,
                           y,
                           c,
                           state);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             ebert_prelec_discounting,
                             ebert_prelec_discounting_grad,
                             ebert_prelec_discounting_hessian);
        }

        lsfitresults(state, info, c, rep);

        if ((int) GetInfo() == 2 || (int) GetInfo() == 5)
        {
            N = y.length();
            SSR = 0;

            for (int i = 0; i < N; i++)
            {
                holder = exp(-pow((exp(c[0])*x[i][0]), c[1]));

                ErrorResidual.append(((double) y[i] - holder));

                SSR += pow(ErrorResidual[i], 2);
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
}

/** Bleichrodt Model
  *  @brief
  */
void ModelSelection::FitBleichrodt(const char *mStarts)
{
    ErrorResidual.clear();

    aicBleichrodt = NULL;
    bicBleichrodt = NULL;
    fitBleichrodtK = NULL;
    fitBleichrodtS = NULL;
    fitBleichrodtBeta = NULL;

    if (fittingAlgorithm == FittingAlgorithm::DifferentialEvolution)
    {
        QVector<double> xValues = GetXVector();
        QVector<double> yValues = GetYVector();

        BeleichrodtModel beleichrodtFunction(xValues, yValues);

        de::DifferentialEvolution de(beleichrodtFunction, 100);

        de.Optimize(1000, false);

        std::vector<double> result = de.GetBestAgent();

        N = y.length();
        SSR = 0;

        for (int i = 0; i < N; i++)
        {
            holder = result[2] * exp(-exp(result[0])*pow(x[i][0], exp(result[1])));

            ErrorResidual.append(((double) y[i] - holder));

            SSR += pow(ErrorResidual[i], 2);
        }

        S2 = SSR / N;

        L = pow((1.0 / sqrt(2 * M_PI * S2)), N) * exp(-SSR / (S2 * 2.0));

        DF = 4;

        aicBleichrodt = (-2 * log(L)) + (2 * DF);
        bicBleichrodt = -2 * log(L) + log(N) * DF;
        fitBleichrodtK = (double) result[0];
        fitBleichrodtS = (double) exp(result[1]);
        fitBleichrodtBeta = (double) result[2];

        if (SSR > 0)
        {
            rmseBleichrodt = sqrt(SSR/(double) N);
        }
    }
    else
    {
        SetStarts(mStarts);

        SetLowerUpperBounds("[Inf, Inf, 1.0]", "[-Inf, -Inf, 0.0]");

        if (fittingAlgorithm == FittingAlgorithm::Function)
        {
            lsfitcreatef(x,
                         y,
                         c,
                         diffstep,
                         state);

            lsfitsetbc(state,
                       bndl,
                       bndu);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             bleichrodt_discounting);

        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradient)
        {
            lsfitcreatefg(x,
                          y,
                          c,
                          true,
                          state);

            lsfitsetbc(state,
                       bndl,
                       bndu);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             bleichrodt_discounting,
                             bleichrodt_discounting_grad);
        }
        else if (fittingAlgorithm == FittingAlgorithm::FunctionGradientHessian)
        {
            lsfitcreatefgh(x,
                           y,
                           c,
                           state);

            lsfitsetbc(state,
                       bndl,
                       bndu);

            lsfitsetcond(state,
                         epsx,
                         maxits);

            alglib::lsfitfit(state,
                             bleichrodt_discounting,
                             bleichrodt_discounting_grad,
                             bleichrodt_discounting_hessian);
        }

        lsfitresults(state, info, c, rep);

        if ((int) GetInfo() == 2 || (int) GetInfo() == 5)
        {
            N = y.length();
            SSR = 0;

            for (int i = 0; i < N; i++)
            {
                holder = c[2] * exp(-exp(c[0])*pow(x[i][0], c[1]));

                ErrorResidual.append(((double) y[i] - holder));

                SSR += pow(ErrorResidual[i], 2);
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

/**
 * @brief ModelSelection::getED50BestModel
 * @param model
 * @return
 */
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

    case ModelType::GeneralizedHyperbolic:
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
        return QString("NA");

        break;

    }
}

/**
 * @brief ModelSelection::getED50ep
 * @return
 */
double ModelSelection::getED50ep () {
    double lowDelay = 0;
    double highDelay = x[x.rows()-1][0] * 100;

    int i = 0;

    while ((highDelay - lowDelay) > 0.001 && i < 100) {
      double lowEst = ChartWindow::ebert_prelec_prediction(fitEbertPrelecK, fitEbertPrelecS, lowDelay);
      double midEst = ChartWindow::ebert_prelec_prediction(fitEbertPrelecK, fitEbertPrelecS, (lowDelay+highDelay)/2);
      double highEst = ChartWindow::ebert_prelec_prediction(fitEbertPrelecK, fitEbertPrelecS, highDelay);

      if (lowEst > 0.5 && midEst > 0.5) {
        //Above 50% mark range
        lowDelay  = (lowDelay+highDelay)/2;
        highDelay = highDelay;

      } else if (highEst < 0.5 && midEst < 0.5) {
        //Below 50% mark range
        lowDelay  = lowDelay;
        highDelay = (lowDelay+highDelay)/2;

      }

      i++;
    }

    double returnValue = log((lowDelay+highDelay)/2);

    return returnValue;
}

/**
 * @brief ModelSelection::getED50crdi
 * @return
 */
double ModelSelection::getED50crdi () {
    double lowDelay = 0;
    double highDelay = x[x.rows()-1][0] * 100;

    int i = 0;

    while ((highDelay - lowDelay) > 0.001 && i < 100) {
      double lowEst = ChartWindow::bleichrodt_prediction(fitBleichrodtK, fitBleichrodtS, fitBleichrodtBeta, lowDelay);
      double midEst = ChartWindow::bleichrodt_prediction(fitBleichrodtK, fitBleichrodtS, fitBleichrodtBeta, (lowDelay+highDelay)/2);
      double highEst = ChartWindow::bleichrodt_prediction(fitBleichrodtK, fitBleichrodtS, fitBleichrodtBeta, highDelay);

      if (lowEst > 0.5 && midEst > 0.5) {
        //Above 50% mark range
        lowDelay  = (lowDelay+highDelay)/2;
        highDelay = highDelay;

      } else if (highEst < 0.5 && midEst < 0.5) {
        //Below 50% mark range
        lowDelay  = lowDelay;
        highDelay = (lowDelay+highDelay)/2;

      }

      i++;
    }

    double returnValue = log((lowDelay+highDelay)/2);

    return returnValue;
}

/**
 * @brief ModelSelection::getED50rodriguez
 * @return
 */
double ModelSelection::getED50rodriguez () {
    double lowDelay = 0;
    double highDelay = x[x.rows()-1][0] * 100;

    int i = 0;

    while ((highDelay - lowDelay) > 0.001 && i < 100) {
      double lowEst = ChartWindow::generalized_hyperbolic_prediction(fitGeneralizedHyperbolicK, fitGeneralizedHyperbolicBeta, lowDelay);
      double midEst = ChartWindow::generalized_hyperbolic_prediction(fitGeneralizedHyperbolicK, fitGeneralizedHyperbolicBeta, (lowDelay+highDelay)/2);
      double highEst = ChartWindow::generalized_hyperbolic_prediction(fitGeneralizedHyperbolicK, fitGeneralizedHyperbolicBeta, highDelay);

      if (lowEst > 0.5 && midEst > 0.5) {
        //Above 50% mark range
        lowDelay  = (lowDelay+highDelay)/2;
        highDelay = highDelay;

      } else if (highEst < 0.5 && midEst < 0.5) {
        //Below 50% mark range
        lowDelay  = lowDelay;
        highDelay = (lowDelay+highDelay)/2;

      }

      i++;
    }

    double returnValue = log((lowDelay+highDelay)/2);

    return returnValue;
}

/**
 * @brief ModelSelection::getAUCBestModel
 * @param model
 * @return
 */
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

    case ModelType::GeneralizedHyperbolic:
        mParams << fitGeneralizedHyperbolicK;
        mParams << fitGeneralizedHyperbolicBeta;

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

/**
 * @brief ModelSelection::getLogAUCBestModel
 * @param model
 * @return
 */
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

    case ModelType::GeneralizedHyperbolic:
        mParams << fitGeneralizedHyperbolicK;
        mParams << fitGeneralizedHyperbolicBeta;

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

/**
 * @brief ModelSelection::PrepareProbabilities
 */
void ModelSelection::PrepareProbabilities()
{
    bfNoise = ScaleFactor(NoiseBIC, NoiseBIC);
    bfExponential = NULL;
    bfParabolic = NULL;
    bfHyperbolic = NULL;
    bfQuasiHyperbolic = NULL;
    bfMyerson = NULL;
    bfRachlin = NULL;
    bfGeneralizedHyperbolic = NULL;
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
        else if (mBicList[i].first == ModelType::Parabolic)
        {
            bfParabolic = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfParabolic;
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
        else if (mBicList[i].first == ModelType::GeneralizedHyperbolic)
        {
            bfGeneralizedHyperbolic = ScaleFactor(mBicList[i].second, NoiseBIC);
            sumBayesFactors = sumBayesFactors + bfGeneralizedHyperbolic;
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
    probsGeneralizedHyperbolic = NULL;
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
        else if (mBicList[i].first == ModelType::Parabolic)
        {
            probsParabolic = bfParabolic/sumBayesFactors;
            mProbList.append(QPair<ModelType, double>(ModelType::Parabolic, probsParabolic));
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
        else if (mBicList[i].first == ModelType::GeneralizedHyperbolic)
        {
            probsGeneralizedHyperbolic = bfGeneralizedHyperbolic/sumBayesFactors;
            mProbList.append(QPair<ModelType, double>(ModelType::GeneralizedHyperbolic, probsGeneralizedHyperbolic));
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

/**
 * @brief ModelSelection::getErrorExponential
 * @param lnK
 * @return
 */
double ModelSelection::getErrorExponential(double lnK)
{
    leastSquaresError = 0;

    for (int i=0; i<y.length(); i++)
    {
        leastSquaresError = leastSquaresError + pow((y[i] - (exp(-exp(lnK)*x[i][0]))), 2);
    }

    return leastSquaresError;
}

/**
 * @brief ModelSelection::getErrorParabolic
 * @param lnK
 * @return
 */
double ModelSelection::getErrorParabolic(double lnK)
{
    leastSquaresError = 0;

    for (int i=0; i<y.length(); i++)
    {
        leastSquaresError = leastSquaresError + pow((y[i] - (1.0 - (exp(lnK) * pow(x[i][0], 2)))), 2);
    }

    return leastSquaresError;
}

/**
 * @brief ModelSelection::getErrorHyperbolic
 * @param lnK
 * @return
 */
double ModelSelection::getErrorHyperbolic(double lnK)
{
    leastSquaresError = 0;

    for (int i=0; i<y.length(); i++)
    {
        leastSquaresError = leastSquaresError + pow((y[i] - (pow((1+exp(lnK)*x[i][0]), -1))), 2);
    }

    return leastSquaresError;
}

/**
 * @brief ModelSelection::getErrorQuasiHyperbolic
 * @param beta
 * @param delta
 * @return
 */
double ModelSelection::getErrorQuasiHyperbolic(double beta, double delta)
{
    leastSquaresError = 0;

    for (int i=0; i<y.length(); i++)
    {
        leastSquaresError = leastSquaresError + pow((y[i] - (pow((beta*delta),x[i][0]))), 2);
    }

    return leastSquaresError;
}

/**
 * @brief ModelSelection::getErrorGreenMyerson
 * @param lnK
 * @param s
 * @return
 */
double ModelSelection::getErrorGreenMyerson(double lnK, double s)
{
    leastSquaresError = 0;

    s = exp(s);

    for (int i=0; i<y.length(); i++)
    {
        leastSquaresError = leastSquaresError + pow((y[i] - (pow((1+exp(lnK)*x[i][0]), (-s)))), 2);
    }

    return leastSquaresError;
}

/**
 * @brief ModelSelection::getErrorGreenRachlin
 * @param lnK
 * @param s
 * @return
 */
double ModelSelection::getErrorRachlin(double lnK, double s)
{
    leastSquaresError = 0;

    s = exp(s);

    for (int i=0; i<y.length(); i++)
    {
        leastSquaresError = leastSquaresError + pow((y[i] - pow((1+exp(lnK)*(pow(x[i][0],s))),(-1))), 2);
    }

    return leastSquaresError;
}

/**
 * @brief ModelSelection::getErrorGeneralizedHyperbolic
 * @param lnK
 * @param beta
 * @return
 */
double ModelSelection::getErrorGeneralizedHyperbolic(double lnK, double beta)
{
    leastSquaresError = 0;

    for (int i=0; i<y.length(); i++)
    {
        leastSquaresError = leastSquaresError + pow((y[i] - (pow((1 + x[i][0] * exp(lnK)),(-exp(beta) / exp(lnK))))), 2);
    }

    return leastSquaresError;
}

/**
 * @brief ModelSelection::getErrorEbertPrelec
 * @param lnK
 * @param s
 * @return
 */
double ModelSelection::getErrorEbertPrelec(double lnK, double s)
{
    leastSquaresError = 0;

    s = exp(s);

    for (int i=0; i<y.length(); i++)
    {
        leastSquaresError = leastSquaresError + pow((y[i] - exp(-pow((exp(lnK)*x[i][0]),s))), 2);
    }

    return leastSquaresError;
}

/**
 * @brief ModelSelection::getErrorBleichrodt
 * @param lnK
 * @param s
 * @param beta
 * @return
 */
double ModelSelection::getErrorBleichrodt(double lnK, double s, double beta)
{
    leastSquaresError = 0;

    s = exp(s);

    for (int i=0; i<y.length(); i++)
    {
        leastSquaresError = leastSquaresError + pow((y[i] - (beta*exp(-(exp(lnK)*pow(x[i][0],s))))), 2);
    }

    return leastSquaresError;
}

/**
 * @brief ModelSelection::SetFittingAlgorithm
 * @param value
 */
void ModelSelection::SetFittingAlgorithm(FittingAlgorithm value)
{
    fittingAlgorithm = value;
}

ModelSelection::ModelSelection()
{

}
