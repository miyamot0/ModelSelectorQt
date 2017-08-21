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
    mDisplayData = stringList;

    mList = mDisplayData[0];

    isED50Figure = (chartOption == ChartingOptions::ED50);
    isAUCLogFigure = (chartOption == ChartingOptions::AreaLog);

    if (chartOption == ChartingOptions::None)
    {
        isED50Figure = isAUCFigure = isAUCLogFigure = false;
    }

    if (!isED50Figure && !isAUCLogFigure)
    {
        close();
    }

    isLogNormalParamerized = isLogNormal;

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

    currentIndexShown = 0;

    installEventFilter(this);

    mTitle = QFont("Serif", 14, -1, false);
    mLegendFont = QFont("Serif", 12, -1, false);


    chart = new QChart();

    buildED50Plot();

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chartArea = new QChart();

    buildAUCPlot();

    chartViewArea = new QChartView(chartArea);
    chartViewArea->setRenderHint(QPainter::Antialiasing);

    windowLayout = new QVBoxLayout;
    stackedWidget = new QStackedWidget;
    stackedWidget->addWidget(chartView);
    stackedWidget->addWidget(chartViewArea);

    buildProbabilityPlot();

    stackedWidget->addWidget(barChartView);

    // Add stacked widget
    windowLayout->addWidget(stackedWidget);

    // Set window layout
    window = new QWidget(parent);
    window->setLayout(windowLayout);

    QToolBar *tb = new QToolBar();

    saveAction = new QAction(tr("Save"), this);
    connect(saveAction, &QAction::triggered, this, &ChartWindow::saveSVGasPNG);

    prevAction = new QAction(tr("Previous"), this);
    connect(prevAction, &QAction::triggered, this, &ChartWindow::on_PreviousButton_clicked);

    nextAction = new QAction(tr("Next"), this);
    connect(nextAction, &QAction::triggered, this, &ChartWindow::on_NextButton_clicked);

    tb->addAction(saveAction);
    tb->addAction(prevAction);
    tb->addAction(nextAction);

    setWindowTitle("Discounting Model Selection Plots");
    windowLayout->setMenuBar(tb);
    setCentralWidget(window);
    resize(800, 600);

    setWindowFlags(Qt::WindowStaysOnTopHint);

    plotED50Series(0);
    plotAUCSeries(0);
}

