#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include "chartwindow.h"

ChartWindow::ChartWindow(QList<QStringList> stringList, bool isAUC, QWidget *parent)
{
    mDisplayData = stringList;
    isAUCFigure = isAUC;

    for (int i=0; i<mDisplayData.length(); i++)
    {
        mList = mDisplayData[0];

        for (int j=0; j<mList.length(); j++)
        {
            paramString1 = mList.at(46);
            QStringList delayPoints = paramString1.split(",");
            lastDelay = delayPoints[delayPoints.length() - 1].toDouble();
        }
    }

    QVBoxLayout *windowLayout = new QVBoxLayout;

    currentIndexShown = 0;

    installEventFilter(this);

    chart = new QChart();

    QFont mTitle("Serif", 14, -1, false);
    chart->setTitleFont(mTitle);

    axisX = new QLogValueAxis;
    axisX->setGridLineColor(Qt::transparent);
    axisX->setTitleText("Delay");
    axisX->setBase(10);
    axisX->setLinePenColor(Qt::black);
    axisX->setLinePen(QPen(Qt::black));

    axisY = new QValueAxis;
    axisY->setGridLineColor(Qt::transparent);
    axisY->setTitleText("Value");
    axisY->setTickCount(5);
    axisY->setMin(0);
    axisY->setMax(1);
    axisY->setLinePenColor(Qt::black);
    axisY->setLinePen(QPen(Qt::black));

    if (isAUCFigure)
    {
        buildAUCPlot();
    }
    else
    {
        buildED50Plot();
    }

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    windowLayout->addWidget(chartView);

    // Set layout in QWidget
    QWidget *window = new QWidget(parent);
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

    windowLayout->setMenuBar(tb);
    resize(500, 500);
    setCentralWidget(window);

    setWindowFlags(Qt::WindowStaysOnTopHint);
}

void ChartWindow::buildAUCPlot()
{
    expSeries = new QLineSeries();
    expSeries->setName("Exponential");
    chart->addSeries(expSeries);

    hypSeries = new QLineSeries();
    hypSeries->setName("Hyperbolic");
    chart->addSeries(hypSeries);

    quasiSeries = new QLineSeries();
    quasiSeries->setName("Beta Delta");
    chart->addSeries(quasiSeries);

    myerSeries = new QLineSeries();
    myerSeries->setName("Myerson");
    chart->addSeries(myerSeries);

    rachSeries = new QLineSeries();
    rachSeries->setName("Rachlin");
    chart->addSeries(rachSeries);

    noiseSeries = new QLineSeries();
    noiseSeries->setName("Noise");
    noiseSeries->setPen(QPen(Qt::black));
    chart->addSeries(noiseSeries);

    empiricalSeries = new QLineSeries();
    empiricalSeries->setName("AUC");
    empiricalSeries->setPen(QPen(Qt::black));
    chart->addSeries(empiricalSeries);

    dataPoints = new QScatterSeries();
    dataPoints->setName("Raw Data");
    dataPoints->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    dataPoints->setPen(QPen(Qt::black));
    dataPoints->setBrush(QBrush(Qt::black));
    dataPoints->setMarkerSize(10);
    chart->addSeries(dataPoints);

    plotAUCSeries(0);

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

    chart->setAxisX(axisX, noiseSeries);
    chart->setAxisY(axisY, noiseSeries);

    chart->setAxisX(axisX, dataPoints);
    chart->setAxisY(axisY, dataPoints);

    chart->setAxisX(axisX, empiricalSeries);
    chart->setAxisY(axisY, empiricalSeries);
}

