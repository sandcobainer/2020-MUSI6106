
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"
#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath,
                            sFilterType;
    
    float                   sFilterDelay,
                            sFilterGain,
                            sMaxFilterDelay;
    
    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float                   **ppfAudioData = 0;

    CAudioFileIf            *phAudioFile = 0,
                            *phAudioOutputFile = 0;
    CCombFilterIf           *pcCombFilter = 0;
    CAudioFileIf::FileSpec_t stFileSpec;
    
    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 5)
    {
        cout << "Missing paramters, try again!";

        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = "/Users/sandeepdasari/Downloads/new.wav";
        
        sFilterType = argv[2];
        sMaxFilterDelay = atof(argv[3]);
        sFilterDelay = atof(argv[4]);
        sFilterGain = atof(argv[5]);
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    CAudioFileIf::create(phAudioOutputFile);
    
    CCombFilterIf::create(pcCombFilter);
    
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);
    
    //////////////////////////////////////////////////////////////////////////////
    // init the comb filter interface
    if(sFilterType == "FIR") {
        pcCombFilter->init(CCombFilterIf::kCombFIR, sMaxFilterDelay, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
        pcCombFilter->setParam(CCombFilterIf::kParamDelay, sFilterDelay);
        pcCombFilter->setParam(CCombFilterIf::kParamGain, sFilterGain);
    }
    else if(sFilterType == "IIR") {
        pcCombFilter->init(CCombFilterIf::CombFilterType_t::kCombIIR, sMaxFilterDelay,stFileSpec.fSampleRateInHz,stFileSpec.iNumChannels);
        pcCombFilter->setParam(CCombFilterIf::kParamDelay, sFilterDelay);
        pcCombFilter->setParam(CCombFilterIf::kParamGain, sFilterGain);
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        ppfAudioData[i] = new float[kBlockSize];
    }
    
    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    // create a pointer to the iNumChannels number of buffer
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for(int i=0;i<stFileSpec.iNumChannels;i++)
        // create buffer of size blockSize
        ppfAudioData[i]=new float[kBlockSize];
    
    
    time = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and process it
    phAudioOutputFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);
        
        cout << "\r" << "reading and writing";
        pcCombFilter->process(ppfAudioData, ppfAudioData, (int) iNumFrames);
        phAudioOutputFile->writeData(ppfAudioData, iNumFrames);
    }
    
    
    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);

    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfAudioData[i];
    }
    
    delete[] ppfAudioData;
    ppfAudioData = 0;
    // all done
    return 0;
}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

