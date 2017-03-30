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

#include <QtWidgets>
#include <QSvgRenderer>
#include <QPixmap>
#include "graphicaloutputdialog.h"
#include "ui_graphicaloutputdialog.h"

GraphicalOutputDialog::GraphicalOutputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphicalOutputDialog)
{
    ui->setupUi(this);

    mSVG = new QSvgWidget();
    ui->verticalLayout->addWidget(mSVG);

    currentIndexShown = 0;
}

GraphicalOutputDialog::~GraphicalOutputDialog()
{
    delete ui;
}

void GraphicalOutputDialog::on_NextButton_clicked()
{
    if (currentIndexShown >= mDisplayData.count() - 1)
    {
        return;
    }

    currentIndexShown++;

    displayPlot();
}

void GraphicalOutputDialog::on_PreviousButton_clicked()
{
    if (currentIndexShown <= 0)
    {
        return;
    }

    currentIndexShown--;

    displayPlot();
}

void GraphicalOutputDialog::appendBase64(QString chartData)
{
    QFile file("tempFileNew.tmp");
    file.open(QIODevice::WriteOnly);
    file.write(QByteArray::fromBase64(chartData.toUtf8()));
    file.close();

    QFile infile("tempFileNew.tmp");
    infile.open(QFile::ReadOnly);

    QFile outfile ("tempFileOut.tmp");
    outfile.open(QFile::WriteOnly | QFile::Truncate);

    QTextStream in(&infile);
    QTextStream out(&outfile);

    QRegExp rx1("<symbol");
    QRegExp rx2("</symbol");

    while (!in.atEnd()) {
        QString line = in.readLine();
        line.replace(rx1, "<g"); // so '<symbol' becomes '<g ...'
        line.replace(rx2, "</g");// and '</symbol becomes '</g'
        out << line << "\n";
    }

    infile.close();
    outfile.close();

    QFile readFile ("tempFileOut.tmp");
    readFile.open(QFile::ReadOnly);

    mDisplayData.append(readFile.readAll().toBase64());

    readFile.close();

    if (mDisplayData.count() == 1)
    {
        displayPlot();
    }

    ui->currentItem->setText("Showing #" + QString::number(currentIndexShown + 1) + " of " + QString::number(mDisplayData.count()) + " plots");
}

void GraphicalOutputDialog::displayPlot()
{
    chartString = mDisplayData.at(currentIndexShown);

    mSVG->load(QByteArray::fromBase64(chartString.toUtf8()));

    ui->currentItem->setText("Showing #" + QString::number(currentIndexShown + 1) + " of " + QString::number(mDisplayData.count()) + " plots");
}
