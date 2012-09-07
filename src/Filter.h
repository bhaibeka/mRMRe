#ifndef mRMRe_Filter_h
#define mRMRe_Filter_h

#include <cmath>
#include <limits>
#include <omp.h>
#include <vector>

#include "Math.h"
#include "Matrix.h"

class Filter
{
private:
    Filter(const Filter&);

    Filter&
    operator=(const Filter&);

    unsigned int const* const mpChildrenCountPerLevel;
    unsigned int const mLevelCount;
    Matrix* const mpFeatureInformationMatrix;
    unsigned int* const mpStartingIndexPerLevel;
    unsigned int* mpIndexTree;
    unsigned int mTreeElementCount;

public:
    Filter(unsigned int const* const pChildrenCountPerLevel, unsigned int const levelCount,
            Matrix* const pFeatureInformationMatrix, unsigned int const targetFeatureIndex);

    ~Filter();

    void const
    build();

    inline unsigned int const
    getParentAbsoluteIndex(unsigned int const absoluteIndex, unsigned int const level) const;

    bool const
    hasAncestorByFeatureIndex(unsigned int const absoluteIndex, unsigned int const featureIndex,
            unsigned int level) const;

    bool const
    hasSamePath(unsigned int const absoluteIndex1, unsigned int const absoluteIndex2,
            unsigned int const level) const;

    bool const
    isRedundantPath(unsigned int const absoluteIndex, unsigned int const featureIndex,
            unsigned int const level) const;

    operator std::vector<unsigned int>() const;

    void const
    placeElements(unsigned int const startingIndex, unsigned int childrenCount,
            unsigned int const level);
};

#endif /* mRMRe_Filter_h */