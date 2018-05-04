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

#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QWidget>
#include <QHBoxLayout>

#include <QMenu>
#include <QMenuBar>
#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QStackedWidget>

#include "Models/fitresults.h"
#include "Models/calculationsettings.h"

#include "Utilities/qcustomplot.h"

class ChartWindow : public QMainWindow
{
public:
    explicit ChartWindow(QList<FitResults> stringList, bool isLogNormal, ChartingOptions chartOption, QWidget *parent = 0);

    void buildProbabilityPlot();

    void buildErrorPlot();

    void buildED50Plot();
    void plotED50Series(int index);
    void plotED50Point(double i);

    void buildAUCPlot();
    void plotAUCSeries(int index);
    void plotAUCPoint(double i);

    void plotProbabilities(int index);
    void plotResiduals(int index);

    double exponential_plotting(double k, double x);
    double hyperbolic_plotting(double k, double x);
    double quasi_hyperbolic_plotting(double beta, double delta, double x);
    double myerson_plotting(double k, double s, double x);
    double rachlin_plotting(double k, double s, double x);
    double static generalized_hyperbolic_plotting(double k, double s, double x);
    double static ebert_prelec_plotting(double k, double s, double x);
    double static bleichrodt_plotting(double k, double s, double beta, double x);

    bool eventFilter(QObject *, QEvent *e);

private slots:
    void on_NextButton_clicked();
    void on_PreviousButton_clicked();
    void saveSVGasPNG();

private:

    /** Layouts
     *
     */
    QWidget *window;

    QVBoxLayout windowLayout;
    QStackedWidget stackedWidget;

    /** Display Widgets
     *
     */

    QCustomPlot *chart;
    QCustomPlot *chartArea;
    QCustomPlot *chartError;
    QCustomPlot *chartBar;

    QCPTextElement *titleMainChart;

    QVector<double> chartXTicks;
    QVector<QString> chartXLabels;

    double penWidth = 1.25;



    QStringList modelAxisCategories,
                delayPoints,
                valuePoints;

    int RawData = 0,
        ModelExponential = 1,
        ModelHyperbolic = 2,
        ModelQuasiHyperbolic = 3,
        ModelGreenMyerson = 4,
        ModelRachlin = 5,
        ModelGeneralizedHyperbolic = 6,
        ModelEbertPrelec = 7,
        ModelBeleichrodt = 8,
        ModelNoise = 9;

    /** Actions
     *
     */

    QAction *saveAction,
            *prevAction,
            *nextAction;

    /** Results
     *
     */

    QList<FitResults> mDisplayData;

    FitResults mList;

    /** Settings
     *
     */
    QString paramString1,
            paramString2;

    QString cleanTitle(QString title)
    {
        if (title.contains(" (f)"))
        {
            return title.replace(" (f)", "");
        }
        else if (title.contains(" (fg)"))
        {
            return title.replace(" (fg)", "");
        }
        else if (title.contains(" (fgh)"))
        {
            return title.replace(" (fgh)", "");
        }

        return title;
    }

    int GetAxisMaxLog10(QVector<double> curr)
    {
        int lower = 0;

        for (int i = 0; i < curr.length(); i++)
        {
            while (pow(10, lower) < curr[i])
            {
                lower++;
            }
        }

        return lower;
    }

    int GetAxisMaxLog10(double curr)
    {
        int lower = 0;

        while (pow(10, lower) < curr)
        {
            lower++;
        }

        return lower;
    }

    int currentIndexShown;

    bool isLogNormalParamerized,
         checkValue1,
         checkValue2,
         expCheck,
         hypCheck,
         quasiCheck,
         myerCheck,
         rachCheck,
         rodriguezCheck,
         ebertCheck,
         bleichrodtCheck;

    double expK,
           hypK,
           quasiB,
           quasiD,
           myerK,
           myerS,
           rachK,
           rachS,
           rodriguezK,
           rodriguezS,
           ebertK,
           ebertS,
           bleichrodtK,
           bleichrodtS,
           bleichrodtBeta,
           noise;

    double param1,
           param2,
           xParam,
           lastDelay,
           minList,
           maxList,
           chartIterator = 0.1,
           colWidth = 100;
};

#endif // CHARTWINDOW_H
