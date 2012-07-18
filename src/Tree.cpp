#include "Tree.hpp"

Tree::Tree(unsigned int* const pChildrenCountPerLevel, unsigned int const levelCount,
        float* const pFeatureInformationMatrix, unsigned int const featureCount,
        unsigned int const targetFeatureIndex) :
        mpChildrenCountPerLevel(pChildrenCountPerLevel), mLevelCount(levelCount), mpFeatureInformationMatrix(
                pFeatureInformationMatrix), mFeatureCount(featureCount)
{
    unsigned int cumulative_element_count = 1;
    unsigned int children_per_level = 1;

    mpStartingIndexPerLevel = new unsigned int[mLevelCount + 1];
    mpStartingIndexPerLevel[0] = 0;

    for (unsigned int level = 0; level < mLevelCount; ++level)
    {
        mpStartingIndexPerLevel[level + 1] = cumulative_element_count;
        children_per_level *= mpChildrenCountPerLevel[level];
        cumulative_element_count += children_per_level;
    }

    mpIndexTree = new unsigned int[cumulative_element_count];
    mpInformativeContributionTree = new float[cumulative_element_count];
    mpRedundantContributionTree = new float[cumulative_element_count];
    mTreeElementCount = cumulative_element_count;

    mpIndexTree[0] = targetFeatureIndex;
    mpInformativeContributionTree[0] = 0.;
    mpRedundantContributionTree[0] = 0.;
}

Tree::~Tree()
{
    delete[] mpStartingIndexPerLevel;
    delete[] mpIndexTree;
    delete[] mpInformativeContributionTree;
    delete[] mpRedundantContributionTree;
}

void const
Tree::build()
{
    for (unsigned int level = 0; level < mLevelCount; ++level)
    {
        unsigned int const parent_count = mpStartingIndexPerLevel[level + 1]
                - mpStartingIndexPerLevel[level];

#pragma omp parallel for schedule(dynamic)
        for (unsigned int parent = 0; parent < parent_count; ++parent)
        {
            for (unsigned int child = 0; child < mpChildrenCountPerLevel[level]; ++child)
            {
                unsigned int const child_absolute_index = mpStartingIndexPerLevel[level + 1]
                        + (parent * mpChildrenCountPerLevel[level]) + child;

                mpIndexTree[child_absolute_index] = selectBestFeature(child_absolute_index,
                        level + 1);
                // mpInformativeContributionTree[child_index]
                // mpRedundantContributionTree[child_index]
            }
        }
    }
}

unsigned int const
Tree::selectBestFeature(unsigned int const absoluteIndex, unsigned int const level) const
{
    unsigned int max_candidate_feature_index = 0;
    float max_candidate_score = -std::numeric_limits<float>::max();

    for (unsigned int i = 0; i < mFeatureCount; ++i)
    {
        if (hasAncestorByFeatureIndex(absoluteIndex, i, level)
                || hasSiblingByIndex(absoluteIndex, i, level))
            continue;

        float candidate_feature_score = mpFeatureInformationMatrix[mpIndexTree[0] * mFeatureCount
                + i];

        unsigned int ancestor_absolute_index = absoluteIndex;
        float ancestry_score_mean = 0;
        if (level > 1)
            for (unsigned int j = level; j > 0; --j)
            {
                ancestor_absolute_index = getParentAbsoluteIndex(ancestor_absolute_index, j);
                ancestry_score_mean +=
                        mpFeatureInformationMatrix[mpIndexTree[ancestor_absolute_index]
                                * mFeatureCount + i];
            }

        ancestry_score_mean /= level + 1;
        float candidate_score = candidate_feature_score - ancestry_score_mean;

        if (candidate_score > max_candidate_score && !isRedundantSolution(absoluteIndex, i, level))
        {
            max_candidate_feature_index = i;
            max_candidate_score = candidate_score;
        }
    }

    return max_candidate_feature_index;
}

bool const
Tree::isRedundantSolution(unsigned int const absoluteIndex, unsigned int const featureIndex,
        unsigned int const level) const
{
    unsigned int const upper_bound =
            (level == mLevelCount) ? mTreeElementCount : mpStartingIndexPerLevel[level + 1];
    float score = computeQualityScore(absoluteIndex, level);
    for (unsigned int i = mpStartingIndexPerLevel[level]; i < upper_bound; ++i)
        if (                //fabs(computeQualityScore(i, level) - score) > 0.00001
        hasAncestorByFeatureIndex(i, featureIndex, level)
                && hasAncestorByFeatureIndex(absoluteIndex, mpIndexTree[i], level))
            return true;

    return false;
}

bool const
Tree::hasSameAncestry(unsigned int const absoluteIndex1, unsigned int const absoluteIndex2,
        unsigned int const level) const
{
    unsigned int parent_absolute_index = absoluteIndex1;
    for (unsigned int i = level; i > 0; --i)
    {
        parent_absolute_index = getParentAbsoluteIndex(parent_absolute_index, i);
        if (!hasAncestorByFeatureIndex(parent_absolute_index, mpIndexTree[absoluteIndex2], i))
            return false;
    }

    return true;
}

float const
Tree::computeQualityScore(unsigned int const absoluteIndex, unsigned int const level) const
{
    return mpInformativeContributionTree[absoluteIndex]
            - 2 * mpRedundantContributionTree[absoluteIndex] / level;
}

bool const
Tree::hasAncestorByFeatureIndex(unsigned int const absoluteIndex, unsigned int const featureIndex,
        unsigned int level) const
{
    unsigned int parent_absolute_index = absoluteIndex;
    for (unsigned int i = level; i > 0; --i)
    {
        parent_absolute_index = getParentAbsoluteIndex(parent_absolute_index, i);
        if (mpIndexTree[parent_absolute_index] == featureIndex)
            return true;
    }

    return false;
}

bool const
Tree::hasSiblingByIndex(unsigned int const absoluteIndex, unsigned int const featureIndex,
        unsigned int const level) const
{
    unsigned int const child_number = (absoluteIndex - mpStartingIndexPerLevel[level])
            % mpChildrenCountPerLevel[level - 1];

    for (unsigned int i = absoluteIndex - child_number; i < absoluteIndex; ++i)
        if (mpIndexTree[i] == featureIndex)
            return true;

    return false;
}

void const
Tree::getPaths(std::vector<unsigned int>* const pPaths) const
{
    pPaths->reserve(mLevelCount * (mTreeElementCount - mpStartingIndexPerLevel[mLevelCount]));

    for (unsigned int end_element_absolute_index = mTreeElementCount - 1;
            end_element_absolute_index >= mpStartingIndexPerLevel[mLevelCount];
            --end_element_absolute_index)
    {
        unsigned int element_absolute_index = end_element_absolute_index;

        for (unsigned int level = mLevelCount; level > 0; --level)
        {
            pPaths->push_back(mpIndexTree[element_absolute_index]);
            element_absolute_index = getParentAbsoluteIndex(element_absolute_index, level);
        }
    }
}

/* inline */unsigned int const
Tree::getParentAbsoluteIndex(unsigned int const absoluteIndex, unsigned int const level) const
{
    return (absoluteIndex - mpStartingIndexPerLevel[level]) / mpChildrenCountPerLevel[level - 1]
            + mpStartingIndexPerLevel[level - 1];
}