void ChartWindow::buildProbabilityPlot()
{
    barChart = new QChart();
    barChart->setTitleFont(mTitle);
    barChart->setFont(mTitle);

    expProbSet = new QBarSet("Exponential");
    expProbSet->setColor(Qt::black);
    expProbSet->setBorderColor(Qt::black);

    hypProbSet = new QBarSet("Hyperbolic");
    hypProbSet->setColor(Qt::green);
    hypProbSet->setBorderColor(Qt::green);

    quasiProbSet = new QBarSet("QuasiHyperbolic");
    quasiProbSet->setColor(Qt::blue);
    quasiProbSet->setBorderColor(Qt::blue);

    myerProbSet = new QBarSet("Green-Myerson");
    myerProbSet->setColor(Qt::cyan);
    myerProbSet->setBorderColor(Qt::cyan);

    rachProbSet = new QBarSet("Rachlin");
    rachProbSet->setColor(Qt::magenta);
    rachProbSet->setBorderColor(Qt::magenta);

    rodriguezProbSet = new QBarSet("Rodriguez-Logue");
    rodriguezProbSet->setColor(Qt::yellow);
    rodriguezProbSet->setBorderColor(Qt::yellow);

    ebertProbSet = new QBarSet("Ebert-Prelec");
    ebertProbSet->setColor(Qt::red);
    ebertProbSet->setBorderColor(Qt::red);

    bleichrodtProbSet = new QBarSet("Belechrodt");
    bleichrodtProbSet->setColor(Qt::darkCyan);
    bleichrodtProbSet->setBorderColor(Qt::darkCyan);

    noiseProbSet = new QBarSet("Noise");
    noiseProbSet->setColor(Qt::darkGray);
    noiseProbSet->setBorderColor(Qt::darkGray);

    for (int l=0; l<9; l++)
    {
        *expProbSet << NULL;
        *hypProbSet << NULL;
        *quasiProbSet << NULL;
        *myerProbSet << NULL;
        *rachProbSet << NULL;
        *rodriguezProbSet << NULL;
        *ebertProbSet << NULL;
        *bleichrodtProbSet << NULL;
        *noiseProbSet << NULL;
    }

    barSeries = new QStackedBarSeries();
    barSeries->append(expProbSet);
    barSeries->append(hypProbSet);
    barSeries->append(quasiProbSet);
    barSeries->append(myerProbSet);
    barSeries->append(rachProbSet);
    barSeries->append(rodriguezProbSet);
    barSeries->append(ebertProbSet);
    barSeries->append(bleichrodtProbSet);
    barSeries->append(noiseProbSet);

    barChart->addSeries(barSeries);
    barChart->setTitle("Simple barchart example");

    modelAxisCategories << "Exponential" <<
                           "Hyperbolic" <<
                           "QuasiHyperbolic" <<
                           "Green-Myerson" <<
                           "Rachlin" <<
                           "Rodriguez-Logue" <<
                           "Ebert-Prelec" <<
                           "Belechrodt" <<
                           "Noise";

    barAxisX = new QBarCategoryAxis();
    barAxisX->append(modelAxisCategories);
    barAxisX->setGridLineColor(Qt::transparent);
    barAxisX->setTitleText("Model Candidates");
    barAxisX->setLabelsFont(mLegendFont);
    barAxisX->setLinePenColor(Qt::black);
    barAxisX->setLinePen(QPen(Qt::black));

    barChart->setAxisX(barAxisX, barSeries);
    //barChart->setAnimationOptions(QChart::SeriesAnimations);

    barAxisY = new QValueAxis();
    barAxisY->setMin(0.0);
    barAxisY->setMax(1.0);
    barAxisY->setTitleText("Probability");
    barAxisY->setGridLineColor(Qt::transparent);
    barAxisY->setLabelsFont(mLegendFont);
    barAxisY->setLinePenColor(Qt::black);
    barAxisY->setLinePen(QPen(Qt::black));
    barChart->setAxisY(barAxisY, barSeries);

    barChart->legend()->setVisible(false);

    barChartView = new QChartView(barChart);
}

