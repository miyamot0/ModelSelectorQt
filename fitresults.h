#ifndef FITRESULTS_H
#define FITRESULTS_H

#include <QObject>

#include "fitresult.h"

class FitResults
{
public:
    FitResults();

    QString Header;
    QString TopModel;
    QString ParticipantValues;
    QString ParticipantDelays;

    QString TopED50;
    QString TopAUC;
    QString TopAUCLog;

    QList<QStringList> StoredValues;

    QList<FitResult*> FittingResults;

};

#endif // FITRESULTS_H
