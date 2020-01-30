//
//  CombFilter.cpp
//  MUSI6106Exec
//
//  Created by Sandeep Dasari on 1/19/20.
//
#include "CombFilter.h"
#include "CombFilterIf.h"

CombFilter::CombFilter(): CCombFilterIf() {
}

CombFilter::~CombFilter() {
    this->reset();
}


Error_t CombFilter::initIntern(CCombFilterIf::CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) {
    setFilterType(eFilterType);
    maxDelay = fMaxDelayLengthInS;
    sampleRate = fSampleRateInHz;
    channels = iNumChannels;
    ppfDelay = new float*[channels];
    delayInSamples = maxDelay * sampleRate;
    delayIndex =  0;
    for (int i = 0; i < channels; i++) {
        ppfDelay[i] = new float[(int) (maxDelay * fSampleRateInHz)];
    }
    
    
    return kNoError;
}

Error_t CombFilter::resetIntern() {
    maxDelay = 0;
    gain = 0;
    delay = 0;
    sampleRate = 0;
    channels = 0;
    delayInSamples = 0;
    delayIndex = 0;
    **ppfDelay = 0;
}

float  CombFilter::getParam(CCombFilterIf::FilterParam_t eParam)  {
    switch(eParam){
        case kParamGain:
            return gain;
        case kParamDelay:
            return delay;
    }
}

Error_t CombFilter::setParamIntern(CCombFilterIf::FilterParam_t eParam, float fParamValue) {
    switch(eParam){
        case kParamGain:
            gain = fParamValue;
        case kParamDelay:
            delay = fParamValue;
    }
    return kNoError;
}

Error_t CombFilter::processFIR(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
    for (int i = 0; i < iNumberOfFrames; i++)
    {
        for (int c = 0; c < channels; c++)
        {
            float y = 0;
            if ( delayIndex < delayInSamples) {
                ppfDelay[c][delayIndex] = ppfInputBuffer[c][i];
                y = ppfInputBuffer[c][i];
                delayIndex+=1;
            }
            else
            {
                for (int k = 1; k<delayInSamples; k++)
                {
                    ppfDelay[c][k-1]=ppfDelay[c][k];
                }
                ppfDelay[c][delayInSamples-1] = ppfInputBuffer[c][i];
                y = ppfInputBuffer[c][i] + gain * (ppfDelay[c][0]);
            }
            ppfOutputBuffer[c][i] = y;
        }
    }
    return kNoError;
}

Error_t CombFilter::processIIR(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) {
    for (int i = 0; i < iNumberOfFrames; i++)
    {
        for (int c = 0; c < channels; c++)
        {
            float y = 0;
            if ( delayIndex < delayInSamples) {
                ppfDelay[c][delayIndex] = ppfInputBuffer[c][i];
                y = ppfInputBuffer[c][i];
                delayIndex+=1;
            }
            else
            {
                for (int k = 1; k<delayInSamples; k++)
                {
                    ppfDelay[c][k-1]=ppfDelay[c][k];
                }
                ppfDelay[c][delayInSamples-1] = ppfInputBuffer[c][i];
                y = ppfInputBuffer[c][i] +  gain * y + gain * (ppfDelay[c][0]) ;
            }
            ppfOutputBuffer[c][i] = y;
        }
    }
    return kNoError;
}
