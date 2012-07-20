#ifndef ensemble_Matrix_hpp
#define ensemble_Matrix_hpp

class Matrix
{
protected:
    float* const mpData;
    unsigned int const mRowCount;
    unsigned int const mColumnCount;
    bool const mHasAllocation;

public:
    Matrix(unsigned int const size, unsigned int const rowCount, unsigned int const columnCount);

    Matrix(unsigned int const rowCount, unsigned int const columnCount);

    Matrix(float* const data, unsigned int const rowCount, unsigned int const columnCount);

    virtual
    ~Matrix();

    virtual float&
    operator()(unsigned int const i, unsigned int const j);

    unsigned int const
    getColumnCount() const;

    unsigned int const
    getRowCount() const;
};

#endif /* ensemble_Matrix_hpp */
