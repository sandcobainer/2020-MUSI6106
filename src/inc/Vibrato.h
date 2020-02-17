//
//  Vibrato.h
//  MUSI6106
//
//  Created by user on 2/15/20.
//

#ifndef Vibrato_h
#define Vibrato_h

#include "ErrorDef.h"
#include "RingBuffer.h"
#include "LFO.h"

/*! \Vibrato class to simulate the vibrato effect using a RingBuffer and an LFO
 */
class Vibrato
{
public:
    Vibrato (float fDelayInSeconds, float fWidthInSeconds, float fSampleRateInHz, int iNumChannels);
    virtual ~Vibrato ();
    
    /*! list of parameters for the comb filters */
    enum FilterParam_t
    {
        kParamRate,                     //!< rate or modulation frequency of LFO changes speed of vibrato
        kParamDepth,

        kNumFilterParams
    };
    
    /*! returns the date of the build
     \return const char*
     */
    static const char* getBuildDate ();
    
    /*! initializes a Vibrato instance
     \param fDelayLengthInS basic delay in seconds
     \param fSampleRateInHz sample rate in Hz
     \param iNumChannels number of audio channels
     \return Error_t
     */
    Error_t init (float fDelayLengthInS, float fSampleRateInHz, int iNumChannels);
    
    /*! resets the internal variables  (requires new call of init)
     \return Error_t
     */
    Error_t reset ();
    
    /*! sets a Vibrato parameter
     \param eParam what parameter (see ::FilterParam_t)
     \param fParamValue value of the parameter
     \return Error_t
     */
    Error_t setParam (FilterParam_t eParam, float fParamValue);
    
    /*! return the value of the specified parameter
     \param eParam
     \return float
     */
    float   getParam (FilterParam_t eParam) const;
    
    /*! processes one block of audio
     \param ppfInputBuffer input buffer [numChannels][iNumberOfFrames]
     \param ppfOutputBuffer output buffer [numChannels][iNumberOfFrames]
     \param iNumberOfFrames buffer length (per channel)
     \return Error_t
     */
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
protected:
    CRingBuffer<float>  **m_ppCDelayRing;
    LFO                 *m_lfoBuffer;
    
    float   m_afParam[Vibrato::kNumFilterParams];
    float   m_aafParamRange[Vibrato::kNumFilterParams][2];
    
private:
    
    Vibrato(const Vibrato& that);
    
    bool    isInParamRange (Vibrato::FilterParam_t eParam, float fValue);
    float           writePosL;
    
    float           m_fSampleRate;                  //!< audio sample rate in Hz
    int             m_iNumChannels;                 //!< number of audio channels
    float           m_fDelayLengthInSamples;        //!< basic delay length to start from
    float           m_fMaxDelayLengthInSamples;
    float           m_fWidthInSamples;
};


#endif /* Vibrato_h */