void ChartWindow::buildAUCPlot()
{
    chartArea->setTitleFont(mTitle);

    auto mLegendArea = chartArea->legend();

    mLegendArea->setFont(mLegendFont);
    mLegendArea->setAlignment(Qt::AlignBottom);

    axisXarea = new QValueAxis;
    axisXarea->setGridLineColor(Qt::transparent);
    axisXarea->setTitleText("ln(Delay)");
    axisXarea->setMin(0);
    axisXarea->setLabelsFont(mLegendFont);
    axisXarea->setLabelFormat(QString("%.0f"));
    axisXarea->setLinePenColor(Qt::black);
    axisXarea->setLinePen(QPen(Qt::black));

    axisYarea = new QValueAxis;
    axisYarea->setGridLineColor(Qt::transparent);
    axisYarea->setTitleText("Value");
    axisYarea->setTickCount(5);
    axisYarea->setLabelsFont(mLegendFont);
    axisYarea->setMin(0);
    axisYarea->setMax(1);
    axisYarea->setLinePenColor(Qt::black);
    axisYarea->setLinePen(QPen(Qt::black));

    expSeriesArea = new QLineSeries();
    expSeriesArea->setName("Exponential");
    chartArea->addSeries(expSeriesArea);

    hypSeriesArea = new QLineSeries();
    hypSeriesArea->setName("Hyperbolic");
    chartArea->addSeries(hypSeriesArea);

    quasiSeriesArea = new QLineSeries();
    quasiSeriesArea->setName("QuasiHyperbolic");
    chartArea->addSeries(quasiSeriesArea);

    myerSeriesArea = new QLineSeries();
    myerSeriesArea->setName("Green-Myerson");
    chartArea->addSeries(myerSeriesArea);

    rachSeriesArea = new QLineSeries();
    rachSeriesArea->setName("Rachlin");
    chartArea->addSeries(rachSeriesArea);

    rodriguezSeriesArea = new QLineSeries();
    rodriguezSeriesArea->setName("Rodriguez-Logue");
    chartArea->addSeries(rodriguezSeriesArea);

    ebertSeriesArea = new QLineSeries();
    ebertSeriesArea->setName("Ebert-Prelec");
    chartArea->addSeries(ebertSeriesArea);

    bleichrodtSeriesArea = new QLineSeries();
    bleichrodtSeriesArea->setName("Bleichrodt");
    chartArea->addSeries(bleichrodtSeriesArea);

    noiseSeriesArea = new QLineSeries();
    noiseSeriesArea->setName("Noise");
    noiseSeriesArea->setPen(QPen(Qt::black));
    chartArea->addSeries(noiseSeriesArea);

    empiricalSeries = new QLineSeries();
    empiricalSeries->setName("AUC");
    empiricalSeries->setPen(QPen(Qt::black));
    chartArea->addSeries(empiricalSeries);

    dataPointsArea = new QScatterSeries();
    dataPointsArea->setName("Raw Data");
    dataPointsArea->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    dataPointsArea->setPen(QPen(Qt::black));
    dataPointsArea->setBrush(QBrush(Qt::black));
    dataPointsArea->setMarkerSize(10);
    chartArea->addSeries(dataPointsArea);

    chartArea->setAxisX(axisXarea, expSeriesArea);
    chartArea->setAxisY(axisYarea, expSeriesArea);

    chartArea->setAxisX(axisXarea, hypSeriesArea);
    chartArea->setAxisY(axisYarea, hypSeriesArea);

    chartArea->setAxisX(axisXarea, quasiSeriesArea);
    chartArea->setAxisY(axisYarea, quasiSeriesArea);

    chartArea->setAxisX(axisXarea, myerSeriesArea);
    chartArea->setAxisY(axisYarea, myerSeriesArea);

    chartArea->setAxisX(axisXarea, rachSeriesArea);
    chartArea->setAxisY(axisYarea, rachSeriesArea);

    chartArea->setAxisX(axisXarea, rodriguezSeriesArea);
    chartArea->setAxisY(axisYarea, rodriguezSeriesArea);

    chartArea->setAxisX(axisXarea, ebertSeriesArea);
    chartArea->setAxisY(axisYarea, ebertSeriesArea);

    chartArea->setAxisX(axisXarea, bleichrodtSeriesArea);
    chartArea->setAxisY(axisYarea, bleichrodtSeriesArea);

    chartArea->setAxisX(axisXarea, noiseSeriesArea);
    chartArea->setAxisY(axisYarea, noiseSeriesArea);

    chartArea->setAxisX(axisXarea, dataPointsArea);
    chartArea->setAxisY(axisYarea, dataPointsArea);

    chartArea->setAxisX(axisXarea, empiricalSeries);
    chartArea->setAxisY(axisYarea, empiricalSeries);

    //installEventFilter(this);
}

