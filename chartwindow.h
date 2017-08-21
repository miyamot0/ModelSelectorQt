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

    void buildED50Plot();
    void plotED50Series(int index);

    void buildAUCPlot();
    void plotAUCSeries(int index);

    double exponential_plotting(double k, double x);
    double hyperbolic_plotting(double k, double x);
    double quasi_hyperbolic_plotting(double beta, double delta, double x);
    double myerson_plotting(double k, double s, double x);
    double rachlin_plotting(double k, double s, double x);
    double static rodriguez_logue_plotting(double k, double s, double x);
    double static ebert_prelec_plotting(double k, double s, double x);
    double static bleichrodt_plotting(double k, double s, double beta, double x);

    bool eventFilter(QObject *object, QEvent *e);

private slots:
    void on_NextButton_clicked();
    void on_PreviousButton_clicked();
    void saveSVGasPNG();

private:
    QVBoxLayout *windowLayout;

    QStackedWidget *stackedWidget;

    QWidget *window;

    QFont mTitle;
    QFont mLegendFont;

    QChart *chart;
    QChartView *chartView;
    QValueAxis *axisX;
    QValueAxis *axisY;

    QChart *barChart;
    QChartView *barChartView;

    QBarSet *expProbSet;
    QBarSet *hypProbSet;
    QBarSet *quasiProbSet;
    QBarSet *myerProbSet;
    QBarSet *rachProbSet;
    QBarSet *rodriguezProbSet;
    QBarSet *ebertProbSet;
    QBarSet *bleichrodtProbSet;
    QBarSet *noiseProbSet;

    QStackedBarSeries *barSeries;

    QStringList modelAxisCategories;

    QBarCategoryAxis *barAxisX;
    QValueAxis *barAxisY;

    QList<FitResults> mDisplayData;

    int currentIndexShown;

    QAction *saveAction;
    QAction *prevAction;
    QAction *nextAction;

    FitResults mList;

    double param1;
    double param2;

    QString paramString1;
    QString paramString2;

    bool checkValue1;
    bool checkValue2;

    QLineSeries *expSeries;
    QLineSeries *hypSeries;
    QLineSeries *quasiSeries;
    QLineSeries *myerSeries;
    QLineSeries *rachSeries;
    QLineSeries *rodriguezSeries;
    QLineSeries *ebertSeries;
    QLineSeries *bleichrodtSeries;
    QLineSeries *noiseSeries;

    QScatterSeries *dataPoints;
    QLineSeries *empiricalSeries;

    bool isLogNormalParamerized;

    bool expCheck;
    double expK;

    bool hypCheck;
    double hypK;

    bool quasiCheck;
    double quasiB;
    double quasiD;

    bool myerCheck;
    double myerK;
    double myerS;

    bool rachCheck;
    double rachK;
    double rachS;

    bool rodriguezCheck;
    double rodriguezK;
    double rodriguezS;

    bool ebertCheck;
    double ebertK;
    double ebertS;

    bool bleichrodtCheck;
    double bleichrodtK;
    double bleichrodtS;
    double bleichrodtBeta;

    double noise;

    double xParam;

    QStringList delayPoints;
    QStringList valuePoints;

    double lastDelay;

    bool isED50Figure;
    bool isAUCFigure;
    bool isAUCLogFigure;
};

#endif // CHARTWINDOW_H
