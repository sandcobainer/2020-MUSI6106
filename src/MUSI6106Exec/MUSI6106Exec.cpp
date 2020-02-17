
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "Vibrato.h"

using std::cout;
using std::endl;

void    showClInfo();
int     applyFilter(std::string sInputFilePath, std::string sOutputFilePath, float sVibratoDelay, float sVibratoWidth, float sVibratoRate, float sVibratoDepth, int blockSize);

int main(int argc, char* argv[]) {
    std::string     sInputFilePath,                 //!< file paths
                    sOutputFilePath;
    
    float           sVibratoDelay,
                    sVibratoRate,                  //!< filter params
                    sVibratoDepth,
                    sVibratoWidth;
    
    static const int kBlockSize = 1024;
    showClInfo();
    
    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 4)
    {
        cout << "Missing paramters, try again!";
        return -1;
    }
    else
    {
        sInputFilePath  = argv[1];
        sOutputFilePath = sInputFilePath + "_processed.wav";
        sVibratoDelay   = std::stof(argv[2]);
        sVibratoWidth   = std::stof(argv[3]);
        sVibratoRate    = std::stof(argv[4]);
        sVibratoDepth   = std::stof(argv[5]);
        
        return applyFilter(sInputFilePath, sOutputFilePath, sVibratoDelay, sVibratoWidth, sVibratoRate, sVibratoDepth, kBlockSize);
    }
}

int applyFilter(std::string sInputFilePath, std::string sOutputFilePath, float sVibratoDelay, float sVibratoWidth, float sVibratoRate, float sVibratoDepth, int blockSize)
{
    // declare variables
    CAudioFileIf                *phAudioInputFile = 0;          //!< audio file objects
    CAudioFileIf                *phAudioOutputFile = 0;
    CAudioFileIf::FileSpec_t    stFileSpec;                     //!< file specs
    
    Vibrato                     *pcVibrato = 0;                   //!< Vibrato instance
    float                       **ppfAudioInputData = 0;            //!< audio data
    float                       **ppfAudioOutputData = 0;
    
    static const int            kBlockSize = blockSize;
    clock_t                     time = 0;
    
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
    // init the Vibrato unit
    pcVibrato = new Vibrato(sVibratoDelay, sVibratoWidth, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    
    // set params for the selected filter type
    pcVibrato->setParam(Vibrato::kParamRate, sVibratoRate);
    pcVibrato->setParam(Vibrato::kParamDepth, sVibratoDepth);
    
    
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
    while (!phAudioInputFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioInputFile->readData(ppfAudioInputData, iNumFrames);
        cout << "\r" << "reading and writing";
        pcVibrato->process(ppfAudioInputData, ppfAudioOutputData, (int)iNumFrames);
        phAudioOutputFile->writeData(ppfAudioOutputData, iNumFrames);
    }
    cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;
    
    
    //////////////////////////////////////////////////////////////////////////////
    // cleaning up
    
    pcVibrato->~Vibrato();
    phAudioInputFile->closeFile();
    phAudioOutputFile->closeFile();
    CAudioFileIf::destroy(phAudioInputFile);
    CAudioFileIf::destroy(phAudioOutputFile);
    
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete[] ppfAudioInputData[i];
        delete[] ppfAudioOutputData[i];
    }
    
    delete[] ppfAudioInputData;
    delete[] ppfAudioOutputData;
    
    ppfAudioInputData = 0;
    ppfAudioOutputData = 0;
    
    return 1;
}

void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout << endl;
    
    return;
}
