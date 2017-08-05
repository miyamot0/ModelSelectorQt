#ifndef CALCULATIONSETTINGS_H
#define CALCULATIONSETTINGS_H

#include <QObject>

class CalculationSettings
{
public:
    CalculationSettings();

    QString scriptName;

    int topDelay, leftDelay, bottomDelay, rightDelay;
    int topValue, leftValue, bottomValue, rightValue;

    double maxValue;

    bool modelExponential,
         modelHyperbolic,
         modelQuasiHyperbolic,
         modelMyersonGreen,
         modelRachlin,
         modelRodriguezLogue,
         modelEbertPrelec,
         modelBleichrodt;

    bool cbRachlin, johnsonBickelTest, showCharts, logNormalParameters;

};

#endif // CALCULATIONSETTINGS_H
