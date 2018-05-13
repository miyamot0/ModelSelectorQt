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

#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

#include "chartwindow.h"

ChartWindow::ChartWindow(QList<FitResults> stringList, bool isLogNormal, ChartingOptions chartOption, QWidget *parent)
{
    if (chartOption == ChartingOptions::None)
    {
        close();
    }

    mDisplayData = stringList;

    isLogNormalParamerized = isLogNormal;

    QVBoxLayout *windowLayout = new QVBoxLayout;

    currentIndexShown = 0;

    installEventFilter(this);

    chart = new QCustomPlot();
    chartArea = new QCustomPlot();
    chartError = new QCustomPlot();
    chartBar = new QCustomPlot();

    buildED50Plot();
    stackedWidget.addWidget(chart);

    buildAUCPlot();
    stackedWidget.addWidget(chartArea);

    buildErrorPlot();
    stackedWidget.addWidget(chartError);

    buildProbabilityPlot();
    stackedWidget.addWidget(chartBar);

    windowLayout->addWidget(&stackedWidget);

    // Set layout in QWidget
    QWidget *window = new QWidget(parent);
    window->setLayout(windowLayout);

    lastDelay = 0;

    for (int i=0; i<mDisplayData.length(); i++)
    {
        paramString1 = mDisplayData[i].ParticipantDelays;
        QStringList delayPoints = paramString1.split(",");

        double testMax = delayPoints[delayPoints.length() - 1].toDouble() * 2;

        if (i == 0)
        {
            lastDelay = testMax;
        }
        else if (i > 0 && testMax > lastDelay)
        {
            lastDelay = testMax;
        }
    }

    saveAction = new QAction(tr("Save"), this);
    connect(saveAction, &QAction::triggered, this, &ChartWindow::saveImage);

    prevAction = new QAction(tr("Previous"), this);
    connect(prevAction, &QAction::triggered, this, &ChartWindow::on_PreviousButton_clicked);

    nextAction = new QAction(tr("Next"), this);
    connect(nextAction, &QAction::triggered, this, &ChartWindow::on_NextButton_clicked);

    QToolBar *tb = new QToolBar();

    tb->addAction(saveAction);
    tb->addAction(prevAction);
    tb->addAction(nextAction);

    setWindowTitle(tr("Discounting Model Selection Plots (Use Arrows to Change Plots)"));
    windowLayout->setMenuBar(tb);
    setCentralWidget(window);
    resize(800, 600);

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    plotED50Series(0);
    plotAUCSeries(0);
    plotProbabilities(0);
    plotResiduals(0);
}

// Series

/**
 * @brief ChartWindow::buildED50Plot
 */
void ChartWindow::buildED50Plot()
{
    chart->legend->setVisible(true);
    chart->yAxis->setLabel("Value");
    chart->yAxis->setScaleType(QCPAxis::stLinear);
    chart->yAxis->setBasePen(QPen(Qt::black));
    chart->yAxis->grid()->setPen(Qt::NoPen);

    chart->xAxis->setScaleType(QCPAxis::stLogarithmic);
    chart->xAxis->setLabel("Delay");
    chart->xAxis->grid()->setPen(Qt::NoPen);

    chart->yAxis->setRangeLower(0);

    titleMainChart = new QCPTextElement(chart, "test", QFont("sans", 12, QFont::Bold));

    chart->plotLayout()->insertRow(0);
    chart->plotLayout()->addElement(0, 0, titleMainChart);

    // Add Points
    chart->addGraph();
    chart->graph(RawData)->setLineStyle(QCPGraph::lsNone);
    chart->graph(RawData)->setName("Raw Data");
    chart->graph(RawData)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                     Qt::black,
                                                     Qt::black,
                                                     8));

    chart->addGraph();
    chart->graph(ModelExponential)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelExponential)->setName("Exponential");
    chart->graph(ModelExponential)->setPen(QPen(Qt::red, penWidth));

    chart->addGraph();
    chart->graph(ModelHyperbolic)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelHyperbolic)->setName("Hyperbolic");
    chart->graph(ModelHyperbolic)->setPen(QPen(Qt::green, penWidth));

    chart->addGraph();
    chart->graph(ModelQuasiHyperbolic)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelQuasiHyperbolic)->setName("Quasi Hyperbolic");
    chart->graph(ModelQuasiHyperbolic)->setPen(QPen(Qt::blue, penWidth));

    chart->addGraph();
    chart->graph(ModelGreenMyerson)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelGreenMyerson)->setName("Green-Myerson");
    chart->graph(ModelGreenMyerson)->setPen(QPen(Qt::lightGray, penWidth));

    chart->addGraph();
    chart->graph(ModelRachlin)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelRachlin)->setName("Rachlin");
    chart->graph(ModelRachlin)->setPen(QPen(Qt::gray, penWidth));

    chart->addGraph();
    chart->graph(ModelGeneralizedHyperbolic)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelGeneralizedHyperbolic)->setName("Generalized-Hyperbolic");
    chart->graph(ModelGeneralizedHyperbolic)->setPen(QPen(Qt::darkBlue, penWidth));

    chart->addGraph();
    chart->graph(ModelEbertPrelec)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelEbertPrelec)->setName("Ebert-Prelec");
    chart->graph(ModelEbertPrelec)->setPen(QPen(Qt::darkGreen, penWidth));

    chart->addGraph();
    chart->graph(ModelBeleichrodt)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelBeleichrodt)->setName("Bleichrodt");
    chart->graph(ModelBeleichrodt)->setPen(QPen(Qt::darkMagenta, penWidth));

    chart->addGraph();
    chart->graph(ModelNoise)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelNoise)->setName("Noise");
    chart->graph(ModelNoise)->setPen(QPen(Qt::darkCyan, penWidth));

    chart->addGraph();
    chart->graph(ModelParabolic)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelParabolic)->setName("Parabolic");
    chart->graph(ModelParabolic)->setPen(QPen(Qt::darkGreen, penWidth));

    chart->addGraph();
    chart->graph(ModelPower)->setLineStyle(QCPGraph::lsLine);
    chart->graph(ModelPower)->setName("Power");
    chart->graph(ModelPower)->setPen(QPen(Qt::blue, penWidth));
}

