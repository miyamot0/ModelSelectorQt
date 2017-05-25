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

#ifndef GRAPHICALOUTPUTDIALOG_H
#define GRAPHICALOUTPUTDIALOG_H

#include <QMenu>
#include <QMenuBar>
#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QtCharts>

#include "modelselection.h"

namespace Ui {
class GraphicalOutputDialog;
}

class GraphicalOutputDialog : public QDialog
{
    Q_OBJECT

public:
    /** Widget for displaying received plots
     * @brief GraphicalOutputDialog
     * @param parent
     */
    explicit GraphicalOutputDialog(QList<QStringList> mData, QWidget *parent = 0);

    /** Method for adding base64 strings to list
     * @brief appendBase64
     * @param chartData - Base data
     */
    void appendBase64(QString chartData);
    void displayChart();
    void plotSeries(QString model);

    double exponential_plotting(double k, double x);

    bool eventFilter(QObject *object, QEvent *e);

    QStringList mDisplayData;

    ~GraphicalOutputDialog();

private slots:
    void on_NextButton_clicked();
    void on_PreviousButton_clicked();
    void displayPlot();
    //void plotSeries(QString model);

    void contextMenuRequested(const QPoint& point);

    void saveSVGasPNG();

    void on_pushButton_clicked();

private:
    Ui::GraphicalOutputDialog *ui;

    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;

    QString chartString;

    QImage img;

    int currentIndexShown;

    QMenu *dialogMenu;
    QAction *savePNG;
};

#endif // GRAPHICALOUTPUTDIALOG_H
