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

    //buildAUCPlot();
    stackedWidget.addWidget(chartArea);

    //buildErrorPlot();
    stackedWidget.addWidget(chartError);

    //buildProbabilityPlot();
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
    connect(saveAction, &QAction::triggered, this, &ChartWindow::saveSVGasPNG);

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

    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    chart->plotLayout()->addElement(1, 1, subLayout);
    subLayout->setMargins(QMargins(0, 0, 10, 0));
    subLayout->addElement(0, 1, new QCPLayoutElement);
    subLayout->addElement(1, 1, chart->legend);
    subLayout->addElement(2, 1, new QCPLayoutElement);
    chart->plotLayout()->setColumnStretchFactor(1, 0.001);

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

    if (hypCheck)
    {
        chart->graph(ModelHyperbolic)->addData(xParam, hyperbolic_plotting(hypK, xParam));
    }

    if (quasiCheck)
    {
        chart->graph(ModelQuasiHyperbolic)->addData(xParam, quasi_hyperbolic_plotting(quasiB, quasiD, xParam));
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

    for (int i = 0; i < 10; i++)
    {
        chart->graph(i)->setVisible(false);
        chart->graph(i)->data().data()->clear();
    }

    chart->graph(RawData)->setVisible(true);
    chart->graph(ModelNoise)->setVisible(true);

    if (mList.Header.contains("dropped", Qt::CaseInsensitive))
    {
        titleMainChart->setText(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    titleMainChart->setText(QString("Participant #%1: %2 ln(ED50) = %3").arg(QString::number(currentIndexShown + 1)).arg(cleanTitle(mList.TopModel)).arg(mList.TopED50));

    expCheck = hypCheck = quasiCheck = myerCheck = rachCheck = rodriguezCheck = ebertCheck = bleichrodtCheck = false;

    for (int i=0; i<mList.FittingResults.length(); i++)
    {
        if (mList.FittingResults[i]->Model == ModelType::Noise)
        {
            noise = mList.FittingResults[i]->Params.first().second;
        }

        if (mList.FittingResults[i]->Model == ModelType::Exponential)
        {
            expK = mList.FittingResults[i]->Params.first().second;

            if (expK != NULL)
            {
                expCheck = true;
                chart->graph(ModelExponential)->setVisible(true);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Hyperbolic)
        {
            hypK = mList.FittingResults[i]->Params.first().second;

            if (hypK != NULL)
            {
                hypCheck = true;
                chart->graph(ModelHyperbolic)->setVisible(true);
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

/**
 * @brief ChartWindow::buildProbabilityPlot
 */
void ChartWindow::buildProbabilityPlot()
{

    /*
    barChart.setTitleFont(mTitle);
    barChart.setFont(mTitle);
    barChart.setTitleBrush(Qt::black);

    barChart.legend()->setFont(mLegendFont);
    barChart.legend()->setAlignment(Qt::AlignBottom);
    barChart.legend()->setLabelBrush(Qt::black);
    barChart.legend()->setLabelColor(Qt::black);
    barChart.legend()->setFont(QFont("Serif", 8, -1, false));

    expPen.setBrush(Qt::black);
    hypPen.setBrush(Qt::green);
    quasiPen.setBrush(Qt::blue);
    myerPen.setBrush(Qt::cyan);
    rachPen.setBrush(Qt::magenta);
    rodriguezPen.setBrush(Qt::yellow);
    ebertPen.setBrush(Qt::red);
    bleichrodtPen.setBrush(Qt::darkCyan);
    noisePen.setBrush(Qt::darkGray);

    expPen.setWidthF(colWidth);
    hypPen.setWidthF(colWidth);
    quasiPen.setWidthF(colWidth);
    myerPen.setWidthF(colWidth);
    rachPen.setWidthF(colWidth);
    rodriguezPen.setWidthF(colWidth);
    ebertPen.setWidthF(colWidth);
    bleichrodtPen.setWidthF(colWidth);
    noisePen.setWidthF(colWidth);

    expProbSet.setName("Exponential");
    expProbSet.setUseOpenGL(true);
    expProbSet.setColor(Qt::black);
    expProbSet.setPen(expPen);

    hypProbSet.setName("Hyperbolic");
    hypProbSet.setUseOpenGL(true);
    hypProbSet.setColor(Qt::green);
    hypProbSet.setPen(hypPen);

    quasiProbSet.setName("QuasiHyperbolic");
    quasiProbSet.setUseOpenGL(true);
    quasiProbSet.setColor(Qt::blue);
    quasiProbSet.setPen(quasiPen);

    myerProbSet.setName("Green-Myerson");
    myerProbSet.setUseOpenGL(true);
    myerProbSet.setColor(Qt::cyan);
    myerProbSet.setPen(myerPen);

    rachProbSet.setName("Rachlin");
    rachProbSet.setUseOpenGL(true);
    rachProbSet.setColor(Qt::magenta);
    rachProbSet.setPen(rachPen);

    rodriguezProbSet.setName("Generalized-Hyperbolic");
    rodriguezProbSet.setUseOpenGL(true);
    rodriguezProbSet.setColor(Qt::yellow);
    rodriguezProbSet.setPen(rodriguezPen);

    ebertProbSet.setName("Ebert-Prelec");
    ebertProbSet.setUseOpenGL(true);
    ebertProbSet.setColor(Qt::red);
    ebertProbSet.setPen(ebertPen);

    bleichrodtProbSet.setName("Bleichrodt");
    bleichrodtProbSet.setUseOpenGL(true);
    bleichrodtProbSet.setColor(Qt::darkCyan);
    bleichrodtProbSet.setPen(bleichrodtPen);

    noiseProbSet.setName("Noise");
    noiseProbSet.setUseOpenGL(true);
    noiseProbSet.setColor(Qt::darkGray);
    noiseProbSet.setPen(noisePen);

    barChart.addSeries(&expProbSet);
    barChart.addSeries(&hypProbSet);
    barChart.addSeries(&quasiProbSet);
    barChart.addSeries(&myerProbSet);
    barChart.addSeries(&rachProbSet);
    barChart.addSeries(&rodriguezProbSet);
    barChart.addSeries(&ebertProbSet);
    barChart.addSeries(&bleichrodtProbSet);
    barChart.addSeries(&noiseProbSet);

    barAxisX.setMin(0);
    barAxisX.setMax(10);
    barAxisX.setTickCount(11);
    barAxisX.applyNiceNumbers();
    barAxisX.setGridLineColor(Qt::transparent);
    barAxisX.setTitleText(tr("Model Candidates"));
    barAxisX.setTitleBrush(Qt::white);
    barAxisX.setTitleFont(QFont("Serif", 10, -1, false));
    barAxisX.setLabelsFont(QFont("Serif", 7, -1, false));
    barAxisX.setLabelsColor(Qt::white);
    barAxisX.setLinePenColor(Qt::black);
    barAxisX.setLinePen(QPen(Qt::black));

    barAxisY.setMin(0.0);
    barAxisY.setMax(1.0);
    barAxisY.setTitleText(tr("Probability"));
    barAxisY.setTitleFont(QFont("Serif", 10, -1, false));
    barAxisY.setTitleBrush(Qt::black);
    barAxisY.setGridLineColor(Qt::transparent);
    barAxisY.setLabelsFont(QFont("Serif", 10, -1, false));
    barAxisY.setLabelsColor(Qt::black);
    barAxisY.setLinePenColor(Qt::black);
    barAxisY.setLinePen(QPen(Qt::black));

    barChart.setAxisX(&barAxisX, &expProbSet);
    barChart.setAxisY(&barAxisY, &expProbSet);

    barChart.setAxisX(&barAxisX, &hypProbSet);
    barChart.setAxisY(&barAxisY, &hypProbSet);

    barChart.setAxisX(&barAxisX, &quasiProbSet);
    barChart.setAxisY(&barAxisY, &quasiProbSet);

    barChart.setAxisX(&barAxisX, &myerProbSet);
    barChart.setAxisY(&barAxisY, &myerProbSet);

    barChart.setAxisX(&barAxisX, &rachProbSet);
    barChart.setAxisY(&barAxisY, &rachProbSet);

    barChart.setAxisX(&barAxisX, &rodriguezProbSet);
    barChart.setAxisY(&barAxisY, &rodriguezProbSet);

    barChart.setAxisX(&barAxisX, &ebertProbSet);
    barChart.setAxisY(&barAxisY, &ebertProbSet);

    barChart.setAxisX(&barAxisX, &bleichrodtProbSet);
    barChart.setAxisY(&barAxisY, &bleichrodtProbSet);

    barChart.setAxisX(&barAxisX, &noiseProbSet);
    barChart.setAxisY(&barAxisY, &noiseProbSet);

    barChartView = new QChartView(&barChart);
    */
}

/**
 * @brief ChartWindow::buildErrorPlot
 */
void ChartWindow::buildErrorPlot()
{

    /*
    chartError.setTitleFont(mTitle);
    chartError.setFont(mTitle);
    chartError.setTitleBrush(Qt::black);

    chartError.legend()->setFont(mLegendFont);
    chartError.legend()->setAlignment(Qt::AlignBottom);
    chartError.legend()->setBrush(Qt::black);
    chartError.legend()->setColor(Qt::black);

    // do we need a table?
    chartError.legend()->setVisible(false);

    axisXerror.setGridLineColor(Qt::transparent);
    axisXerror.setTitleText(tr("Residual"));
    axisXerror.setMin(0);
    axisXerror.setLabelsFont(QFont("Serif", 10, -1, false));
    axisXerror.setLabelsColor(Qt::black);
    axisXerror.setLabelFormat(QString("%.0f"));
    axisXerror.setLinePenColor(Qt::black);
    axisXerror.setLinePen(QPen(Qt::black));
    axisXerror.setTitleBrush(Qt::black);
    axisXerror.setTitleFont(QFont("Serif", 10, -1, false));

    axisYerror.setGridLineColor(Qt::transparent);
    axisYerror.setTitleText(tr("Error Value Value"));
    axisYerror.setTickCount(9);
    axisYerror.setLabelsFont(QFont("Serif", 10, -1, false));
    axisYerror.setLabelsColor(Qt::black);
    axisYerror.setMin(-1);
    axisYerror.setMax(1);
    axisYerror.setLinePenColor(Qt::black);
    axisYerror.setLinePen(QPen(Qt::black));
    axisYerror.setTitleBrush(Qt::black);
    axisYerror.setTitleFont(QFont("Serif", 10, -1, false));

    errSeries.setUseOpenGL(true);
    errSeries.setName("");
    errSeries.setPointsVisible(false);
    errSeries.setPen(QPen(Qt::black));
    chartError.addSeries(&errSeries);

    errDataPoints.setName(tr("err"));
    errDataPoints.setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    errDataPoints.setPen(QPen(Qt::black));
    errDataPoints.setBrush(QBrush(Qt::black));
    errDataPoints.setMarkerSize(10);
    chartError.addSeries(&errDataPoints);

    chartError.setAxisX(&axisXerror, &errSeries);
    chartError.setAxisY(&axisYerror, &errSeries);

    chartError.setAxisX(&axisXerror, &errDataPoints);
    chartError.setAxisY(&axisYerror, &errDataPoints);

    chartViewError = new QChartView(&chartError);

    */
}

/**
 * @brief ChartWindow::buildAUCPlot
 */
void ChartWindow::buildAUCPlot()
{

    /*
    chartArea.setTitleFont(mTitle);
    chartArea.setTitleBrush(Qt::black);

    chartArea.legend()->setFont(mLegendFont);
    chartArea.legend()->setAlignment(Qt::AlignBottom);
    chartArea.legend()->setLabelBrush(Qt::black);
    chartArea.legend()->setLabelColor(Qt::black);
    chartArea.legend()->setFont(QFont("Serif", 8, -1, false));

    axisXarea.setGridLineColor(Qt::transparent);
    axisXarea.setTitleText(tr("ln(Delay)"));
    axisXarea.setTitleBrush(Qt::black);
    axisXarea.setTitleFont(QFont("Serif", 10, -1, false));
    axisXarea.setMin(0);
    axisXarea.setLabelsFont(QFont("Serif", 10, -1, false));
    axisXarea.setLabelsBrush(Qt::black);
    axisXarea.setLabelsColor(Qt::black);
    axisXarea.setLabelFormat(QString("%.0f"));
    axisXarea.setLinePenColor(Qt::black);
    axisXarea.setLinePen(QPen(Qt::black));

    axisYarea.setGridLineColor(Qt::transparent);
    axisYarea.setTitleText(tr("Value"));
    axisYarea.setTitleBrush(Qt::black);
    axisYarea.setTitleFont(QFont("Serif", 10, -1, false));
    axisYarea.setTickCount(5);
    axisYarea.setLabelsFont(QFont("Serif", 10, -1, false));
    axisYarea.setLabelsBrush(Qt::black);
    axisYarea.setLabelsColor(Qt::black);
    axisYarea.setMin(0);
    axisYarea.setMax(1);
    axisYarea.setLinePenColor(Qt::black);
    axisYarea.setLinePen(QPen(Qt::black));

    expSeriesArea.setUseOpenGL(true);
    expSeriesArea.setName("Exponential");
    expSeriesArea.setPen(QPen(Qt::black));
    chartArea.addSeries(&expSeriesArea);

    hypSeriesArea.setUseOpenGL(true);
    hypSeriesArea.setName("Hyperbolic");
    hypSeriesArea.setPen(QPen(Qt::green));
    chartArea.addSeries(&hypSeriesArea);

    quasiSeriesArea.setUseOpenGL(true);
    quasiSeriesArea.setName("QuasiHyperbolic");
    quasiSeriesArea.setPen(QPen(Qt::blue));
    chartArea.addSeries(&quasiSeriesArea);

    myerSeriesArea.setUseOpenGL(true);
    myerSeriesArea.setName("Green-Myerson");
    myerSeriesArea.setPen(QPen(Qt::cyan));
    chartArea.addSeries(&myerSeriesArea);

    rachSeriesArea.setUseOpenGL(true);
    rachSeriesArea.setName("Rachlin");
    rachSeriesArea.setPen(QPen(Qt::magenta));
    chartArea.addSeries(&rachSeriesArea);

    rodriguezSeriesArea.setUseOpenGL(true);
    rodriguezSeriesArea.setName("Generalized-Hyperbolic");
    rodriguezSeriesArea.setPen(QPen(Qt::yellow));
    chartArea.addSeries(&rodriguezSeriesArea);

    ebertSeriesArea.setUseOpenGL(true);
    ebertSeriesArea.setName("Ebert-Prelec");
    ebertSeriesArea.setPen(QPen(Qt::red));
    chartArea.addSeries(&ebertSeriesArea);

    bleichrodtSeriesArea.setUseOpenGL(true);
    bleichrodtSeriesArea.setName("Bleichrodt");
    bleichrodtSeriesArea.setPen(QPen(Qt::darkCyan));
    chartArea.addSeries(&bleichrodtSeriesArea);

    noiseSeriesArea.setUseOpenGL(true);
    noiseSeriesArea.setName("Noise");
    noiseSeriesArea.setPen(QPen(Qt::darkGray));
    chartArea.addSeries(&noiseSeriesArea);

    empiricalSeries.setUseOpenGL(true);
    empiricalSeries.setName("AUC");
    empiricalSeries.setPen(QPen(Qt::black));
    chartArea.addSeries(&empiricalSeries);

    dataPointsArea.setName("Raw Data");
    dataPointsArea.setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    dataPointsArea.setPen(QPen(Qt::black));
    dataPointsArea.setBrush(QBrush(Qt::black));
    dataPointsArea.setMarkerSize(10);
    chartArea.addSeries(&dataPointsArea);

    chartArea.setAxisX(&axisXarea, &expSeriesArea);
    chartArea.setAxisY(&axisYarea, &expSeriesArea);

    chartArea.setAxisX(&axisXarea, &hypSeriesArea);
    chartArea.setAxisY(&axisYarea, &hypSeriesArea);

    chartArea.setAxisX(&axisXarea, &quasiSeriesArea);
    chartArea.setAxisY(&axisYarea, &quasiSeriesArea);

    chartArea.setAxisX(&axisXarea, &myerSeriesArea);
    chartArea.setAxisY(&axisYarea, &myerSeriesArea);

    chartArea.setAxisX(&axisXarea, &rachSeriesArea);
    chartArea.setAxisY(&axisYarea, &rachSeriesArea);

    chartArea.setAxisX(&axisXarea, &rodriguezSeriesArea);
    chartArea.setAxisY(&axisYarea, &rodriguezSeriesArea);

    chartArea.setAxisX(&axisXarea, &ebertSeriesArea);
    chartArea.setAxisY(&axisYarea, &ebertSeriesArea);

    chartArea.setAxisX(&axisXarea, &bleichrodtSeriesArea);
    chartArea.setAxisY(&axisYarea, &bleichrodtSeriesArea);

    chartArea.setAxisX(&axisXarea, &noiseSeriesArea);
    chartArea.setAxisY(&axisYarea, &noiseSeriesArea);

    chartArea.setAxisX(&axisXarea, &dataPointsArea);
    chartArea.setAxisY(&axisYarea, &dataPointsArea);

    chartArea.setAxisX(&axisXarea, &empiricalSeries);
    chartArea.setAxisY(&axisYarea, &empiricalSeries);

    */
}

/**
 * @brief ChartWindow::plotAUCSeries
 * @param index
 */
void ChartWindow::plotAUCSeries(int index)
{
    /*
    mList = mDisplayData.at(index);

    expSeriesArea.clear();
    hypSeriesArea.clear();
    quasiSeriesArea.clear();
    myerSeriesArea.clear();
    rachSeriesArea.clear();
    rodriguezSeriesArea.clear();
    ebertSeriesArea.clear();
    bleichrodtSeriesArea.clear();
    noiseSeriesArea.clear();
    dataPointsArea.clear();
    empiricalSeries.clear();

    if (mList.Header.contains("dropped", Qt::CaseInsensitive))
    {
        chartArea.setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    chartArea.setTitle(QString("Participant #%1: %2 Scaled AUC = %3").arg(QString::number(currentIndexShown + 1)).arg(cleanTitle(mList.TopModel)).arg(mList.TopAUCLog));

    expSeriesArea.hide();
    hypSeriesArea.hide();
    quasiSeriesArea.hide();
    myerSeriesArea.hide();
    rachSeriesArea.hide();
    rodriguezSeriesArea.hide();
    ebertSeriesArea.hide();
    bleichrodtSeriesArea.hide();
    noiseSeriesArea.hide();

    switch (mList.TopModelType)
    {
        case ModelType::Noise:
            noiseSeriesArea.show();
            break;

        case ModelType::Exponential:
            expSeriesArea.show();
            break;

        case ModelType::Hyperbolic:
            hypSeriesArea.show();
            break;

        case ModelType::BetaDelta:
            quasiSeriesArea.show();
            break;

        case ModelType::Myerson:
            myerSeriesArea.show();
            break;

        case ModelType::Rachlin:
            rachSeriesArea.show();
            break;

        case ModelType::GeneralizedHyperbolic:
            rodriguezSeriesArea.show();
            break;

        case ModelType::EbertPrelec:
            ebertSeriesArea.show();
            break;

        case ModelType::Beleichrodt:
            bleichrodtSeriesArea.show();
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

    int tickHack = ((int) log(lastDelay)) + 1;

    int negLogs = 3;

    axisXarea.setMin(-negLogs);
    axisXarea.setMax(tickHack + negLogs);
    axisXarea.setTickCount(tickHack + 1 + negLogs);

    for (double i = 0 - 3; i < (log(lastDelay)+1); i = i + chartIterator)
    {
        plotAUCPoint(i);
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

        dataPointsArea << QPointF(log(param1), param2);
        empiricalSeries << QPointF(log(param1), param2);
    }
    */
}

/**
 * @brief ChartWindow::plotAUCPoint
 * @param i
 */
void ChartWindow::plotAUCPoint(double i)
{
    /*
    xParam = exp(i);

    noiseSeriesArea << QPointF(i, noise);

    if (expCheck)
    {
        expSeriesArea << QPointF(i, exponential_plotting(expK, xParam));
    }

    if (hypCheck)
    {
        hypSeriesArea << QPointF(i, hyperbolic_plotting(hypK, xParam));
    }

    if (quasiCheck)
    {
         quasiSeriesArea << QPointF(i, quasi_hyperbolic_plotting(quasiB, quasiD, xParam));
    }

    if (myerCheck)
    {
        myerSeriesArea << QPointF(i, myerson_plotting(myerK, myerS, xParam));
    }

    if (rachCheck)
    {
        rachSeriesArea << QPointF(i, rachlin_plotting(rachK, rachS, xParam));
    }

    if (rodriguezCheck)
    {
        rodriguezSeriesArea << QPointF(i, generalized_hyperbolic_plotting(rodriguezK, rodriguezS, xParam));
    }

    if (ebertCheck)
    {
        ebertSeriesArea << QPointF(i, ebert_prelec_plotting(ebertK, ebertS, xParam));
    }

    if (bleichrodtCheck)
    {
        bleichrodtSeriesArea << QPointF(i, bleichrodt_plotting(bleichrodtK, bleichrodtS, bleichrodtBeta, xParam));
    }

    */
}

/**
 * @brief ChartWindow::plotProbabilities
 * @param index
 */
void ChartWindow::plotProbabilities(int index)
{
    /*
    mList = mDisplayData.at(index);

    if (mList.Header.contains("dropped", Qt::CaseInsensitive))
    {
        barChart.setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    barChart.setTitle(QString("Participant #%1: Model Probabilities").arg(QString::number(currentIndexShown + 1)));

    expCheck = hypCheck = quasiCheck = myerCheck = rachCheck = rodriguezCheck = ebertCheck = bleichrodtCheck = false;

    expProbSet.clear();
    hypProbSet.clear();
    quasiProbSet.clear();
    myerProbSet.clear();
    rachProbSet.clear();
    rodriguezProbSet.clear();
    ebertProbSet.clear();
    bleichrodtProbSet.clear();
    noiseProbSet.clear();

    expProbSet << QPointF(1,0);
    hypProbSet << QPointF(2,0);
    quasiProbSet << QPointF(3,0);
    myerProbSet << QPointF(4,0);
    rachProbSet << QPointF(5,0);
    rodriguezProbSet << QPointF(6,0);
    ebertProbSet << QPointF(7,0);
    bleichrodtProbSet << QPointF(8,0);
    noiseProbSet << QPointF(9,0);

    for (int i=0; i<mList.FittingResults.length(); i++)
    {

        if (mList.FittingResults[i]->Model == ModelType::Noise)
        {
            noiseProbSet << QPointF(9, mList.FittingResults[i]->Probability);
        }

        if (mList.FittingResults[i]->Model == ModelType::Exponential)
        {
            expK = mList.FittingResults[i]->Params.first().second;

            if (expK != NULL)
            {
                expProbSet << QPointF(1, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Hyperbolic)
        {
            hypK = mList.FittingResults[i]->Params.first().second;

            if (hypK != NULL)
            {
                hypProbSet << QPointF(2, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::BetaDelta)
        {
            quasiB = mList.FittingResults[i]->Params.first().second;
            quasiD = mList.FittingResults[i]->Params.last().second;

            if (quasiB != NULL && quasiD != NULL)
            {
                quasiProbSet << QPointF(3, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Myerson)
        {
            myerK = mList.FittingResults[i]->Params.first().second;
            myerS = mList.FittingResults[i]->Params.last().second;

            if (myerK != NULL && myerS != NULL)
            {
                myerProbSet << QPointF(4, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::Rachlin)
        {
            rachK = mList.FittingResults[i]->Params.first().second;
            rachS = mList.FittingResults[i]->Params.last().second;

            if (rachK != NULL && rachS != NULL)
            {
                rachProbSet << QPointF(5, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::GeneralizedHyperbolic)
        {
            rodriguezK = mList.FittingResults[i]->Params.first().second;
            rodriguezS = mList.FittingResults[i]->Params.last().second;

            if (rodriguezK != NULL && rodriguezS != NULL)
            {
                rodriguezProbSet << QPointF(6, mList.FittingResults[i]->Probability);
            }
        }

        if (mList.FittingResults[i]->Model == ModelType::EbertPrelec)
        {
            ebertK = mList.FittingResults[i]->Params.first().second;
            ebertS = mList.FittingResults[i]->Params.last().second;

            if (ebertK != NULL && ebertS != NULL)
            {
                ebertProbSet << QPointF(7, mList.FittingResults[i]->Probability);
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
                bleichrodtProbSet << QPointF(8, mList.FittingResults[i]->Probability);
            }
        }
    }
    */
}

/**
 * @brief ChartWindow::plotResiduals
 * @param index
 */
void ChartWindow::plotResiduals(int index)
{
    /*
    mList = mDisplayData.at(index);

    errSeries.clear();
    errDataPoints.clear();

    errSeries << QPointF(0, 0);

    for (int j=0; j<mList.TopErrPar.length(); j++)
    {
        errSeries << QPointF(j+1, 0);
        errDataPoints << QPointF(j+1, mList.TopErrPar[j]);
    }

    if (mList.TopErrPar.length() > 0)
    {
        minList = std::abs(*std::min_element(mList.TopErrPar.begin(), mList.TopErrPar.end())) * 1.5;
        maxList = std::abs(*std::max_element(mList.TopErrPar.begin(), mList.TopErrPar.end())) * 1.5;

        axisYerror.setMin((maxList >= minList) ? -maxList : -minList);
        axisYerror.setMax((maxList >= minList) ? maxList : minList);
        axisYerror.setTickCount(9);
    }

    axisXerror.setMin(0);
    axisXerror.setMax(mList.TopErrPar.length());
    axisXerror.setTickCount(mList.TopErrPar.count() + 1);

    if (mList.Header.contains("dropped", Qt::CaseInsensitive))
    {
        chartError.setTitle(QString("Participant #%1: Dropped").arg(QString::number(currentIndexShown + 1)));

        return;
    }

    chartError.setTitle(QString("Participant #%1: %2 Residual Plot").arg(QString::number(currentIndexShown + 1)).arg(cleanTitle(mList.TopModel)));
    */
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
    return beta * pow(delta, x);
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
void ChartWindow::saveSVGasPNG()
{
    QString file_name;
    QString fileFilter = "PNG (*.png)";

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, tr("Save PNG"), QDir::homePath(),
                                         fileFilter);

#elif TARGET_OS_MAC

        file_name = QFileDialog::getSaveFileName(this, tr("Save PNG"), QDir::homePath(),
                                         fileFilter, &fileFilter, QFileDialog::Option::DontUseNativeDialog);

        if (!file_name.contains(".png"))
        {
            file_name.append(".png");
        }

#endif

    if(!file_name.trimmed().isEmpty())
    {
        //stackedWidget.currentWidget()->grab().save(file_name, "PNG", 9);
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