void ChartWindow::buildED50Plot()
{
    expSeries = new QLineSeries();
    expSeries->setName("Exponential");
    chart->addSeries(expSeries);

    hypSeries = new QLineSeries();
    hypSeries->setName("Hyperbolic");
    chart->addSeries(hypSeries);

    quasiSeries = new QLineSeries();
    quasiSeries->setName("Beta Delta");
    chart->addSeries(quasiSeries);

    myerSeries = new QLineSeries();
    myerSeries->setName("Myerson");
    chart->addSeries(myerSeries);

    rachSeries = new QLineSeries();
    rachSeries->setName("Rachlin");
    chart->addSeries(rachSeries);

    noiseSeries = new QLineSeries();
    noiseSeries->setName("Noise");
    noiseSeries->setPen(QPen(Qt::black));
    chart->addSeries(noiseSeries);

    dataPoints = new QScatterSeries();
    dataPoints->setName("Raw Data");
    dataPoints->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    dataPoints->setPen(QPen(Qt::black));
    dataPoints->setBrush(QBrush(Qt::black));
    dataPoints->setMarkerSize(10);
    chart->addSeries(dataPoints);

    plotED50Series(0);

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

    chart->setAxisX(axisX, noiseSeries);
    chart->setAxisY(axisY, noiseSeries);

    chart->setAxisX(axisX, dataPoints);
    chart->setAxisY(axisY, dataPoints);
}

void ChartWindow::plotAUCSeries(int index)
{
    mList = mDisplayData.at(index);

    QString title;
    title.append("Participant #");
    title.append(QString::number(currentIndexShown + 1));
    title.append(" (");
    title.append(mList[45]);
    title.append(")");
    chart->setTitle(title);

    expSeries->clear();
    hypSeries->clear();
    quasiSeries->clear();
    myerSeries->clear();
    rachSeries->clear();
    noiseSeries->clear();
    dataPoints->clear();
    empiricalSeries->clear();

    expSeries->hide();
    hypSeries->hide();
    quasiSeries->hide();
    myerSeries->hide();
    rachSeries->hide();
    noiseSeries->hide();

    if (mList[45].contains("Noise"))
    {
        noiseSeries->show();
    }
    else if (mList[45].contains("Exponential"))
    {
        expSeries->show();
    }
    else if (mList[45].contains("Hyperbolic"))
    {
        hypSeries->show();
    }
    else if (mList[45].contains("Beta Delta"))
    {
        quasiSeries->show();
    }
    else if (mList[45].contains("Myerson"))
    {
        myerSeries->show();
    }
    else if (mList[45].contains("Rachlin"))
    {
        rachSeries->show();
    }

    expK = mList[8].toDouble(&expCheck);

    hypCheck;
    hypK = mList[1].toDouble(&hypCheck);

    quasiCheck;
    quasiB = mList[15].toDouble(&quasiCheck);
    quasiD = mList[16].toDouble(&quasiCheck);

    myerCheck;
    myerK = mList[23].toDouble(&myerCheck);
    myerS = mList[24].toDouble(&myerCheck);

    rachCheck;
    rachK = mList[31].toDouble(&rachCheck);
    rachS = mList[32].toDouble(&rachCheck);

    noise = mList[39].toDouble();

    for (int i = 1; i < int(lastDelay); i++)
    {
        xParam = double(i);

        *noiseSeries << QPointF(xParam, noise);

        if (expCheck)
        {
            *expSeries << QPointF(xParam, exponential_plotting(expK, xParam));
        }

        if (hypCheck)
        {
            *hypSeries << QPointF(xParam, hyperbolic_plotting(hypK, xParam));
        }

        if (quasiCheck)
        {
             *quasiSeries << QPointF(xParam, quasi_hyperbolic_plotting(quasiB, quasiD, xParam));
        }

        if (myerCheck)
        {
            *myerSeries << QPointF(xParam, myerson_plotting(myerK, myerS, xParam));
        }

        if (rachCheck)
        {
            *rachSeries << QPointF(xParam, rachlin_plotting(rachK, rachS, xParam));
        }

        if (i > 10 && i < 100)
        {
            i += 9;
        }
        else if (i > 100 && i < 1000)
        {
            i += 99;
        }
        else if (i > 1000 && i < 10000)
        {
            i += 999;
        }
        else if (i > 10000)
        {
            i += 9999;
        }
    }

    paramString1 = mList.at(46);
    paramString2 = mList.at(47);

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
        *dataPoints << QPointF(param1, param2);
        *empiricalSeries << QPointF(param1, param2);
    }
}

