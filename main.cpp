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

#include <QApplication>
#include <QDir>

#include "sheetwidget.h"
#include "fitresults.h"

#ifdef _WIN32
    #include <QSettings>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qRegisterMetaType<FitResults>("FitResults");

    SheetWidget mNewSheet;
    mNewSheet.setWindowIcon(QPixmap(":/images/x-office-spreadsheet.png"));
    mNewSheet.show();

    return app.exec();
}
