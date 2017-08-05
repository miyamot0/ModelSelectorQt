#include "fitresult.h"

FitResult::FitResult(ModelType mModel)
{
    Model = mModel;

    BIC = -1;
    AIC = -1;
    BF = -1;
    Probability = -1;

    Rank = -1;
}