void ChartWindow::plotED50Series(int index)
{
    mList = mDisplayData.at(index);

    QString title;
    title.append("Participant #");
    title.append(QString::number(currentIndexShown + 1));
    title.append(" (");
    title.append(mList[45]);
    title.append(")");
    chart->setTitle(title);

    expSeries->clear();
    hypSeries->clear();
    quasiSeries->clear();
    myerSeries->clear();
    rachSeries->clear();
    noiseSeries->clear();
    dataPoints->clear();

    expK = mList[8].toDouble(&expCheck);

    hypCheck;
    hypK = mList[1].toDouble(&hypCheck);

    quasiCheck;
    quasiB = mList[15].toDouble(&quasiCheck);
    quasiD = mList[16].toDouble(&quasiCheck);

    myerCheck;
    myerK = mList[23].toDouble(&myerCheck);
    myerS = mList[24].toDouble(&myerCheck);

    rachCheck;
    rachK = mList[31].toDouble(&rachCheck);
    rachS = mList[32].toDouble(&rachCheck);

    noise = mList[39].toDouble();

    for (int i = 1; i < int(lastDelay); i++)
    {
        xParam = double(i);

        *noiseSeries << QPointF(xParam, noise);

        if (expCheck)
        {
            *expSeries << QPointF(xParam, exponential_plotting(expK, xParam));
        }

        if (hypCheck)
        {
            *hypSeries << QPointF(xParam, hyperbolic_plotting(hypK, xParam));
        }

        if (quasiCheck)
        {
             *quasiSeries << QPointF(xParam, quasi_hyperbolic_plotting(quasiB, quasiD, xParam));
        }

        if (myerCheck)
        {
            *myerSeries << QPointF(xParam, myerson_plotting(myerK, myerS, xParam));
        }

        if (rachCheck)
        {
            *rachSeries << QPointF(xParam, rachlin_plotting(rachK, rachS, xParam));
        }

        if (i > 10 && i < 100)
        {
            i += 9;
        }
        else if (i > 100 && i < 1000)
        {
            i += 99;
        }
        else if (i > 1000 && i < 10000)
        {
            i += 999;
        }
        else if (i > 10000)
        {
            i += 9999;
        }
    }

    paramString1 = mList.at(46);
    paramString2 = mList.at(47);

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
        *dataPoints << QPointF(param1, param2);
    }
}

double ChartWindow::exponential_plotting(double k, double x)
{
    return exp(-exp(k)*x);
}

double ChartWindow::hyperbolic_plotting(double k, double x)
{
    return pow((1+exp(k)*x), -1);
}

double ChartWindow::quasi_hyperbolic_plotting(double beta, double delta, double x)
{
    return beta * pow(delta, x);
}

double ChartWindow::myerson_plotting(double k, double s, double x)
{
    return pow((1+exp(k)*x), -s);
}

double ChartWindow::rachlin_plotting(double k, double s, double x)
{
    return pow((1+exp(k)*pow(x, s)), -1);
}

bool ChartWindow::eventFilter(QObject *object, QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        //QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        //qDebug() << keyEvent;
        //QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        //std::cout << event->key() << "\n";
    }
    return false;
}

void ChartWindow::saveSVGasPNG()
{
    QString file_name;
    QString fileFilter = "PNG (*.png)";
    /*

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, "Open spreadsheet file", QDir::homePath(),
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
        QSvgRenderer *renderer = mSVG->renderer();
        QImage image(600, 600, QImage::Format_RGB32);
        QPainter painter;
        painter.begin(&image);
        renderer->render(&painter);
        painter.end();
        image.save(file_name, "PNG", 9);
    }
        */

}

void ChartWindow::on_NextButton_clicked()
{
    if (currentIndexShown >= mDisplayData.count() - 1)
    {
        return;
    }

    currentIndexShown++;

    if (isAUCFigure)
    {
        plotAUCSeries(currentIndexShown);
    }
    else
    {
        plotED50Series(currentIndexShown);
    }
}

void ChartWindow::on_PreviousButton_clicked()
{
    if (currentIndexShown <= 0)
    {
        return;
    }

    currentIndexShown--;

    if (isAUCFigure)
    {
        plotAUCSeries(currentIndexShown);
    }
    else
    {
        plotED50Series(currentIndexShown);
    }
}
