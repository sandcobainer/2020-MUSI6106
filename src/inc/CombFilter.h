#ifndef CombFilter_h
#define CombFilter_h

#include "CombFilterIf.h"

/*! \brief header file for FIR and IIR comb filters.
 */
class CombFilter : public CCombFilterIf
{
public:
    CombFilter ();
    virtual ~CombFilter();
    float getParam(FilterParam_t eParam);
    
private:
    float maxDelay;
    float gain;
    float delay;
    float sampleRate;
    int channels;
    int delayInSamples;
    int delayIndex;
    float **ppfDelay = 0;
    
    Error_t processFIR(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) override;
    Error_t processIIR(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) override;
    Error_t initIntern(CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) override;
    Error_t resetIntern() override;
    Error_t setParamIntern(FilterParam_t eParam, float fParamValue) override ;
};
#endif /* CombFilter_h */



