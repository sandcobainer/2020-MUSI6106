
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"

#include "CombFilterIf.h"
#include "CombFilter.h"

static const char*  kCMyProjectBuildDate             = __DATE__;
long int       CCombFilterIf::m_DelayLineLength; //delay line length

CCombFilterIf::CCombFilterIf () :
    m_bIsInitialized(false),
    m_fSampleRate(0)
{
    // this never hurts
    this->reset ();
}


CCombFilterIf::~CCombFilterIf ()
{
    this->reset ();
}


const int  CCombFilterIf::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;

    switch (eVersionIdx)
    {
    case kMajor:
        iVersion    = MUSI6106_VERSION_MAJOR; 
        break;
    case kMinor:
        iVersion    = MUSI6106_VERSION_MINOR; 
        break;
    case kPatch:
        iVersion    = MUSI6106_VERSION_PATCH; 
        break;
    case kNumVersionInts:
        iVersion    = -1;
        break;
    }

    return iVersion;
}
const char*  CCombFilterIf::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t CCombFilterIf::create( CCombFilterIf*& pCCombFilter)
{
    pCCombFilter = new CCombFilter();
    if (!pCCombFilter)
        return kMemError;

    return kNoError;
}

Error_t CCombFilterIf::destroy (CCombFilterIf*& pCCombFilter)
{
    delete pCCombFilter;
    pCCombFilter = 0;
    return kNoError;
}

Error_t CCombFilterIf::init( CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels )
{
    // check for parameter problems
    if (iNumChannels < 0 || fMaxDelayLengthInS < 0 )
        return kFunctionInvalidArgsError;
    reset();

    m_iNumChannels = iNumChannels;
    m_fSampleRate = fSampleRateInHz;
    m_eFilterType = eFilterType;
    m_fFilterGain = 1;
    m_DelayLineLength = fMaxDelayLengthInS * fSampleRateInHz;
    // set delay line to max length for now
    setParam(FilterParam_t::kParamDelay, fMaxDelayLengthInS);
    m_bIsInitialized = true;

    return initIntern();
}

Error_t CCombFilterIf::reset ()
{
    m_bIsInitialized = false;
    m_iNumChannels = 0;
    m_fSampleRate = 0;
    m_fFilterGain = 1;
    m_DelayLineLength = 0;
    setParam(FilterParam_t::kParamDelay, 0);
    setParam(FilterParam_t::kParamGain, 0);
    return resetIntern();
}

Error_t CCombFilterIf::process( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames )
{
    // check all buffers and frames
    if (!ppfInputBuffer || !ppfInputBuffer[0] || !ppfOutputBuffer || !ppfOutputBuffer[0] || iNumberOfFrames < 0)
        return kFunctionInvalidArgsError;

    // check file properties
    if (!m_bIsInitialized)
        return kNotInitializedError;

    switch(m_eFilterType) {
        case kCombFIR:
            return FIRIntern(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
        case kCombIIR:
            return IIRIntern(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
    }
    return kFunctionIllegalCallError;
}

Error_t CCombFilterIf::setParam( FilterParam_t eParam, float fParamValue )
{
    // check init status
    if (!m_bIsInitialized)
        return kNotInitializedError;

    switch(eParam) {
        case FilterParam_t::kParamDelay:
            // check if delay < 0 or if max delay set during init < current delay value
            if (fParamValue < 0 || getParam(FilterParam_t::kParamDelay) < fParamValue)
                return kFunctionInvalidArgsError;
            m_DelayLineLength = fParamValue * m_fSampleRate;
        case FilterParam_t::kParamGain:
            if (fParamValue < -1 || fParamValue > 1)
                return kFunctionInvalidArgsError;
            m_fFilterGain = fParamValue;
    }

    return kNoError;
}

float CCombFilterIf::getParam( FilterParam_t eParam ) const
{
    // check init status
    if (!m_bIsInitialized)
        return kNotInitializedError;

    switch(eParam) {
        case FilterParam_t::kParamDelay:
            return m_DelayLineLength / m_fSampleRate;
        case FilterParam_t::kParamGain:
            return m_fFilterGain;
    }

    return kFunctionInvalidArgsError;
}

Error_t CCombFilterIf::initIntern()
{
    return kNoError;
}

Error_t CCombFilterIf::resetIntern()
{
    return kNoError;
}

int CCombFilterIf::getNumChannels() const
{
    return m_iNumChannels;
}