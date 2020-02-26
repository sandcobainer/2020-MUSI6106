//
//  Vibrato.h
//  MUSI6106
//
//  Created by Sandeep on 2/15/20.
//
#ifndef LFO_h
#define LFO_h

#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdlib>

#include "ErrorDef.h"
#include "Util.h"
#include "RingBuffer.h"

/*! \brief LFO class for a wavetable LFO
 *         using RingBuffer
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

    /*! generate a sine way and store in a waveTable ring buffer
     \param modFreq frequency of sine wave used to calculate increment
     \param sampleRate sampleRate in Hertz
     \return Error_t
     */
    Error_t generateSine (float modFreq, float sampleRate)
    {
        increment = modFreq * wtSize / sampleRate;
        for (int i = 0; i < wtSize; i++)
        {
            waveTable->putPostInc(static_cast<float>(sin (2*M_PI * i / wtSize)) );
        }

        return kNoError;
    }
    
    /*! get current waveTable value based on increment
     \return float
     */
    float getVal()
    {
        phase = fmod ((phase + increment), wtSize - 1);
        return depth * (waveTable->get(phase));
    }
    
    /*! set amplitude of LFO
     \param amplitude depth of waveTable LFO
     \return Error_t
     */
    Error_t setDepth(float amplitude)
    {
        depth = amplitude;
        return kNoError;
    }
    
    /*! get read index of waveTable ring buffer
     \return float
     */
    float getReadIdx()
    {
        return waveTable->getReadIdx();
    }
    
    /*! get write index of waveTable ring buffer
     \return float
     */
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
    
    
protected:
    CRingBuffer<float>  *waveTable;              //!< ringbuffer for waveTable LFO

private:
    float           increment;                   //!< store increment of waveTable based on modFrequency and wtSize
    float           sampleRate;                  //!< audio sample rate in Hz
    int             wtSize;                      //!< size of waveTable (equal to width/amplitude of Vibrato)
    float           phase;                       //!< index of current location in waveTable
    float           depth;                       //!< depth is the amplitude of LFO
};

#endif /* LFO_h */
