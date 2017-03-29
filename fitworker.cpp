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

   The FitWorker class was based on earlier work by Fabien Pierre-Nicolas; its license is presented below

   =======================================================================================================

   Copyright 2013 Fabien Pierre-Nicolas.
      - Primarily authored by Fabien Pierre-Nicolas

   This file is part of simple-qt-thread-example, a simple example to demonstrate how to use threads.

   This example is explained on http://fabienpn.wordpress.com/qt-thread-simple-and-stable-with-sources/

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This progra is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

  */

#include <QtWidgets>
#include <QProcess>

#include "fitworker.h"

FitWorker::FitWorker(QString mCommand, QStringList mCommandArgs, bool boundRachlin)
{
    command = mCommand;
    commandParameterList = mCommandArgs;
    boundRachlinS = boundRachlin;
}

void FitWorker::startWork()
{
    emit workStarted();
}

void FitWorker::working()
{
    bool rachlinNotation = false;

    QProcess process;
    QByteArray output;
    QByteArray error;

    QStringList mArgList;

    QJsonParseError err;
    QJsonDocument jsonDoc;
    QJsonArray jsonArr;
    QJsonValue jsonVal;
    QJsonObject jsonObj;

    for (int i=0; i<commandParameterList.count(); i++)
    {
        rachlinNotation = false;

        mArgList.clear();
        mArgList << command;
        mArgList << commandParameterList.at(i);

        process.start(mArgList.join(" "));
        process.waitForFinished(-1);

        output = process.readAllStandardOutput();
        error = process.readAllStandardError();

        QString mOutput(output);
        jsonDoc = QJsonDocument::fromJson(mOutput.toUtf8(), &err);
        jsonArr = jsonDoc.array();
        jsonVal = jsonArr.at(0);
        jsonObj = jsonVal.toObject();

        if (boundRachlinS && jsonObj["Rachlin.s"].toDouble() >= 1.0)
        {
            rachlinNotation = true;
            QString mListString1 = commandParameterList.at(i);
            QStringList mList1   = mListString1.split(" ");
            QString scriptString = mList1.at(0);
            QString delayString = mList1.at(1);
            QString valueString = mList1.at(2);
            QString mListString2 = mList1.at(3);
            QStringList mList2   = mListString2.split(",");
            mList2[3] = "0";

            mArgList.clear();
            mArgList << command;
            mArgList << scriptString;
            mArgList << delayString;
            mArgList << valueString;
            mArgList << mList2.join(",");

            process.start(mArgList.join(" "));
            process.waitForFinished();

            output = process.readAllStandardOutput();
            error = process.readAllStandardError();

            QString mOutput(output);
            jsonDoc = QJsonDocument::fromJson(mOutput.toUtf8(), &err);
            jsonArr = jsonDoc.array();
            jsonVal = jsonArr.at(0);
            jsonObj = jsonVal.toObject();
        }

        QStringList resultsList;

        resultsList << QString::number(i+1);
        resultsList << QString::number(jsonObj["Mazur.lnk"].toDouble());
        resultsList << "";
        resultsList << QString::number(jsonObj["Mazur.RMSE"].toDouble());
        resultsList << QString::number(jsonObj["Mazur.BIC"].toDouble());
        resultsList << QString::number(jsonObj["Mazur.AIC"].toDouble());
        resultsList << QString::number(jsonObj["Mazur.BF"].toDouble());
        resultsList << QString::number(jsonObj["Mazur.prob"].toDouble());

        resultsList << QString::number(jsonObj["exp.lnk"].toDouble());
        resultsList << "";
        resultsList << QString::number(jsonObj["exp.RMSE"].toDouble());
        resultsList << QString::number(jsonObj["exp.BIC"].toDouble());
        resultsList << QString::number(jsonObj["exp.AIC"].toDouble());
        resultsList << QString::number(jsonObj["exp.BF"].toDouble());
        resultsList << QString::number(jsonObj["exp.prob"].toDouble());

        resultsList << QString::number(jsonObj["BD.beta"].toDouble());
        resultsList << QString::number(jsonObj["BD.delta"].toDouble());
        resultsList << QString::number(jsonObj["BD.RMSE"].toDouble());
        resultsList << QString::number(jsonObj["BD.BIC"].toDouble());
        resultsList << QString::number(jsonObj["BD.AIC"].toDouble());
        resultsList << QString::number(jsonObj["BD.BF"].toDouble());
        resultsList << QString::number(jsonObj["BD.prob"].toDouble());

        resultsList << QString::number(jsonObj["MG.lnk"].toDouble());
        resultsList << QString::number(jsonObj["MG.s"].toDouble());
        resultsList << QString::number(jsonObj["MG.RMSE"].toDouble());
        resultsList << QString::number(jsonObj["MG.BIC"].toDouble());
        resultsList << QString::number(jsonObj["MG.AIC"].toDouble());
        resultsList << QString::number(jsonObj["MG.BF"].toDouble());
        resultsList << QString::number(jsonObj["MG.prob"].toDouble());

        if (rachlinNotation)
        {
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
            resultsList << "Exceeded Bounds";
        }
        else
        {
            resultsList << QString::number(jsonObj["Rachlin.lnk"].toDouble());
            resultsList << QString::number(jsonObj["Rachlin.s"].toDouble());
            resultsList << QString::number(jsonObj["Rachlin.RMSE"].toDouble());
            resultsList << QString::number(jsonObj["Rachlin.BIC"].toDouble());
            resultsList << QString::number(jsonObj["Rachlin.AIC"].toDouble());
            resultsList << QString::number(jsonObj["Rachlin.BF"].toDouble());
            resultsList << QString::number(jsonObj["Rachlin.prob"].toDouble());
        }

        resultsList << QString::number(jsonObj["noise.mean"].toDouble());
        resultsList << "";
        resultsList << QString::number(jsonObj["noise.RMSE"].toDouble());
        resultsList << QString::number(jsonObj["noise.BIC"].toDouble());
        resultsList << QString::number(jsonObj["noise.AIC"].toDouble());
        resultsList << QString::number(jsonObj["noise.BF"].toDouble());
        resultsList << QString::number(jsonObj["noise.prob"].toDouble());

        resultsList << jsonObj["prob.frame.probmodel"].toString();
        resultsList << QString::number(jsonObj["lnED50.mostprob"].toDouble());
        resultsList << jsonObj["chart"].toString();

        emit workingResult(resultsList);
    }

    emit workFinished();
}