void ChartWindow::buildED50Plot()
{
    chart->setTitleFont(mTitle);

    auto mLegend = chart->legend();

    mLegend->setFont(mLegendFont);
    mLegend->setAlignment(Qt::AlignBottom);

    axisX = new QValueAxis;
    axisX->setGridLineColor(Qt::transparent);
    axisX->setTitleText("ln(Delay)");
    axisX->setMin(0);
    axisX->setLabelsFont(mLegendFont);
    axisX->setLabelFormat(QString("%.0f"));
    axisX->setLinePenColor(Qt::black);
    axisX->setLinePen(QPen(Qt::black));

    axisY = new QValueAxis;
    axisY->setGridLineColor(Qt::transparent);
    axisY->setTitleText("Value");
    axisY->setTickCount(5);
    axisY->setLabelsFont(mLegendFont);
    axisY->setMin(0);
    axisY->setMax(1);
    axisY->setLinePenColor(Qt::black);
    axisY->setLinePen(QPen(Qt::black));

    expSeries = new QLineSeries();
    expSeries->setName("Exponential");
    expSeries->setPen(QPen(Qt::black));
    chart->addSeries(expSeries);

    hypSeries = new QLineSeries();
    hypSeries->setName("Hyperbolic");
    hypSeries->setPen(QPen(Qt::green));
    chart->addSeries(hypSeries);

    quasiSeries = new QLineSeries();
    quasiSeries->setName("QuasiHyperbolic");
    quasiSeries->setPen(QPen(Qt::blue));
    chart->addSeries(quasiSeries);

    myerSeries = new QLineSeries();
    myerSeries->setName("Green-Myerson");
    myerSeries->setPen(QPen(Qt::cyan));
    chart->addSeries(myerSeries);

    rachSeries = new QLineSeries();
    rachSeries->setName("Rachlin");
    rachSeries->setPen(QPen(Qt::magenta));
    chart->addSeries(rachSeries);

    rodriguezSeries = new QLineSeries();
    rodriguezSeries->setName("Rodriguez-Logue");
    rodriguezSeries->setPen(QPen(Qt::yellow));
    chart->addSeries(rodriguezSeries);

    ebertSeries = new QLineSeries();
    ebertSeries->setName("Ebert-Prelec");
    ebertSeries->setPen(QPen(Qt::red));
    chart->addSeries(ebertSeries);

    bleichrodtSeries = new QLineSeries();
    bleichrodtSeries->setName("Bleichrodt");
    bleichrodtSeries->setPen(QPen(Qt::darkCyan));
    chart->addSeries(bleichrodtSeries);

    noiseSeries = new QLineSeries();
    noiseSeries->setName("Noise");
    noiseSeries->setPen(QPen(QPen(Qt::darkGray)));
    chart->addSeries(noiseSeries);

    dataPoints = new QScatterSeries();
    dataPoints->setName("Raw Data");
    dataPoints->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    dataPoints->setPen(QPen(Qt::black));
    dataPoints->setBrush(QBrush(Qt::black));
    dataPoints->setMarkerSize(10);
    chart->addSeries(dataPoints);

    chart->setAxisX(axisX, expSeries);
    chart->setAxisY(axisY, expSeries);

    chart->setAxisX(axisX, hypSeries);
    chart->setAxisY(axisY, hypSeries);

    chart->setAxisX(axisX, quasiSeries);
    chart->setAxisY(axisY, quasiSeries);

    chart->setAxisX(axisX, myerSeries);
    chart->setAxisY(axisY, myerSeries);

    chart->setAxisX(axisX, rachSeries);
    chart->setAxisY(axisY, rachSeries);

    chart->setAxisX(axisX, rodriguezSeries);
    chart->setAxisY(axisY, rodriguezSeries);

    chart->setAxisX(axisX, ebertSeries);
    chart->setAxisY(axisY, ebertSeries);

    chart->setAxisX(axisX, bleichrodtSeries);
    chart->setAxisY(axisY, bleichrodtSeries);

    chart->setAxisX(axisX, noiseSeries);
    chart->setAxisY(axisY, noiseSeries);

    chart->setAxisX(axisX, dataPoints);
    chart->setAxisY(axisY, dataPoints);
}

