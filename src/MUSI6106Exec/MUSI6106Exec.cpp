
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

void    showClInfo();

int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath,
                            sFilterType;

    float                   sFilterDelay,                  //!< filter params
                            sFilterGain,
                            sMaxFilterDelay;

    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float                   **ppfAudioInputData = 0,
                            **ppfAudioOutputData = 0;

    // Audio IO file objects
    CAudioFileIf            *phAudioInputFile = 0,
                            *phAudioOutputFile = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    CCombFilterIf           *pcCombFilter = 0;
    CCombFilterIf::CombFilterType_t stFilterType;               //!< filter  type

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 6)
    {
        cout << "Missing paramters, try again!";

        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = sInputFilePath + "_processed.wav";

        sFilterType = argv[2];
        sMaxFilterDelay = std::stof(argv[3]);
        sFilterDelay = std::stof(argv[4]);
        sFilterGain = std::stof(argv[5]);
    }



    //////////////////////////////////////////////////////////////////////////////
    // open  input wave file
    phAudioInputFile->create(phAudioInputFile);
    phAudioInputFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioInputFile->isOpen()) { //check
        cout << "Error: Cannot open, Input file already open.";
        return -1;
    }
    phAudioInputFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open  output wave file here too
    phAudioOutputFile->create(phAudioOutputFile);
    phAudioOutputFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite,  &stFileSpec);   // open as write!
    if (!phAudioOutputFile->isOpen()) { //check
        cout << "Error: Cannot open, output file already open.";
        return -1;
    }


    //////////////////////////////////////////////////////////////////////////////
    // init the comb filter interface
    pcCombFilter->create(pcCombFilter);

    if (sFilterType=="FIR")
        pcCombFilter->init(CCombFilterIf::kCombFIR, sMaxFilterDelay, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    else if (sFilterType == "IIR")
        pcCombFilter->init(CCombFilterIf::kCombIIR, sMaxFilterDelay,stFileSpec.fSampleRateInHz,stFileSpec.iNumChannels);
    else
    {
    cout << "Error: Cannot initialize combfilter interface due to invalid filterType (FIR/IIR)";
    }
    // set params for the selected filter type
    pcCombFilter->setParam(CCombFilterIf::kParamDelay, sFilterDelay);
    pcCombFilter->setParam(CCombFilterIf::kParamGain, sFilterGain);


    //////////////////////////////////////////////////////////////////////////////
    // allocate memory for audio
    // create a pointer to the iNumChannels number of buffer
    ppfAudioInputData = new float* [stFileSpec.iNumChannels];
    ppfAudioOutputData = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        ppfAudioInputData[i] = new float[kBlockSize];
        ppfAudioOutputData[i] = new float[kBlockSize];
    }

    time = clock();


    //////////////////////////////////////////////////////////////////////////////
    // read, process and write audio
    while (!phAudioInputFile->isEof()) //until end of input file is reached
    {
        long long iNumFrames = kBlockSize; //number of frames to read at a time
        //read in 1024 frames from audio file channels 1 and 2
        //store in ppfInputAudioData[0] and ppfInputAudioData[1] respectively
        phAudioInputFile->readData(ppfAudioInputData, iNumFrames);

        cout << "\r" << "reading and writing";

        //apply the comb filter and write data to ppfOutputAudioData[0] and ppfOutputAudioData[1] respectively
        pcCombFilter->process(ppfAudioInputData, ppfAudioOutputData, (int)iNumFrames);

        //write processed audio data to the output file
        phAudioOutputFile->writeData(ppfAudioOutputData, iNumFrames);
    }

    cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;


    //////////////////////////////////////////////////////////////////////////////
    // cleaning up
    phAudioInputFile->closeFile();
    phAudioOutputFile->closeFile();

    CAudioFileIf::destroy(phAudioInputFile);
    CAudioFileIf::destroy(phAudioOutputFile);
    CCombFilterIf::destroy(pcCombFilter);

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete[] ppfAudioInputData[i];
        delete[] ppfAudioOutputData[i];
    }

    delete[] ppfAudioInputData;
    delete[] ppfAudioOutputData;

    ppfAudioInputData = 0;
    ppfAudioOutputData = 0;
}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout << endl;

    return;
}
