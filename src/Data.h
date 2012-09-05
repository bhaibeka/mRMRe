#ifndef mRMRe_Data_h
#define mRMRe_Data_h

#include <limits>

#include "Math.h"
#include "Matrix.h"

class Data
{
private:
    Data(const Data&);

    Data&
    operator=(const Data&);

    Matrix const* const mpDataMatrix;
    Matrix* const mpOrderMatrix;
    Matrix const* const mpPriorsMatrix;
    bool* const mpHasOrderCached;
    unsigned int const* const mpSampleStrata;
    float const* const mpSampleWeights;
    unsigned int const* const mpFeatureTypes;
    unsigned int const mSampleStratumCount;
    unsigned int** const mpSampleIndicesPerStratum;
    unsigned int* const mpSampleCountPerStratum;
    bool const mUsesRanks;
    bool const mOutX;
    unsigned int const mBootstrapCount;
    float const mPriorsWeight;

public:
    static unsigned int const FEATURE_CONTINUOUS = 0;
    static unsigned int const FEATURE_DISCRETE = 1;
    static unsigned int const FEATURE_SURVIVAL_EVENT = 2;
    static unsigned int const FEATURE_SURVIVAL_TIME = 3;

    Data(float* const pData, Matrix const* const pPriorsMatrix, float const priorsWeight,
            unsigned int const sampleCount, unsigned int const featureCount,
            unsigned int const* const pSampleStrata, float const* const pSampleWeights,
            unsigned int const* const pFeatureTypes, unsigned int const sampleStratumCount,
            bool const usesRanks, bool const outX, unsigned int const bootstrapCount);

    ~Data();

    float const
    computeMiBetweenFeatures(unsigned int const i, unsigned int const j) const;

    float const
    computeCorrelationBetweenContinuousFeatures(unsigned int const i, unsigned int const j) const;

    unsigned int const
    getSampleCount() const;

    unsigned int const
    getFeatureCount() const;
};

#endif /* mRMRe_Data_h */