void ChartWindow::plotAUCSeries(int index)
{
    mList = mDisplayData.at(index);

    expSeriesArea->clear();
    hypSeriesArea->clear();
    quasiSeriesArea->clear();
    myerSeriesArea->clear();
    rachSeriesArea->clear();
    rodriguezSeriesArea->clear();
    ebertSeriesArea->clear();
    bleichrodtSeriesArea->clear();
    noiseSeriesArea->clear();
    dataPointsArea->clear();
    empiricalSeries->clear();

    if (mList.Header.contains("dropped", Qt::CaseInsensitive))
    {
        chartArea->setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    chartArea->setTitle(QString("Participant #%1: %2 Scaled AUC = %3").arg(QString::number(currentIndexShown + 1)).arg(mList.TopModel).arg(mList.TopAUCLog));

    expSeriesArea->hide();
    hypSeriesArea->hide();
    quasiSeriesArea->hide();
    myerSeriesArea->hide();
    rachSeriesArea->hide();
    rodriguezSeriesArea->hide();
    ebertSeriesArea->hide();
    bleichrodtSeriesArea->hide();
    noiseSeriesArea->hide();

    switch (mList.TopModelType)
    {
        case ModelType::Noise:
            noiseSeriesArea->show();
            break;

        case ModelType::Exponential:
            expSeriesArea->show();
            break;

        case ModelType::Hyperbolic:
            hypSeriesArea->show();
            break;

        case ModelType::BetaDelta:
            quasiSeriesArea->show();
            break;

        case ModelType::Myerson:
            myerSeriesArea->show();
            break;

        case ModelType::Rachlin:
            rachSeriesArea->show();
            break;

        case ModelType::RodriguezLogue:
            rodriguezSeriesArea->show();
            break;

        case ModelType::EbertPrelec:
            ebertSeriesArea->show();
            break;

        case ModelType::Beleichrodt:
            bleichrodtSeriesArea->show();
            break;
    }

    expCheck = hypCheck = quasiCheck = myerCheck = rachCheck = rodriguezCheck = ebertCheck = bleichrodtCheck = false;

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

        if (mList.FittingResults[i]->Model == ModelType::RodriguezLogue)
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

    int tickHack = ((int) log(lastDelay)) + 1;

    axisXarea->setMax(tickHack);
    axisXarea->setTickCount(tickHack + 1);

    for (double i = 0; i < (log(lastDelay)+1); i = i + 0.1)
    {
        xParam = exp(i);

        *noiseSeriesArea << QPointF(i, noise);

        if (expCheck)
        {
            *expSeriesArea << QPointF(i, exponential_plotting(expK, xParam));
        }

        if (hypCheck)
        {
            *hypSeriesArea << QPointF(i, hyperbolic_plotting(hypK, xParam));
        }

        if (quasiCheck)
        {
             *quasiSeriesArea << QPointF(i, quasi_hyperbolic_plotting(quasiB, quasiD, xParam));
        }

        if (myerCheck)
        {
            *myerSeriesArea << QPointF(i, myerson_plotting(myerK, myerS, xParam));
        }

        if (rachCheck)
        {
            *rachSeriesArea << QPointF(i, rachlin_plotting(rachK, rachS, xParam));
        }

        if (rodriguezCheck)
        {
            *rodriguezSeriesArea << QPointF(i, rodriguez_logue_plotting(rodriguezK, rodriguezS, xParam));
        }

        if (ebertCheck)
        {
            *ebertSeriesArea << QPointF(i, ebert_prelec_plotting(ebertK, ebertS, xParam));
        }

        if (bleichrodtCheck)
        {
            *bleichrodtSeriesArea << QPointF(i, bleichrodt_plotting(bleichrodtK, bleichrodtS, bleichrodtBeta, xParam));
        }
    }

    paramString1 = mList.ParticipantDelays;
    paramString2 = mList.ParticipantValues;

    delayPoints = paramString1.split(",");
    valuePoints = paramString2.split(",");

    for (int i = 0; i < delayPoints.length(); i++)
    {
        param1 = delayPoints[i].toDouble(&checkValue1);
        param2 = valuePoints[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        *dataPointsArea << QPointF(log(param1), param2);
        *empiricalSeries << QPointF(log(param1), param2);
    }
}

void ChartWindow::plotED50Series(int index)
{
    mList = mDisplayData.at(index);

    expSeries->clear();
    hypSeries->clear();
    quasiSeries->clear();
    myerSeries->clear();
    rachSeries->clear();
    noiseSeries->clear();
    rodriguezSeries->clear();
    ebertSeries->clear();
    bleichrodtSeries->clear();
    dataPoints->clear();

    expSeries->hide();
    hypSeries->hide();
    quasiSeries->hide();
    myerSeries->hide();
    rachSeries->hide();
    noiseSeries->hide();
    rodriguezSeries->hide();
    ebertSeries->hide();
    bleichrodtSeries->hide();

    for (int l=0; l<9; l++)
    {
        expProbSet->replace(l, NULL);
        expProbSet->replace(l, NULL);
        hypProbSet->replace(l, NULL);
        quasiProbSet->replace(l, NULL);
        myerProbSet->replace(l, NULL);
        rachProbSet->replace(l, NULL);
        rodriguezProbSet->replace(l, NULL);
        ebertProbSet->replace(l, NULL);
        bleichrodtProbSet->replace(l, NULL);
        noiseProbSet->replace(l, NULL);
    }

    if (mList.Header.contains("dropped", Qt::CaseInsensitive))
    {
        chart->setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));
        barChart->setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    chart->setTitle(QString("Participant #%1: %2 ln(ED50) = %3").arg(QString::number(currentIndexShown + 1)).arg(mList.TopModel).arg(mList.TopED50));
    barChart->setTitle(QString("Participant #%1: %2 ln(ED50) = %3").arg(QString::number(currentIndexShown + 1)).arg(mList.TopModel).arg(mList.TopED50));

    expCheck = hypCheck = quasiCheck = myerCheck = rachCheck = rodriguezCheck = ebertCheck = bleichrodtCheck = false;

    for (int i=0; i<mList.FittingResults.length(); i++)
    {
        if (mList.FittingResults[i]->Model == ModelType::Noise)
        {
            noise = mList.FittingResults[i]->Params.first().second;
            noiseProbSet->replace(8, mList.FittingResults[i]->Probability);
        }

        if (mList.FittingResults[i]->Model == ModelType::Exponential)
        {
            expK = mList.FittingResults[i]->Params.first().second;

            if (expK != NULL)
            {
                expCheck = true;
                expSeries->show();
                expProbSet->replace(0, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Hyperbolic)
        {
            hypK = mList.FittingResults[i]->Params.first().second;

            if (hypK != NULL)
            {
                hypCheck = true;
                hypSeries->show();
                hypProbSet->replace(1, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::BetaDelta)
        {
            quasiB = mList.FittingResults[i]->Params.first().second;
            quasiD = mList.FittingResults[i]->Params.last().second;

            if (quasiB != NULL && quasiD != NULL)
            {
                quasiCheck = true;
                quasiSeries->show();
                quasiProbSet->replace(2, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Myerson)
        {
            myerK = mList.FittingResults[i]->Params.first().second;
            myerS = mList.FittingResults[i]->Params.last().second;

            if (myerK != NULL && myerS != NULL)
            {
                myerCheck = true;
                myerSeries->show();
                myerProbSet->replace(3, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Rachlin)
        {
            rachK = mList.FittingResults[i]->Params.first().second;
            rachS = mList.FittingResults[i]->Params.last().second;

            if (rachK != NULL && rachS != NULL)
            {
                rachCheck = true;
                rachSeries->show();
                rachProbSet->replace(4, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::RodriguezLogue)
        {
            rodriguezK = mList.FittingResults[i]->Params.first().second;
            rodriguezS = mList.FittingResults[i]->Params.last().second;

            if (rodriguezK != NULL && rodriguezS != NULL)
            {
                rodriguezCheck = true;
                rodriguezSeries->show();
                rodriguezProbSet->replace(5, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::EbertPrelec)
        {
            ebertK = mList.FittingResults[i]->Params.first().second;
            ebertS = mList.FittingResults[i]->Params.last().second;

            if (ebertK != NULL && ebertS != NULL)
            {
                ebertCheck = true;
                ebertSeries->show();
                ebertProbSet->replace(6, mList.FittingResults[i]->Probability);
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
                bleichrodtSeries->show();
                bleichrodtProbSet->replace(7, mList.FittingResults[i]->Probability);
            }
        }
    }

    int tickHack = ((int) log(lastDelay)) + 1;

    axisX->setMax(tickHack);
    axisX->setTickCount(tickHack + 1);

    for (double i = 0; i < (log(lastDelay)+1); i = i + 0.1)
    {
        xParam = exp(i);

        *noiseSeries << QPointF(i, noise);

        if (expCheck)
        {
            *expSeries << QPointF(i, exponential_plotting(expK, xParam));
        }

        if (hypCheck)
        {
            *hypSeries << QPointF(i, hyperbolic_plotting(hypK, xParam));
        }

        if (quasiCheck)
        {
             *quasiSeries << QPointF(i, quasi_hyperbolic_plotting(quasiB, quasiD, xParam));
        }

        if (myerCheck)
        {
            *myerSeries << QPointF(i, myerson_plotting(myerK, myerS, xParam));
        }

        if (rachCheck)
        {
            *rachSeries << QPointF(i, rachlin_plotting(rachK, rachS, xParam));
        }

        if (rodriguezCheck)
        {
            *rodriguezSeries << QPointF(i, rodriguez_logue_plotting(rodriguezK, rodriguezS, xParam));
        }

        if (ebertCheck)
        {
            *ebertSeries << QPointF(i, ebert_prelec_plotting(ebertK, ebertS, xParam));
        }

        if (bleichrodtCheck)
        {
            *bleichrodtSeries << QPointF(i, bleichrodt_plotting(bleichrodtK, bleichrodtS, bleichrodtBeta, xParam));
        }
    }

    paramString1 = mList.ParticipantDelays;
    paramString2 = mList.ParticipantValues;

    delayPoints = paramString1.split(",");
    valuePoints = paramString2.split(",");

    for (int i = 0; i < delayPoints.length(); i++)
    {
        param1 = delayPoints[i].toDouble(&checkValue1);
        param2 = valuePoints[i].toDouble(&checkValue2);

        if (!checkValue1 || !checkValue2)
        {
            break;
        }

        *dataPoints << QPointF(log(param1), param2);
    }
}

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

double ChartWindow::quasi_hyperbolic_plotting(double beta, double delta, double x)
{
    return beta * pow(delta, x);
}

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

double ChartWindow::rodriguez_logue_plotting(double k, double s, double x)
{
    return pow((1 + x * exp(k)),(-exp(s) / exp(k)));
}

double ChartWindow::ebert_prelec_plotting(double k, double s, double x)
{
    return exp(-pow((exp(k)*x),s));
}

double ChartWindow::bleichrodt_plotting(double k, double s, double beta, double x)
{
    return beta * exp(-exp(k)*pow(x,s));
}

bool ChartWindow::eventFilter(QObject *object, QEvent *e)
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
            if (stackedWidget->currentIndex() <= 0)
            {
                return false;
            }

            stackedWidget->setCurrentIndex(stackedWidget->currentIndex() - 1);
        }
        else if (keyEvent->key() == Qt::Key::Key_Down)
        {
            if (stackedWidget->currentIndex() >= (stackedWidget->count() - 1))
            {
                return false;
            }

            stackedWidget->setCurrentIndex(stackedWidget->currentIndex() + 1);
        }
    }

    return false;
}

void ChartWindow::saveSVGasPNG()
{
    QString file_name;
    QString fileFilter = "PNG (*.png)";

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, "Save PNG", QDir::homePath(),
                                         fileFilter);

#elif TARGET_OS_MAC

        file_name = QFileDialog::getSaveFileName(this, "Save PNG", QDir::homePath(),
                                         fileFilter, &fileFilter, QFileDialog::Option::DontUseNativeDialog);

        if (!file_name.contains(".png"))
        {
            file_name.append(".png");
        }

#endif

    if(!file_name.trimmed().isEmpty())
    {
        chartView->grab().save(file_name, "PNG", 9);
    }
}

void ChartWindow::on_NextButton_clicked()
{
    if (currentIndexShown >= mDisplayData.count() - 1)
    {
        return;
    }

    currentIndexShown++;

    plotED50Series(currentIndexShown);
    plotAUCSeries(currentIndexShown);
}

void ChartWindow::on_PreviousButton_clicked()
{
    if (currentIndexShown <= 0)
    {
        return;
    }

    currentIndexShown--;

    plotED50Series(currentIndexShown);
    plotAUCSeries(currentIndexShown);
}
