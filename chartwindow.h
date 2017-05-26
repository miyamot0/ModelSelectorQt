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

class ChartWindow : public QMainWindow
{
public:
    explicit ChartWindow(QList<QStringList> stringList, bool isAUC, QWidget *parent = 0);
    //void drawFigure(int index);

    void buildED50Plot();
    void plotED50Series(int index);

    void buildAUCPlot();
    void plotAUCSeries(int index);

    double exponential_plotting(double k, double x);
    double hyperbolic_plotting(double k, double x);
    double quasi_hyperbolic_plotting(double beta, double delta, double x);
    double myerson_plotting(double k, double s, double x);
    double rachlin_plotting(double k, double s, double x);

    bool eventFilter(QObject *object, QEvent *e);

private slots:
    void on_NextButton_clicked();
    void on_PreviousButton_clicked();
    void saveSVGasPNG();

private:
    QChart *chart;
    QChartView *chartView;
    QLogValueAxis *axisX;
    QValueAxis *axisY;

    QList<QStringList> mDisplayData;

    int currentIndexShown;

    QAction *saveAction;
    QAction *prevAction;
    QAction *nextAction;

    QStringList mList;

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
    QLineSeries *noiseSeries;

    QScatterSeries *dataPoints;
    QLineSeries *empiricalSeries;

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

    double noise;

    double xParam;

    QStringList delayPoints;
    QStringList valuePoints;

    double lastDelay;

    bool isAUCFigure;
};

#endif // CHARTWINDOW_H