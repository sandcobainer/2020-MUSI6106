
#include "Vector.h"
#include "Util.h"

#include "Dtw.h"

CDtw::CDtw( void )
{

}

CDtw::~CDtw( void )
{

}

Error_t CDtw::init( int iNumRows, int iNumCols )
{
    if (iNumRows <= 0 || iNumCols <= 0)
        return kFunctionInvalidArgsError;
    
    m_iNumRows = iNumRows;
    m_iNumCols = iNumCols;
    pathCost   = 0.f;
    
    ppfDistanceMatrix = new float *[iNumRows];
    for (int i = 0; i < m_iNumRows; i++)
    {
        ppfDistanceMatrix[i] = new float [m_iNumCols];
        for (int j = 0; j < m_iNumCols; j++)
            ppfDistanceMatrix[i][j] = 0;
    }
    
        
    ppfCostMatrix = new float *[iNumRows];
    for (int i = 0; i < m_iNumRows; i++)
    {
        ppfCostMatrix[i] = new float [m_iNumCols];
        for (int j = 0; j < m_iNumCols; j++)
            ppfCostMatrix[i][j] = 0;
    }
    
    ppiPathResult = new int *[2];
    for (int i = 0; i < 2; i++)
    {
        ppiPathResult[i] = new int [m_iNumRows + m_iNumCols - 2];
        for (int j = 0; j < m_iNumRows + m_iNumCols - 2; j++)
            ppiPathResult[i][j] = 0;
    }
    
    ppfDirectionsMatrix = new Directions_t *[iNumRows];
    for (int i = 0; i < m_iNumRows; i++)
    {
        ppfDirectionsMatrix[i] = new Directions_t [m_iNumCols];
        for (int j = 0; j < m_iNumCols; j++)
            ppfDirectionsMatrix[i][j] = Directions_t::kDiag;
    }
    
    m_bIsInitialized = true;
    
    return kNoError;
}

Error_t CDtw::reset()
{
    pathCost = 0.f;
    for (int i= 0; i < m_iNumRows; i++)
           delete ppfDistanceMatrix[i];
    delete [] ppfDistanceMatrix;
    ppfDistanceMatrix       = 0;
    
    for (int i= 0; i < m_iNumRows; i++)
        delete ppfCostMatrix[i];
    delete [] ppfCostMatrix;
    ppfCostMatrix       = 0;
    
    for (int i= 0; i < m_iNumRows; i++)
        delete ppiPathResult[i];
    delete [] ppiPathResult;
    ppiPathResult       = 0;

    for (int i= 0; i < m_iNumRows; i++)
        delete ppfDirectionsMatrix[i];
    delete [] ppfDirectionsMatrix;
    ppfDirectionsMatrix       = 0;
    
    m_bIsInitialized = false;
    
    return kNoError;
}

Error_t CDtw::process(float **ppfDistanceMatrix)
{
    if(!m_bIsInitialized)
        return kNotInitializedError;
    
    if(ppfDistanceMatrix == 0)
        return kFunctionInvalidArgsError;
    
    // first column
    for (int i = 1; i < m_iNumRows; i++)
        ppfCostMatrix[i][0] = ppfDistanceMatrix[i][0] +  ppfCostMatrix[i-1][0];
    // first row
    for (int j = 1; j < m_iNumCols; j++)
        ppfCostMatrix[0][j] = ppfDistanceMatrix[0][j] + ppfCostMatrix[0][j-1];

    // rest of the matrix
    for (int i = 1; i < m_iNumRows; i++)
    {
        for (int j = 1; j < m_iNumCols; j++)
        {
            ppfCostMatrix[i][j] = ppfDistanceMatrix[i][j] + getMinCost(ppfCostMatrix[i][j-1],                                                             ppfCostMatrix[i-1][j], ppfCostMatrix[i-1][j-1]);
        }
    }
    
    // backtracking
    int i = m_iNumRows - 1;
    int j = m_iNumCols - 1;
    
    while (i+j == 0)
    {
        if (i-1 < 0)
        {
            pathCost = ppfCostMatrix[i][j] + ppfCostMatrix[i][j-1];
            j = j - 1;
            
        }
        else if (j-1 < 0)
        {
            pathCost = ppfCostMatrix[i][j] + ppfCostMatrix[i-1][j];
            i = i-1;
        }
        else
        {
        pathCost = ppfCostMatrix[i][j] + getMinCost(ppfCostMatrix[i-1][j],ppfCostMatrix[i][j-1],ppfCostMatrix[i-1][j-1], &ppfDirectionsMatrix[i][j]);
            i = i-1;
            
        }
    }
    
    return kNoError;
}

int CDtw::getPathLength()
{    
    return 0;
}

float CDtw::getPathCost() const
{
    return 0.f;
}

Error_t CDtw::getPath( int **ppiPathResult ) const
{
    
    return kNoError;
}

float CDtw::getMinCost(float horiz, float vert, float diag, Directions_t& direction) const
{
    float min = diag;

    if (vert < diag)
        min = diag;
    if (horiz < diag and horiz < vert)
        min = horiz;

    return min;
}

//CDtw::Directions_t CDtw::getMinCost(float horiz, float vert, float diag) const
//{
//    Cdtw::Directions_t min = Directions_t::kDiag;
//
//    if (vert < diag)
//        min = Directions_t::kVert;
//    if (horiz < diag and horiz < vert)
//        min = horiz;
//
//    return min;
//}