/**
 * @brief ChartWindow::plotED50Point
 * @param i
 */
void ChartWindow::plotED50Point(double i)
{
    xParam = pow(10, i);

    chart->graph(ModelNoise)->addData(xParam, noise);

    if (expCheck)
    {
        chart->graph(ModelExponential)->addData(xParam, exponential_plotting(expK, xParam));
    }

    if (paraCheck)
    {
        chart->graph(ModelParabolic)->addData(xParam, parabolic_plotting(paraK, xParam));
    }

    if (hypCheck)
    {
        chart->graph(ModelHyperbolic)->addData(xParam, hyperbolic_plotting(hypK, xParam));
    }

    if (quasiCheck)
    {
        chart->graph(ModelQuasiHyperbolic)->addData(xParam, quasi_hyperbolic_plotting(quasiB, quasiD, xParam));
    }

    if (powerCheck)
    {
        chart->graph(ModelPower)->addData(xParam, power_plotting(powerK, powerS, xParam));
    }

    if (myerCheck)
    {
        chart->graph(ModelGreenMyerson)->addData(xParam, myerson_plotting(myerK, myerS, xParam));
    }

    if (rachCheck)
    {
        chart->graph(ModelRachlin)->addData(xParam, rachlin_plotting(rachK, rachS, xParam));
    }

    if (rodriguezCheck)
    {
        chart->graph(ModelGeneralizedHyperbolic)->addData(xParam, generalized_hyperbolic_plotting(rodriguezK, rodriguezS, xParam));
    }

    if (ebertCheck)
    {
        chart->graph(ModelEbertPrelec)->addData(xParam, ebert_prelec_plotting(ebertK, ebertS, xParam));
    }

    if (bleichrodtCheck)
    {
        chart->graph(ModelBeleichrodt)->addData(xParam, bleichrodt_plotting(bleichrodtK, bleichrodtS, bleichrodtBeta, xParam));
    }
}

/**
 * @brief ChartWindow::plotED50Series
 * @param index
 */
