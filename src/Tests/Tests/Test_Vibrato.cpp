#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>

#include "UnitTest++.h"
#include "Vector.h"
#include "Vibrato.h"
#include "Synthesis.h"

SUITE(Vibrato)
{
    struct VibratoData
    {
        VibratoData() :
        m_pVibrato(0),
        m_ppfInputData(0),
        m_ppfOutputData(0),
        m_iDataLength(35131),
        m_fDelayLengthInS(0.2F),
        m_iBlockLength(171),
        m_iNumChannels(3),
        m_fSampleRateInHz(8000),
        m_fParamRate(10),
        m_fParamDepth(0.7)
        {
            m_pVibrato = new Vibrato::Vibrato (m_fDelayLengthInS, m_fWidthInS, m_fSampleRateInHz, m_iNumChannels);
            
            m_ppfInputData  = new float*[m_iNumChannels];
            m_ppfOutputData = new float*[m_iNumChannels];
            m_ppfInputTmp   = new float*[m_iNumChannels];
            m_ppfOutputTmp  = new float*[m_iNumChannels];
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_ppfInputData[i]   = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfInputData[i], m_iDataLength);
                m_ppfOutputData[i]  = new float [m_iDataLength];
                CVectorFloat::setZero(m_ppfOutputData[i], m_iDataLength);            }
        }
        
        ~VibratoData()
        {
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfOutputData[i];
                delete [] m_ppfInputData[i];
            }
            delete [] m_ppfOutputTmp;
            delete [] m_ppfInputTmp;
            delete [] m_ppfOutputData;
            delete [] m_ppfInputData;
            
            delete  m_pVibrato ;
        }
        
        void TestProcess()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);
                
                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                    m_ppfOutputTmp[c]   = &m_ppfOutputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pVibrato->process(m_ppfInputTmp, m_ppfOutputTmp, iNumFrames);
                
                iNumFramesRemaining -= iNumFrames;
            }
        }
        void TestProcessInplace()
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);
                
                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pVibrato->process(m_ppfInputTmp, m_ppfInputTmp, iNumFrames);
                
                iNumFramesRemaining -= iNumFrames;
            }
        }
        
        Vibrato *m_pVibrato;
        float   **m_ppfInputData,
                **m_ppfOutputData,
                **m_ppfInputTmp,
                **m_ppfOutputTmp;
        int     m_iDataLength;
        float   m_fDelayLengthInS;
        float   m_fWidthInS;
        int     m_iBlockLength;
        int     m_iNumChannels;
        float   m_fSampleRateInHz;
        float   m_fWidthInSamples;
        float   m_fParamRate,
                m_fParamDepth;
        
        float   *m_pfData;
    };


    UNITTEST_TEST(ParamsTest)
    {
        Vibrato *m_pVibrato = new Vibrato::Vibrato (0.005, 0.003, 44100, 2);

        m_pVibrato->setParam(Vibrato::kParamRate, 5.0);
        m_pVibrato->setParam(Vibrato::kParamDepth, 0.2);

        CHECK_EQUAL(m_pVibrato->getParam(Vibrato::kParamRate), 5.0f);
        CHECK_EQUAL(m_pVibrato->getParam(Vibrato::kParamDepth), 0.2f);

        m_pVibrato->reset();
    }

    UNITTEST_TEST(ParamsRangeTest)
    {
        Vibrato *m_pVibrato = new Vibrato::Vibrato (0.005, 0.003, 44100, 2);

        m_pVibrato->setParam(Vibrato::kParamRate, 100.0);
        m_pVibrato->setParam(Vibrato::kParamDepth, -1.0);

        // params should remain at 0.f
        CHECK_EQUAL(m_pVibrato->getParam(Vibrato::kParamRate), 0.f);
        CHECK_EQUAL(m_pVibrato->getParam(Vibrato::kParamDepth), 0.f);

        m_pVibrato->reset();
    }

    TEST_FIXTURE(VibratoData, ZeroInput)
    {
        m_pVibrato->setParam(Vibrato::kParamRate, m_fParamRate);
        m_pVibrato->setParam(Vibrato::kParamDepth, m_fParamDepth);

        TestProcess();
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);

        m_pVibrato->reset();
    }

    TEST_FIXTURE(VibratoData, ModZeroIsDelayedBuffer)
    {
        int m_fDelayLengthInSamples = floor(m_fDelayLengthInS * m_fSampleRateInHz);
        m_iDataLength = 1024;

        m_pVibrato->setParam(Vibrato::kParamRate, m_fParamRate);
        m_pVibrato->setParam(Vibrato::kParamDepth, 0);

        for (int c = 0; c < m_iNumChannels; c++)
            CSynthesis::generateSine(m_ppfInputData[c], 20.F, m_fSampleRateInHz , m_iDataLength, .7F, static_cast<float>(M_PI_2));

        TestProcess();

        for (int c = 0; c < m_iNumChannels; c++)
        {
            for (int i = 0; i < m_iDataLength; i++)
            {
                if ( i >= m_fDelayLengthInSamples)
                    CHECK_CLOSE(m_ppfOutputTmp[c][i], m_ppfInputTmp[c][i-m_fDelayLengthInSamples], 1e-3F);
            }
        }

        m_pVibrato->reset();
    }

    TEST_FIXTURE(VibratoData, DCInput)
    {
        int m_fDelayLengthInSamples = floor(m_fDelayLengthInS * m_fSampleRateInHz);
        m_iDataLength = 1024;

        m_pVibrato->setParam(Vibrato::kParamRate, m_fParamRate);
        m_pVibrato->setParam(Vibrato::kParamDepth, 0);

        for (int c = 0; c < m_iNumChannels; c++)
            CSynthesis::generateDc(m_ppfInputData[c], m_iDataLength);
        TestProcess();

        for (int c = 0; c < m_iNumChannels; c++)
        {
            for (int i = 0; i < m_iDataLength; i++)
            {
                if ( i >= m_fDelayLengthInSamples)
                    CHECK_CLOSE(m_ppfOutputData[c][i], m_ppfInputData[c][i], 1e-3F);
            }
        }

        m_pVibrato->reset();
    }

    TEST_FIXTURE(VibratoData, VaryingBlocksize)
    {
        for (int c = 0; c < m_iNumChannels; c++)
            CSynthesis::generateSine (m_ppfInputData[c], 387.F, m_fSampleRateInHz, m_iDataLength, .8F, static_cast<float>(c*M_PI_2));

        m_pVibrato-> setParam(Vibrato::kParamRate, m_fParamRate);
        m_pVibrato-> setParam(Vibrato::kParamDepth, m_fParamDepth);

        TestProcess();
        m_pVibrato-> reset();

        m_pVibrato-> setParam(Vibrato::kParamRate, m_fParamRate);
        m_pVibrato-> setParam(Vibrato::kParamDepth, m_fParamDepth);
        {
            int iNumFramesRemaining = m_iDataLength;
            while (iNumFramesRemaining > 0)
            {

                int iNumFrames = std::min(static_cast<float>(iNumFramesRemaining), static_cast<float>(rand())/RAND_MAX*17000.F);

                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c]    = &m_ppfInputData[c][m_iDataLength - iNumFramesRemaining];
                }
                m_pVibrato->process(m_ppfInputTmp, m_ppfInputTmp, iNumFrames);

                iNumFramesRemaining -= iNumFrames;
            }
        }

        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_iDataLength, 1e-3);
    }


    TEST_FIXTURE(VibratoData, ZeroRateIsDelayedBuffer)
    {
        int m_fDelayLengthInSamples = floor(m_fDelayLengthInS * m_fSampleRateInHz);
        m_iDataLength = 1024;

        for (int c = 0; c < m_iNumChannels; c++)
            CSynthesis::generateSine (m_ppfInputData[c], 387.F, m_fSampleRateInHz, m_iDataLength, .8F, static_cast<float>(c*M_PI_2));

        m_pVibrato->setParam(Vibrato::kParamRate, 0);
        m_pVibrato->setParam(Vibrato::kParamDepth, m_fParamDepth);

        TestProcess();

        for (int c = 0; c < m_iNumChannels; c++)
        {
            for (int i = 0; i < m_iDataLength; i++)
            {
                if ( i >= m_fDelayLengthInSamples)
                    CHECK_CLOSE(m_ppfOutputTmp[c][i], m_ppfInputTmp[c][i-m_fDelayLengthInSamples], 1e-3F);
            }
        }
        m_pVibrato->reset();
    }
}
#endif //WITH_TESTS


