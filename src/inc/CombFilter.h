#if !defined(__CombFilter_hdr__)
#define __CombFilter_hdr__

#include "CombFilterIf.h"


class CCombFilter : public CCombFilterIf
{
public:
    CCombFilter();
    virtual ~CCombFilter();

private:
    CCombFilter(const CCombFilter& that);
    Error_t initIntern() ;
    Error_t resetIntern() ;
    Error_t FIRIntern(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumberOfFrames) override;
    Error_t IIRIntern(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumberOfFrames) override;

    float  **delayLine;           //!< delay line
};

#endif  //__CombFilter_hdr__