void ChartWindow::plotED50Series(int index)
{
    mList = mDisplayData.at(index);

    for (int i = 0; i < ModelEmpirical; i++)
    {
        chart->graph(i)->setVisible(false);
        chart->graph(i)->removeFromLegend();
        chart->graph(i)->data().data()->clear();
    }

    chart->graph(RawData)->setVisible(true);

    chart->graph(ModelNoise)->setVisible(true);
    chart->graph(ModelNoise)->addToLegend();

    if (mList.Header.contains("dropped", Qt::CaseInsensitive))
    {
        titleMainChart->setText(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    titleMainChart->setText(QString("Participant #%1: %2 ln(ED50) = %3").arg(QString::number(currentIndexShown + 1)).arg(cleanTitle(mList.TopModel)).arg(mList.TopED50));

    expCheck = hypCheck = quasiCheck = myerCheck = rachCheck = rodriguezCheck = ebertCheck = bleichrodtCheck = paraCheck = false;

    for (int i=0; i<mList.FittingResults.length(); i++)
    {
        if (mList.FittingResults[i]->Model == ModelType::Noise)
        {
            noise = mList.FittingResults[i]->Params.first().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::Parabolic)
        {
            paraK = mList.FittingResults[i]->Params.first().second;

            if (paraK != NULL)
            {
                paraCheck = true;
                chart->graph(ModelParabolic)->setVisible(true);
                chart->graph(ModelParabolic)->addToLegend();
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Exponential)
        {
            expK = mList.FittingResults[i]->Params.first().second;

            if (expK != NULL)
            {
                expCheck = true;
                chart->graph(ModelExponential)->setVisible(true);
                chart->graph(ModelExponential)->addToLegend();
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Hyperbolic)
        {
            hypK = mList.FittingResults[i]->Params.first().second;

            if (hypK != NULL)
            {
                hypCheck = true;
                chart->graph(ModelHyperbolic)->setVisible(true);
                chart->graph(ModelHyperbolic)->addToLegend();
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::BetaDelta)
        {
            quasiB = mList.FittingResults[i]->Params.first().second;
            quasiD = mList.FittingResults[i]->Params.last().second;

            if (quasiB != NULL && quasiD != NULL)
            {
                quasiCheck = true;
                chart->graph(ModelQuasiHyperbolic)->setVisible(true);
                chart->graph(ModelQuasiHyperbolic)->addToLegend();
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Power)
        {
            powerK = mList.FittingResults[i]->Params.first().second;
            powerS = mList.FittingResults[i]->Params.last().second;

            if (powerK != NULL && powerS != NULL)
            {
                powerCheck = true;
                chart->graph(ModelPower)->setVisible(true);
                chart->graph(ModelPower)->addToLegend();
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Myerson)
        {
            myerK = mList.FittingResults[i]->Params.first().second;
            myerS = mList.FittingResults[i]->Params.last().second;

            if (myerK != NULL && myerS != NULL)
            {
                myerCheck = true;
                chart->graph(ModelGreenMyerson)->setVisible(true);
                chart->graph(ModelGreenMyerson)->addToLegend();
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Rachlin)
        {
            rachK = mList.FittingResults[i]->Params.first().second;
            rachS = mList.FittingResults[i]->Params.last().second;

            if (rachK != NULL && rachS != NULL)
            {
                rachCheck = true;
                chart->graph(ModelRachlin)->setVisible(true);
                chart->graph(ModelRachlin)->addToLegend();
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::GeneralizedHyperbolic)
        {
            rodriguezK = mList.FittingResults[i]->Params.first().second;
            rodriguezS = mList.FittingResults[i]->Params.last().second;

            if (rodriguezK != NULL && rodriguezS != NULL)
            {
                rodriguezCheck = true;
                chart->graph(ModelGeneralizedHyperbolic)->setVisible(true);
                chart->graph(ModelGeneralizedHyperbolic)->addToLegend();
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::EbertPrelec)
        {
            ebertK = mList.FittingResults[i]->Params.first().second;
            ebertS = mList.FittingResults[i]->Params.last().second;

            if (ebertK != NULL && ebertS != NULL)
            {
                ebertCheck = true;
                chart->graph(ModelEbertPrelec)->setVisible(true);
                chart->graph(ModelEbertPrelec)->addToLegend();
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Beleichrodt)
        {
            bleichrodtCheck = true;
            bleichrodtK = mList.FittingResults[i]->Params.first().second;
            bleichrodtS = mList.FittingResults[i]->Params[1].second;
            bleichrodtBeta = mList.FittingResults[i]->Params.last().second;

            if (bleichrodtK != NULL && bleichrodtS != NULL && bleichrodtBeta != NULL)
            {
                bleichrodtCheck = true;
                chart->graph(ModelBeleichrodt)->setVisible(true);
                chart->graph(ModelBeleichrodt)->addToLegend();
            }
        }
    }

    int finalLog = GetAxisMaxLog10(lastDelay);

    for (double i = -1; i < (finalLog+1); i = i + chartIterator)
    {
        plotED50Point(i);
    }

    paramString1 = mList.ParticipantDelays;
    paramString2 = mList.ParticipantValues;

    delayPoints = paramString1.split(",");
    valuePoints = paramString2.split(",");

    double maxValue = -1;

    for (int i = 0; i < delayPoints.length(); i++)
    {
        param1 = delayPoints[i].toDouble(&checkValue1);
        param2 = valuePoints[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        if (param1 > maxValue)
        {
            maxValue = param1;
        }

        chart->graph(RawData)->addData(param1, param2);
    }

    chart->yAxis->setRangeLower(0);
    chart->yAxis->setRangeUpper(1);

    int getXLabel = GetAxisMaxLog10(maxValue);

    chartXTicks.clear();
    chartXLabels.clear();

    for (int i = -1; i <= getXLabel; i++)
    {
        chartXTicks.append(pow(10, i));
        chartXLabels.append(QString("%1").arg(pow(10, i)));
    }

    QSharedPointer<QCPAxisTickerText> chartTicker(new QCPAxisTickerText);
    chartTicker->addTicks(chartXTicks, chartXLabels);
    chart->xAxis->setTicker(chartTicker);

    QSharedPointer<QCPAxisTickerText> chartTicker2(new QCPAxisTickerText);
    chartTicker2->addTicks(QVector<double>({0, 0.2, 0.4, 0.6, 0.8, 1.0}),
                           QVector<QString>({"0", "0.2", "0.4", "0.6", "0.8", "1.0"}));
    chart->yAxis->setTicker(chartTicker2);

    chart->xAxis->setRangeLower(0.1);
    chart->xAxis->setRangeUpper(pow(10, getXLabel));

    chart->replot();
}

// Area

/**
 * @brief ChartWindow::buildAUCPlot
 */
void ChartWindow::buildAUCPlot()
{
    chartArea->legend->setVisible(true);
    chartArea->yAxis->setLabel("Value");
    chartArea->yAxis->setScaleType(QCPAxis::stLinear);
    chartArea->yAxis->setBasePen(QPen(Qt::black));
    chartArea->yAxis->grid()->setPen(Qt::NoPen);

    chartArea->xAxis->setScaleType(QCPAxis::stLogarithmic);
    chartArea->xAxis->setLabel("Delay");
    chartArea->xAxis->grid()->setPen(Qt::NoPen);

    chartArea->yAxis->setRangeLower(0);

    titleAreaChart = new QCPTextElement(chartArea, "test", QFont("sans", 12, QFont::Bold));

    chartArea->plotLayout()->insertRow(0);
    chartArea->plotLayout()->addElement(0, 0, titleAreaChart);

    // Add Points
    chartArea->addGraph();
    chartArea->graph(RawData)->setLineStyle(QCPGraph::lsNone);
    chartArea->graph(RawData)->setName("Raw Data");
    chartArea->graph(RawData)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                     Qt::black,
                                                     Qt::black,
                                                     8));

    chartArea->addGraph();
    chartArea->graph(ModelExponential)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelExponential)->setName("Exponential");
    chartArea->graph(ModelExponential)->setPen(QPen(Qt::red, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelHyperbolic)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelHyperbolic)->setName("Hyperbolic");
    chartArea->graph(ModelHyperbolic)->setPen(QPen(Qt::green, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelQuasiHyperbolic)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelQuasiHyperbolic)->setName("Quasi Hyperbolic");
    chartArea->graph(ModelQuasiHyperbolic)->setPen(QPen(Qt::blue, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelGreenMyerson)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelGreenMyerson)->setName("Green-Myerson");
    chartArea->graph(ModelGreenMyerson)->setPen(QPen(Qt::lightGray, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelRachlin)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelRachlin)->setName("Rachlin");
    chartArea->graph(ModelRachlin)->setPen(QPen(Qt::gray, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelGeneralizedHyperbolic)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelGeneralizedHyperbolic)->setName("Generalized-Hyperbolic");
    chartArea->graph(ModelGeneralizedHyperbolic)->setPen(QPen(Qt::darkBlue, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelEbertPrelec)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelEbertPrelec)->setName("Ebert-Prelec");
    chartArea->graph(ModelEbertPrelec)->setPen(QPen(Qt::darkGreen, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelBeleichrodt)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelBeleichrodt)->setName("Bleichrodt");
    chartArea->graph(ModelBeleichrodt)->setPen(QPen(Qt::darkMagenta, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelNoise)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelNoise)->setName("Noise");
    chartArea->graph(ModelNoise)->setPen(QPen(Qt::darkCyan, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelParabolic)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelParabolic)->setName("Parabolic");
    chartArea->graph(ModelParabolic)->setPen(QPen(Qt::darkGreen, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelPower)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelPower)->setName("Power");
    chartArea->graph(ModelPower)->setPen(QPen(Qt::blue, penWidth));

    chartArea->addGraph();
    chartArea->graph(ModelEmpirical)->setLineStyle(QCPGraph::lsLine);
    chartArea->graph(ModelEmpirical)->setName("Empirical");
    chartArea->graph(ModelEmpirical)->setPen(QPen(Qt::black, penWidth));
}

/**
 * @brief ChartWindow::plotAUCSeries
 * @param index
 */
void ChartWindow::plotAUCSeries(int index)
{
    mList = mDisplayData.at(index);

    for (int i = 0; i < ModelEmpirical; i++)
    {
        chartArea->graph(i)->setVisible(false);
        chartArea->graph(i)->removeFromLegend();
        chartArea->graph(i)->data().data()->clear();
    }

    chartArea->graph(RawData)->setVisible(true);

    chartArea->graph(ModelEmpirical)->data().data()->clear();
    chartArea->graph(ModelEmpirical)->setVisible(true);
    chartArea->graph(ModelEmpirical)->removeFromLegend();

    if (mList.Header.contains("dropped", Qt::CaseInsensitive))
    {
        titleAreaChart->setText(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    titleAreaChart->setText(QString("Participant #%1: %2 Log Scaled MB-AUC = %3").arg(QString::number(currentIndexShown + 1)).arg(cleanTitle(mList.TopModel)).arg(mList.TopAUCLog));

    expCheck = hypCheck = quasiCheck = myerCheck = rachCheck = rodriguezCheck = ebertCheck = bleichrodtCheck = paraCheck = false;

    switch (mList.TopModelType)
    {
        case ModelType::Noise:
            chartArea->graph(ModelNoise)->setVisible(true);
            chartArea->graph(ModelNoise)->addToLegend();
            break;

        case ModelType::Exponential:
            chartArea->graph(ModelExponential)->setVisible(true);
            chartArea->graph(ModelExponential)->addToLegend();
            break;

        case ModelType::Parabolic:
            chartArea->graph(ModelParabolic)->setVisible(true);
            chartArea->graph(ModelParabolic)->addToLegend();
            break;

        case ModelType::Hyperbolic:
            chartArea->graph(ModelHyperbolic)->setVisible(true);
            chartArea->graph(ModelHyperbolic)->addToLegend();
            break;

        case ModelType::BetaDelta:
            chartArea->graph(ModelQuasiHyperbolic)->setVisible(true);
            chartArea->graph(ModelQuasiHyperbolic)->addToLegend();
            break;

        case ModelType::Power:
            chartArea->graph(ModelPower)->setVisible(true);
            chartArea->graph(ModelPower)->addToLegend();
            break;

        case ModelType::Myerson:
            chartArea->graph(ModelGreenMyerson)->setVisible(true);
            chartArea->graph(ModelGreenMyerson)->addToLegend();
            break;

        case ModelType::Rachlin:
            chartArea->graph(ModelRachlin)->setVisible(true);
            chartArea->graph(ModelRachlin)->addToLegend();
            break;

        case ModelType::GeneralizedHyperbolic:
            chartArea->graph(ModelGeneralizedHyperbolic)->setVisible(true);
            chartArea->graph(ModelGeneralizedHyperbolic)->addToLegend();
            break;

        case ModelType::EbertPrelec:
            chartArea->graph(ModelEbertPrelec)->setVisible(true);
            chartArea->graph(ModelEbertPrelec)->addToLegend();
            break;

        case ModelType::Beleichrodt:
            chartArea->graph(ModelBeleichrodt)->setVisible(true);
            chartArea->graph(ModelBeleichrodt)->addToLegend();
            break;
    }

    for (int i=0; i<mList.FittingResults.length(); i++)
    {
        if (mList.FittingResults[i]->Model == ModelType::Noise)
        {
            noise = mList.FittingResults[i]->Params.first().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::Exponential)
        {
            expCheck = true;
            expK = mList.FittingResults[i]->Params.first().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::Parabolic)
        {
            paraCheck = true;
            paraK = mList.FittingResults[i]->Params.first().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::Hyperbolic)
        {
            hypCheck = true;
            hypK = mList.FittingResults[i]->Params.first().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::BetaDelta)
        {
            quasiCheck = true;
            quasiB = mList.FittingResults[i]->Params.first().second;
            quasiD = mList.FittingResults[i]->Params.last().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::Power)
        {
            powerCheck = true;
            powerK = mList.FittingResults[i]->Params.first().second;
            powerS = mList.FittingResults[i]->Params.last().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::Myerson)
        {
            myerCheck = true;
            myerK = mList.FittingResults[i]->Params.first().second;
            myerS = mList.FittingResults[i]->Params.last().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::Rachlin)
        {
            rachCheck = true;
            rachK = mList.FittingResults[i]->Params.first().second;
            rachS = mList.FittingResults[i]->Params.last().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::GeneralizedHyperbolic)
        {
            rodriguezCheck = true;
            rodriguezK = mList.FittingResults[i]->Params.first().second;
            rodriguezS = mList.FittingResults[i]->Params.last().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::EbertPrelec)
        {
            ebertCheck = true;
            ebertK = mList.FittingResults[i]->Params.first().second;
            ebertS = mList.FittingResults[i]->Params.last().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::Beleichrodt)
        {
            bleichrodtCheck = true;
            bleichrodtK = mList.FittingResults[i]->Params.first().second;
            bleichrodtS = mList.FittingResults[i]->Params[1].second;
            bleichrodtBeta = mList.FittingResults[i]->Params.last().second;
        }
    }

    int finalLog = GetAxisMaxLog10(lastDelay);

    for (double i = -1; i < (finalLog+1); i = i + chartIterator)
    {
        plotAUCPoint(i);
    }

    paramString1 = mList.ParticipantDelays;
    paramString2 = mList.ParticipantValues;

    delayPoints = paramString1.split(",");
    valuePoints = paramString2.split(",");

    double maxValue = -1;

    for (int i = 0; i < delayPoints.length(); i++)
    {
        param1 = delayPoints[i].toDouble(&checkValue1);
        param2 = valuePoints[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        //Hack
        if (param1 == 0)
        {
            param1 = 0.1;
        }

        if (param1 > maxValue)
        {
            maxValue = param1;
        }

        chartArea->graph(RawData)->addData(param1, param2);
        chartArea->graph(ModelEmpirical)->addData(param1, param2);
    }

    chartArea->yAxis->setRangeLower(0);
    chartArea->yAxis->setRangeUpper(1);

    int getXLabel = GetAxisMaxLog10(maxValue);

    chartXTicks.clear();
    chartXLabels.clear();

    for (int i = -1; i <= getXLabel; i++)
    {
        chartXTicks.append(pow(10, i));
        chartXLabels.append(QString("%1").arg(pow(10, i)));
    }

    QSharedPointer<QCPAxisTickerText> chartTicker(new QCPAxisTickerText);
    chartTicker->addTicks(chartXTicks, chartXLabels);
    chartArea->xAxis->setTicker(chartTicker);

    QSharedPointer<QCPAxisTickerText> chartTicker2(new QCPAxisTickerText);
    chartTicker2->addTicks(QVector<double>({0, 0.2, 0.4, 0.6, 0.8, 1.0}),
                           QVector<QString>({"0", "0.2", "0.4", "0.6", "0.8", "1.0"}));
    chartArea->yAxis->setTicker(chartTicker2);

    chartArea->xAxis->setRangeLower(0.1);
    chartArea->xAxis->setRangeUpper(pow(10, getXLabel));

    chartArea->replot();
}

/**
 * @brief ChartWindow::plotAUCPoint
 * @param i
 */
void ChartWindow::plotAUCPoint(double i)
{
    xParam = pow(10, i);

    chartArea->graph(ModelNoise)->addData(xParam, noise);

    if (expCheck)
    {
        chartArea->graph(ModelExponential)->addData(xParam, exponential_plotting(expK, xParam));
    }

    if (paraCheck)
    {
        chartArea->graph(ModelParabolic)->addData(xParam, parabolic_plotting(paraK, xParam));
    }

    if (hypCheck)
    {
        chartArea->graph(ModelHyperbolic)->addData(xParam, hyperbolic_plotting(hypK, xParam));
    }

    if (quasiCheck)
    {
        chartArea->graph(ModelQuasiHyperbolic)->addData(xParam, quasi_hyperbolic_plotting(quasiB, quasiD, xParam));
    }

    if (powerCheck)
    {
        chartArea->graph(ModelPower)->addData(xParam, power_plotting(powerK, powerS, xParam));
    }

    if (myerCheck)
    {
        chartArea->graph(ModelGreenMyerson)->addData(xParam, myerson_plotting(myerK, myerS, xParam));
    }

    if (rachCheck)
    {
        chartArea->graph(ModelRachlin)->addData(xParam, rachlin_plotting(rachK, rachS, xParam));
    }

    if (rodriguezCheck)
    {
        chartArea->graph(ModelGeneralizedHyperbolic)->addData(xParam, generalized_hyperbolic_plotting(rodriguezK, rodriguezS, xParam));
    }

    if (ebertCheck)
    {
        chartArea->graph(ModelEbertPrelec)->addData(xParam, ebert_prelec_plotting(ebertK, ebertS, xParam));
    }

    if (bleichrodtCheck)
    {
        chartArea->graph(ModelBeleichrodt)->addData(xParam, bleichrodt_plotting(bleichrodtK, bleichrodtS, bleichrodtBeta, xParam));
    }
}

// Residuals

/**
 * @brief ChartWindow::buildErrorPlot
 */
void ChartWindow::buildErrorPlot()
{
    chartError->legend->setVisible(false);

    chartError->yAxis->setLabel(tr("Error Value"));
    chartError->yAxis->setScaleType(QCPAxis::stLinear);
    chartError->yAxis->setBasePen(QPen(Qt::black));
    chartError->yAxis->grid()->setPen(Qt::NoPen);

    chartError->xAxis->setLabel(tr("Residual"));
    chartError->xAxis->setScaleType(QCPAxis::stLinear);
    chartError->xAxis->setBasePen(QPen(Qt::black));
    chartError->xAxis->grid()->setPen(Qt::NoPen);

    titleErrorChart = new QCPTextElement(chartError, "test", QFont("sans", 12, QFont::Bold));

    chartError->plotLayout()->insertRow(0);
    chartError->plotLayout()->addElement(0, 0, titleErrorChart);

    // Add Points
    chartError->addGraph();
    chartError->graph(0)->setLineStyle(QCPGraph::lsNone);
    chartError->graph(0)->setName("Error");
    chartError->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare,
                                                     Qt::black,
                                                     Qt::black,
                                                     8));
}

/**
 * @brief ChartWindow::plotResiduals
 * @param index
 */
void ChartWindow::plotResiduals(int index)
{
    mList = mDisplayData.at(index);

    if (mList.Header.contains("dropped", Qt::CaseInsensitive))
    {
        titleErrorChart->setText(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    titleErrorChart->setText(QString("Residual Error for Participant #%1").arg(QString::number(currentIndexShown + 1)));

    chartError->graph(0)->data().data()->clear();
    chartError->graph(0)->removeFromLegend();

    for (int j=0; j<mList.TopErrPar.length(); j++)
    {
        chartError->graph(0)->addData(j+1, mList.TopErrPar[j]);
    }

    if (mList.TopErrPar.length() > 0)
    {
        minList = std::abs(*std::min_element(mList.TopErrPar.begin(), mList.TopErrPar.end())) * 1.5;
        maxList = std::abs(*std::max_element(mList.TopErrPar.begin(), mList.TopErrPar.end())) * 1.5;

        chartError->yAxis->setRangeLower((maxList >= minList) ? -maxList : -minList);
        chartError->yAxis->setRangeUpper((maxList >= minList) ? maxList : minList);
    }

    chartError->xAxis->setRangeLower(0);
    chartError->xAxis->setRangeUpper(mList.TopErrPar.length() + 1);

    chartError->replot();
}

// Probabilities

/**
 * @brief ChartWindow::buildProbabilityPlot
 */
void ChartWindow::buildProbabilityPlot()
{
    chartBarsSeries = new QCPBars(chartBar->xAxis, chartBar->yAxis);
    chartBarsSeries->setName("Model Probability");

    titleBarChart = new QCPTextElement(chartBar, "test", QFont("sans", 12, QFont::Bold));

    chartBar->plotLayout()->insertRow(0);
    chartBar->plotLayout()->addElement(0, 0, titleBarChart);

    chartBar->yAxis->setRangeLower(0);
    chartBar->yAxis->setRangeUpper(1);
    chartBar->yAxis->setLabel("Probability");
    chartBar->yAxis->grid()->setPen(Qt::NoPen);

    chartBar->xAxis->setLabel("Model");
    chartBar->xAxis->setRangeLower(0);
    chartBar->xAxis->setRangeUpper(12);
    chartBar->xAxis->grid()->setPen(Qt::NoPen);
}

/**
 * @brief ChartWindow::plotProbabilities
 * @param index
 */
void ChartWindow::plotProbabilities(int index)
{
    mList = mDisplayData.at(index);

    if (mList.Header.contains("dropped", Qt::CaseInsensitive))
    {
        titleBarChart->setText(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    titleBarChart->setText(QString("Participant #%1: Model Probabilities").arg(QString::number(currentIndexShown + 1)));

    chartBarsSeries->data().data()->clear();

    expCheck = hypCheck = quasiCheck = myerCheck = rachCheck = rodriguezCheck = ebertCheck = bleichrodtCheck = false;

    QVector<QString> modelStrings({"Exponential", "Hyperbolic", "Beta Delta",
                                   "Green-Myerson", "Rachlin", "G. Hyperbolic",
                                   "Ebert-Prelec", "Beleichrodt", "Noise", "Parabolic",
                                   "Power"});

    QVector<double> modelTicks({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
    QVector<double> modelProbabilities({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});

    for (int i=0; i<mList.FittingResults.length(); i++)
    {

        if (mList.FittingResults[i]->Model == ModelType::Noise)
        {
            modelProbabilities[8] = mList.FittingResults[i]->Probability;
        }

        if (mList.FittingResults[i]->Model == ModelType::Parabolic)
        {
            paraK = mList.FittingResults[i]->Params.first().second;

            if (paraK != NULL)
            {
                modelProbabilities[9] = mList.FittingResults[i]->Probability;
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Power)
        {
            powerK = mList.FittingResults[i]->Params.first().second;

            if (powerK != NULL)
            {
                modelProbabilities[10] = mList.FittingResults[i]->Probability;
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Exponential)
        {
            expK = mList.FittingResults[i]->Params.first().second;

            if (expK != NULL)
            {
                modelProbabilities[0] = mList.FittingResults[i]->Probability;
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Hyperbolic)
        {
            hypK = mList.FittingResults[i]->Params.first().second;

            if (hypK != NULL)
            {
                modelProbabilities[1] = mList.FittingResults[i]->Probability;
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::BetaDelta)
        {
            quasiB = mList.FittingResults[i]->Params.first().second;
            quasiD = mList.FittingResults[i]->Params.last().second;

            if (quasiB != NULL && quasiD != NULL)
            {
                modelProbabilities[2] = mList.FittingResults[i]->Probability;
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Myerson)
        {
            myerK = mList.FittingResults[i]->Params.first().second;
            myerS = mList.FittingResults[i]->Params.last().second;

            if (myerK != NULL && myerS != NULL)
            {
                modelProbabilities[3] = mList.FittingResults[i]->Probability;
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Rachlin)
        {
            rachK = mList.FittingResults[i]->Params.first().second;
            rachS = mList.FittingResults[i]->Params.last().second;

            if (rachK != NULL && rachS != NULL)
            {
                modelProbabilities[4] = mList.FittingResults[i]->Probability;
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::GeneralizedHyperbolic)
        {
            rodriguezK = mList.FittingResults[i]->Params.first().second;
            rodriguezS = mList.FittingResults[i]->Params.last().second;

            if (rodriguezK != NULL && rodriguezS != NULL)
            {
                modelProbabilities[5] = mList.FittingResults[i]->Probability;
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::EbertPrelec)
        {
            ebertK = mList.FittingResults[i]->Params.first().second;
            ebertS = mList.FittingResults[i]->Params.last().second;

            if (ebertK != NULL && ebertS != NULL)
            {
                modelProbabilities[6] = mList.FittingResults[i]->Probability;
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Beleichrodt)
        {
            bleichrodtCheck = true;
            bleichrodtK = mList.FittingResults[i]->Params.first().second;
            bleichrodtS = mList.FittingResults[i]->Params[1].second;
            bleichrodtBeta = mList.FittingResults[i]->Params.last().second;

            if (bleichrodtK != NULL && bleichrodtS != NULL && bleichrodtBeta != NULL)
            {
                modelProbabilities[7] = mList.FittingResults[i]->Probability;
            }
        }
    }

    chartBarsSeries->setData(modelTicks, modelProbabilities);

    QSharedPointer<QCPAxisTickerText> chartTicker(new QCPAxisTickerText);
    chartTicker->addTicks(modelTicks, modelStrings);
    chartBar->xAxis->setTicker(chartTicker);

    chartBar->replot();
}

/**
 * @brief ChartWindow::exponential_plotting
 * @param k
 * @param x
 * @return
 */
double ChartWindow::exponential_plotting(double k, double x)
{
    if (isLogNormalParamerized)
    {
        return exp(-exp(log(k))*x);
    }
    else
    {
        return exp(-exp(k)*x);
    }
}

/**
 * @brief ChartWindow::parabolic_plotting
 * @param k
 * @param x
 * @return
 */
double ChartWindow::parabolic_plotting(double k, double x)
{
    if (isLogNormalParamerized)
    {
        return 1.0 - (k * pow(x, 2));
    }
    else
    {
        return 1.0 - (exp(k) * pow(x, 2));
    }
}

/**
 * @brief ChartWindow::parabolic_prediction
 * @param k
 * @param x
 * @return
 */
double ChartWindow::parabolic_prediction(double k, double x)
{
    return 1.0 - (exp(k) * pow(x, 2));
}

/**
 * @brief ChartWindow::hyperbolic_plotting
 * @param k
 * @param x
 * @return
 */
double ChartWindow::hyperbolic_plotting(double k, double x)
{
    if (isLogNormalParamerized)
    {
        return pow((1+exp(log(k))*x), -1);
    }
    else
    {
        return pow((1+exp(k)*x), -1);
    }
}

/**
 * @brief ChartWindow::quasi_hyperbolic_plotting
 * @param beta
 * @param delta
 * @param x
 * @return
 */
double ChartWindow::quasi_hyperbolic_plotting(double beta, double delta, double x)
{
    if (isLogNormalParamerized)
    {
        return beta * pow((1-delta), x);
    }
    else
    {
        return beta * pow(delta, x);
    }
}

/**
 * @brief ChartWindow::power_plotting
 * @param k
 * @param s
 * @param x
 * @return
 */
double ChartWindow::power_plotting(double k, double s, double x)
{
    if (isLogNormalParamerized)
    {
        return (1 - k * pow(x, s));
    }
    else
    {
        return (1 - exp(k) * pow(x, exp(s)));
    }
}

/**
 * @brief ChartWindow::power_prediction
 * @param k
 * @param s
 * @param x
 * @return
 */
double ChartWindow::power_prediction(double k, double s, double x)
{
    return (1 - exp(k) * pow(x, exp(s)));
}

/**
 * @brief ChartWindow::myerson_plotting
 * @param k
 * @param s
 * @param x
 * @return
 */
double ChartWindow::myerson_plotting(double k, double s, double x)
{
    if (isLogNormalParamerized)
    {
        return pow((1+exp(log(k))*x), -s);
    }
    else
    {
        return pow((1+exp(k)*x), -s);
    }
}

/**
 * @brief ChartWindow::rachlin_plotting
 * @param k
 * @param s
 * @param x
 * @return
 */
double ChartWindow::rachlin_plotting(double k, double s, double x)
{
    if (isLogNormalParamerized)
    {
        return pow((1+exp(log(k))*pow(x, s)), -1);
    }
    else
    {
        return pow((1+exp(k)*pow(x, s)), -1);
    }
}

/**
 * @brief ChartWindow::generalized_hyperbolic_plotting
 * @param k
 * @param s
 * @param x
 * @return
 */
double ChartWindow::generalized_hyperbolic_plotting(double k, double s, double x)
{
    if (isLogNormalParamerized)
    {
        return pow((1 + x * k),(-s/k));
    }
    else
    {
        return pow((1 + x * exp(k)),(-exp(s) / exp(k)));
    }
}

/**
 * @brief ChartWindow::generalized_hyperbolic_prediction
 * @param k
 * @param s
 * @param x
 * @return
 */
double ChartWindow::generalized_hyperbolic_prediction(double k, double s, double x)
{
    return pow((1 + x * exp(k)),(-exp(s) / exp(k)));
}

/**
 * @brief ChartWindow::ebert_prelec_plotting
 * @param k
 * @param s
 * @param x
 * @return
 */
double ChartWindow::ebert_prelec_plotting(double k, double s, double x)
{
    if (isLogNormalParamerized)
    {
        return exp(-pow((k*x),s));
    }
    else
    {
        return exp(-pow((exp(k)*x),s));
    }
}

/**
 * @brief ChartWindow::ebert_prelec_prediction
 * @param k
 * @param s
 * @param x
 * @return
 */
double ChartWindow::ebert_prelec_prediction(double k, double s, double x)
{
    return exp(-pow((exp(k)*x),s));
}

/**
 * @brief ChartWindow::bleichrodt_plotting
 * @param k
 * @param s
 * @param beta
 * @param x
 * @return
 */
double ChartWindow::bleichrodt_plotting(double k, double s, double beta, double x)
{
    if (isLogNormalParamerized)
    {
        return beta * exp(-k*pow(x,s));
    }
    else
    {
        return beta * exp(-exp(k)*pow(x,s));
    }
}

/**
 * @brief ChartWindow::bleichrodt_prediction
 * @param k
 * @param s
 * @param beta
 * @param x
 * @return
 */
double ChartWindow::bleichrodt_prediction(double k, double s, double beta, double x)
{
    return beta * exp(-exp(k)*pow(x,s));
}

/**
 * @brief ChartWindow::eventFilter
 * @param e
 * @return
 */
bool ChartWindow::eventFilter(QObject *, QEvent *e)
{
    if (e->type() == QEvent::ShortcutOverride)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);

        if (keyEvent->key() == Qt::Key::Key_Left)
        {
            on_PreviousButton_clicked();
        }
        else if (keyEvent->key() == Qt::Key::Key_Right)
        {
            on_NextButton_clicked();
        }
        else if (keyEvent->key() == Qt::Key::Key_Up)
        {
            if (stackedWidget.currentIndex() <= 0)
            {
                return false;
            }

            stackedWidget.setCurrentIndex(stackedWidget.currentIndex() - 1);
        }
        else if (keyEvent->key() == Qt::Key::Key_Down)
        {
            if (stackedWidget.currentIndex() >= (stackedWidget.count() - 1))
            {
                return false;
            }

            stackedWidget.setCurrentIndex(stackedWidget.currentIndex() + 1);
        }
    }

    return false;
}

/**
 * @brief ChartWindow::saveSVGasPNG
 */
void ChartWindow::saveImage()
{
    QString file_name;
    QString fileFilter = "PNG (*.png);;JPEG (*.jpeg);;BMP (*.bmp);;PDF (*.pdf)";
    QString selectedType;

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, tr("Save current chart?"),
                                                 QDir::homePath(),
                                                 fileFilter,
                                                 &selectedType,
                                                 QFileDialog::Option::DontUseNativeDialog);

#elif TARGET_OS_MAC

        file_name = QFileDialog::getSaveFileName(this, tr("Save PNG"), QDir::homePath(),
                                         fileFilter, &fileFilter, QFileDialog::Option::DontUseNativeDialog);

//        if (!file_name.contains(".png"))
//        {
//            file_name.append(".png");
//        }

#endif


    if(!file_name.trimmed().isEmpty())
    {
        switch (stackedWidget.currentIndex()) {
        case 0:
            saveChartImage(chart, file_name, selectedType);
            break;
        case 1:
            saveChartImage(chartArea, file_name, selectedType);
            break;
        case 2:
            saveChartImage(chartError, file_name, selectedType);
            break;
        case 3:
            saveChartImage(chartBar, file_name, selectedType);
            break;
        default:
            break;
        }
    }
}

void ChartWindow::saveChartImage(QCustomPlot *currPlot, QString filePath, QString fileExt)
{
    if (fileExt.contains(".png"))
    {
        currPlot->savePng(filePath.append(".png"));
    }
    else if (fileExt.contains(".bmp"))
    {
        currPlot->saveBmp(filePath.append(".bmp"));
    }
    else if (fileExt.contains(".pdf"))
    {
        currPlot->savePdf(filePath.append(".pdf"));
    }
    else if (fileExt.contains(".jpeg"))
    {
        currPlot->saveJpg(filePath.append(".jpeg"));
    }
}

/**
 * @brief ChartWindow::on_NextButton_clicked
 */
void ChartWindow::on_NextButton_clicked()
{
    if (currentIndexShown >= mDisplayData.count() - 1)
    {
        return;
    }

    currentIndexShown++;

    plotED50Series(currentIndexShown);
    plotAUCSeries(currentIndexShown);
    plotProbabilities(currentIndexShown);
    plotResiduals(currentIndexShown);
}

/**
 * @brief ChartWindow::on_PreviousButton_clicked
 */
void ChartWindow::on_PreviousButton_clicked()
{
    if (currentIndexShown <= 0)
    {
        return;
    }

    currentIndexShown--;

    plotED50Series(currentIndexShown);
    plotAUCSeries(currentIndexShown);
    plotProbabilities(currentIndexShown);
    plotResiduals(currentIndexShown);
}
