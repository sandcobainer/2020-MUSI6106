
// standard headers
#include <iostream>
// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"
#include "RingBuffer.h"

#include "Vibrato.h"

static const char*  kCMyProjectBuildDate             = __DATE__;

Vibrato::Vibrato (float fDelayLengthInS, float fWidthSamplesInS, float fSampleRateInHz, int iNumChannels) :
m_ppCDelayRing(0)
{
    assert(fDelayLengthInS >= 0.0f);
    assert(fSampleRateInHz >= 0);
    assert(iNumChannels >= 0);
    
    m_fSampleRate               = fSampleRateInHz;
    m_iNumChannels              = iNumChannels;
    m_fDelayLengthInSamples     = fDelayLengthInS * fSampleRateInHz;
    m_fWidthInSamples           = fWidthSamplesInS * fSampleRateInHz;
    
    // calculate max delay length for buffer : 2 + basic_delay + (maxWidth*2)
    m_fMaxDelayLengthInSamples  = floor(2 + m_fDelayLengthInSamples + (1.0f * fSampleRateInHz* 2));
    
    // allocate memory for ring buffer delay
    m_ppCDelayRing = new CRingBuffer<float>*[m_iNumChannels];
    for (int c = 0; c < m_iNumChannels; c++)
        m_ppCDelayRing[c]  = new CRingBuffer<float>(m_fMaxDelayLengthInSamples);
    
    // init buffer with 0s
    for (int c = 0; c < m_iNumChannels; c++)
    {
        for (int i = 0; i < m_fMaxDelayLengthInSamples; i++)
        {
            m_ppCDelayRing[c]->putPostInc(0.F);
        }
    }
    
    // initialize LFO
    m_lfoBuffer = new LFO (m_fSampleRate);
    
    // initialize params
    for (int i = 0; i < Vibrato::kNumFilterParams; i++)
    {
        m_afParam[i]            = 0.F;
    }
    
    // set ranges for params
    m_aafParamRange[Vibrato::kParamRate][0]  = 0.f;
    m_aafParamRange[Vibrato::kParamRate][1]  = 14.0f;
    
    m_aafParamRange[Vibrato::kParamDepth][0]  = 0.f;
    m_aafParamRange[Vibrato::kParamDepth][1]  = 1.0f;
}

Vibrato::~Vibrato ()
{
    if (m_ppCDelayRing)
    {
        for (int c = 0; c < m_iNumChannels; c++)
            delete m_ppCDelayRing[c];
    }
    delete [] m_ppCDelayRing;
    
}

const char*  Vibrato::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t Vibrato::reset()
{
    for (int c = 0; c < m_iNumChannels; c++)
    {
        m_ppCDelayRing[c]->reset ();
        m_ppCDelayRing[c]->setWriteIdx(CUtil::float2int<int>(m_fDelayLengthInSamples));
    }
    
    m_fDelayLengthInSamples   = 0;
    m_fSampleRate       = 0;
    m_iNumChannels      = 0;
    
    return kNoError;
}

bool Vibrato::isInParamRange( Vibrato::FilterParam_t eParam, float fValue )
{
    if (fValue < m_aafParamRange[eParam][0] || fValue > m_aafParamRange[eParam][1])
    {
        return false;
    }
    else
    {
        return true;
    }
}

Error_t Vibrato::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    for(int c = 0; c < m_iNumChannels; c++)
    {
        
        for (int i = 0; i < iNumberOfFrames; i++)
        {
            float sine   =  m_lfoBuffer->getVal();
            float offset =  m_fDelayLengthInSamples + (sine * m_fWidthInSamples);
            
            float writeInd     = m_ppCDelayRing[c] -> getWriteIdx();
            float ringContents = 0;
            if (offset <  writeInd)
            {
                m_ppCDelayRing[c] -> setReadIdx(writeInd - offset);
                ringContents      = m_ppCDelayRing[c]->getPostInc();
                
                ppfOutputBuffer[c][i] = ppfInputBuffer[c][i] + ringContents;
                m_ppCDelayRing[c]     ->putPostInc(ppfInputBuffer[c][i]);
            }
            else
            {
                ringContents = 0;
                ppfOutputBuffer[c][i] = ppfInputBuffer[c][i] + ringContents;
                m_ppCDelayRing[c]->putPostInc(ppfInputBuffer[c][i]);
            }
        }
    }
    return kNoError;
}


Error_t Vibrato::setParam( Vibrato::FilterParam_t eParam, float fParamValue )
{
    if (!isInParamRange(eParam, fParamValue))
        return kFunctionInvalidArgsError;
    
    // convert mod frequency or rate into samples
    if (eParam == Vibrato::kParamRate)
    {
        m_afParam[eParam]  = fParamValue;
        m_lfoBuffer->generateSine(fParamValue, m_fSampleRate);
    }
    // set depth of LFO
    else if (eParam == Vibrato::kParamDepth)
    {
        m_afParam[eParam]  = fParamValue;
        m_lfoBuffer->setDepth(fParamValue);
    }
    
    return kNoError;
}

float Vibrato::getParam( Vibrato::FilterParam_t eParam ) const
{
    return m_afParam[eParam];
}


