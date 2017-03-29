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

#include <QProcess>
#include <QDesktopWidget>
#include "statusdialog.h"
#include "ui_statusdialog.h"

StatusDialog::StatusDialog(bool rInstalled, QString commandParameter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatusDialog)
{
    ui->setupUi(this);

    this->window()->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            window()->size(),
            qApp->desktop()->availableGeometry()
        )
    );

    orderVar = 0;

    if (!rInstalled)
    {
        ui->statusR->setText("R was not found!");
    }
    else
    {
        ui->statusR->setText("R found!");
        ui->statusR->setStyleSheet("QLabel { color : green; }");
    }

    QStringList mPackageInstall;
    mPackageInstall << "installDependencyReshape.R";
    mPackageInstall << "installDependencyBase64.R";
    mPackageInstall << "installDependencyJsonlite.R";

    thread = new QThread();
    rWorker = new Rworker(commandParameter, mPackageInstall);

    rWorker->moveToThread(thread);

    connect(rWorker, SIGNAL(workStarted()), thread, SLOT(start()));
    connect(thread, SIGNAL(started()), rWorker, SLOT(working()));
    connect(rWorker, SIGNAL(workingResult(QString)), this, SLOT(WorkUpdate(QString)));
    connect(rWorker, SIGNAL(workFinished()), thread, SLOT(quit()), Qt::DirectConnection);

    thread->wait();
    rWorker->startWork();
}

void StatusDialog::WorkUpdate(QString status)
{
    bool isTrue = status.contains("TRUE");

    if (orderVar == 0)
    {
        if (!isTrue)
        {
            ui->statusReshape->setText("reshape could not be installed!");
        }
        else
        {
            ui->statusReshape->setText("reshape installed!");
            ui->statusReshape->setStyleSheet("QLabel { color : green; }");
        }
    }
    else if (orderVar == 1)
    {
        if (!isTrue)
        {
            ui->statusBase64->setText("base64enc could not be installed!");
        }
        else
        {
            ui->statusBase64->setText("base64enc installed!");
            ui->statusBase64->setStyleSheet("QLabel { color : green; }");
        }
    }
    else if (orderVar == 2)
    {
        if (!isTrue)
        {
            ui->statusJson->setText("jsonlite could not be installed!");
        }
        else
        {
            ui->statusJson->setText("jsonlite installed!");
            ui->statusJson->setStyleSheet("QLabel { color : green; }");
        }
    }

    orderVar++;
}

StatusDialog::~StatusDialog()
{    
    thread->wait();

    delete thread;
    delete rWorker;

    delete ui;
}
