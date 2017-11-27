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
#include <QtCharts>

#include "fitresults.h"
#include "calculationsettings.h"

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

    QToolBar tb;

    /** Display Widgets
     *
     */
    QChart chart,
           chartArea,
           chartError,
           barChart;

    QChartView *chartView,
               *chartViewArea,
               *barChartView,
               *chartViewError;

    QValueAxis axisX,
               axisY,
               axisXarea,
               axisYarea,
               axisXerror,
               axisYerror,
               barAxisX,
               barAxisY;

    QStringList modelAxisCategories,
                delayPoints,
                valuePoints;

    QLineSeries expSeries,
                hypSeries,
                quasiSeries,
                myerSeries,
                rachSeries,
                rodriguezSeries,
                ebertSeries,
                bleichrodtSeries,
                noiseSeries,

                errSeries,

                expProbSet,
                hypProbSet,
                quasiProbSet,
                myerProbSet,
                rachProbSet,
                rodriguezProbSet,
                ebertProbSet,
                bleichrodtProbSet,
                noiseProbSet,

                expSeriesArea,
                hypSeriesArea,
                quasiSeriesArea,
                myerSeriesArea,
                rachSeriesArea,
                rodriguezSeriesArea,
                ebertSeriesArea,
                bleichrodtSeriesArea,
                noiseSeriesArea,
                empiricalSeries;

    QScatterSeries dataPoints,
                   dataPointsArea,
                   errDataPoints;

    QFont mTitle = QFont("Serif", 14, -1, false),
          mLegendFont = QFont("Serif", 10, -1, false);

    QPen expPen,
         hypPen,
         quasiPen,
         myerPen,
         rachPen,
         rodriguezPen,
         ebertPen,
         bleichrodtPen,
         noisePen;

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
           chartIterator = 0.25,
           colWidth = 100;
};

#endif // CHARTWINDOW_H
