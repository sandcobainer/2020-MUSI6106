
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"
#include "CombFilter.h"
#include "CombFilterIf.h"

static const char*  kCMyProjectBuildDate             = __DATE__;


CCombFilterIf::CCombFilterIf () :
m_bIsInitialized(false)
//m_pCCombFilter(0),
{
    // this never hurts
    // this->reset();
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
    pCCombFilter = new CombFilter();
    if (!pCCombFilter)
        return kMemError;
    
    return kNoError;
}

Error_t CCombFilterIf::destroy (CCombFilterIf*& pCCombFilter)
{
    delete pCCombFilter;
    pCCombFilter  = 0;
    
    return kNoError;
}

Error_t CCombFilterIf::init(CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels )
{
    if (iNumChannels < 0 || fMaxDelayLengthInS < 0)
        return kFunctionInvalidArgsError;
    Error_t kError = initIntern(eFilterType, fMaxDelayLengthInS,fSampleRateInHz,iNumChannels);
    m_bIsInitialized = true;
    
    return kError;
}

Error_t CCombFilterIf::reset()
{
    Error_t kError = resetIntern();
    m_bIsInitialized = false;
    return kError;
}

Error_t CCombFilterIf::process( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames )
{
    Error_t kError;
    switch(m_eFilterType) {
        case kCombFIR:
            kError = processFIR(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
        case kCombIIR:
            kError = processIIR(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
    }
    return kError;
}

Error_t CCombFilterIf::setParam( FilterParam_t eParam, float fParamValue )
{
    if(!m_bIsInitialized)
        return kNotInitializedError;
    
    m_eParam = eParam;
    m_fParamValue = fParamValue;
    
    Error_t kError = setParamIntern(eParam,fParamValue);
    return kError;
}

float CCombFilterIf::getParam( FilterParam_t eParam ) const
{
    return kNoError;
}

Error_t CCombFilterIf::setFilterType(CombFilterType_t eFilterType)
{
    m_eFilterType   = eFilterType;
    return kNoError;
}

CCombFilterIf::CombFilterType_t CCombFilterIf::getFilterType(CombFilterType_t eFilterType)
{
    return m_eFilterType;
}
