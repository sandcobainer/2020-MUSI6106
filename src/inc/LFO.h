#ifndef LFO_h
#define LFO_h

#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdlib>

#include "ErrorDef.h"
#include "Util.h"
#include "RingBuffer.h"

/*! \LFO class for a wavetable LFO
*/
class LFO
{
public:
    LFO (float fSampleRate) :
    waveTable(0)
    {
        assert(fSampleRate > 0);
        
        wtSize = 8192 ;
        sampleRate = fSampleRate;
        phase = 0.f;
        
        waveTable =  new CRingBuffer<float>(wtSize);
        reset();
    }
    
    virtual ~LFO ()
    {
        delete [] waveTable;
        waveTable    = 0;
    }

    Error_t generateSine (float modFreq, float sampleRate)
    {
        increment = modFreq * wtSize / sampleRate;
        for (int i = 0; i < wtSize; i++)
        {
            waveTable->putPostInc(static_cast<float>(sin (2*M_PI * i / wtSize)) );
        }

        return kNoError;
    }
    
    float getVal()
    {
        phase = fmod ((phase + increment), wtSize - 1);
        return depth * (waveTable->get(phase));
    }
    
    Error_t setDepth(float amplitude)
    {
        depth = amplitude;
        return kNoError;
    }
    
    float getReadIdx()
    {
        return waveTable->getReadIdx();
    }
    
    float getWriteIdx()
    {
        return waveTable->getWriteIdx();
    }
    /*! set buffer content to 0 AND set phase index to 0
     \return void
     */
    void reset ()
    {
        waveTable->reset ();
        waveTable->setWriteIdx(0);
        phase = 0.f;
    }
    
    float           increment;
    
protected:
    CRingBuffer<float>  *waveTable;
    
    
private:
    float           sampleRate;                  //!< audio sample rate in Hz
    int             wtSize;                      //!< size of waveTable (equal to width/amplitude of Vibrato)
    float           phase;                       //!< index of current location in waveTable
    float           depth;                       //!< depth is the amplitude of LFO
};

#endif /* LFO_h */
