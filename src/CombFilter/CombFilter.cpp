//
//  CombFilter.cpp
//  MUSI6106Exec
//
//  Created by Sandeep Dasari on 1/19/20.
//
#include <cassert>
#include <iostream>

#include "Util.h"
#include "Vector.h"

#include "CombFilter.h"

using namespace std;

void shift(float* delayLine, long length, float newVal);

CCombFilter::CCombFilter() : CCombFilterIf(),
delayLine(0)
{
    reset();
}

CCombFilter::~CCombFilter()
{
    reset();
}

Error_t CCombFilter::resetIntern()
{
    if (!delayLine)
        return kMemError;
    for (int i = 0; i < getNumChannels(); i++)
        delete[] delayLine[i];
    delete[] delayLine;
    delayLine = 0;

    return kNoError;
}

Error_t CCombFilter::initIntern()
{
    resetIntern();
    int channels = getNumChannels();
    delayLine = new float* [channels];
    //allocate memory buffer
    for(int i = 0; i < channels; i++)
        delayLine[i] = new float[static_cast<unsigned int>(m_DelayLineLength)];
    // fill delayLine with 0's
    for (int c = 0; c < channels; c++)
    {
        for (int i = 0; i < m_DelayLineLength; i++)
        {
            delayLine[c][i] = 0;
        }
    }

//    if (!delayLine)
//        return kMemError;
//    else
    return kNoError;
}

Error_t CCombFilter::FIRIntern(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumberOfFrames)
{

    int channels = getNumChannels();
    float gain = getParam(CCombFilterIf::FilterParam_t::kParamGain);

    for (int c = 0; c < channels; c++)
    {
        for (int i = 0; i < iNumberOfFrames; i++)
        {
            if (m_DelayLineLength == 0) {
                ppfOutputBuffer[c][i] = ppfInputBuffer[c][i];
            }
            else {
                ppfOutputBuffer[c][i] = ppfInputBuffer[c][i] + (gain * delayLine[c][m_DelayLineLength - 1]);
                shift(delayLine[c], m_DelayLineLength, ppfOutputBuffer[c][i]);
            }

        }
    }

    return kNoError;
}

Error_t CCombFilter::IIRIntern(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumberOfFrames)
{
    int channels = getNumChannels();
    float gain = getParam(CCombFilterIf::FilterParam_t::kParamGain);

    for (int c = 0; c < channels; c++)
    {
        for (int i = 0; i < iNumberOfFrames; i++)
        {
            if (m_DelayLineLength == 0) {
                ppfOutputBuffer[c][i] = ppfInputBuffer[c][i];
            }
            else {
                float y = 0;
                y = ppfInputBuffer[c][i] + (gain * delayLine[c][m_DelayLineLength - 1]);
                ppfOutputBuffer[c][i] = ppfOutputBuffer[c][i] + (gain * )
                shift(delayLine[c], m_DelayLineLength, ppfOutputBuffer[c][i]);
            }
        }
    }

    return kNoError;
}

// receive delayLine as pointer to change memory addresses instead of local copy
void shift(float* delayLine, long length, float newSample)
{
    float a = delayLine[0];
    float temp = 0;
    for (int i = 1; i < length; i++)
    {
        temp = delayLine[i];
        delayLine[i] = a;
        a = temp;
    }
    delayLine[0] = newSample;
}