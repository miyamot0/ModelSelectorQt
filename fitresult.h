#ifndef FITRESULT_H
#define FITRESULT_H

#include <QObject>

enum class ModelType {
    Noise,
    Exponential,
    Hyperbolic,
    RodriguezLogue,
    BetaDelta,
    Myerson,
    Rachlin,
    EbertPrelec,
    Beleichrodt
};

class FitResult
{
public:
    FitResult(ModelType mModel);

    ModelType Model;

    QList<QPair<QString, double>> Params;

    double RMS;
    double BIC;
    double AIC;
    double BF;
    double Probability;

    int Rank;

    QString Status;
};

#endif // FITRESULT_H
