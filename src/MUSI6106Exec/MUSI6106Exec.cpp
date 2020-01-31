
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

void    showClInfo();
int     applyFilter(std::string sInputFilePath, std::string sOutputFilePath, std::string sFilterType, float sMaxFilterDelay, float sFilterDelay, float sFilterGain, int numBlocks);
void t1();
void t2();
void t3();
void t4();
void t5();

int main(int argc, char* argv[]) {
    std::string     sInputFilePath,                 //!< file paths
                    sOutputFilePath,
                    sFilterType;

    float           sFilterDelay,                  //!< filter params
                    sFilterGain,
                    sMaxFilterDelay;

    static const int kBlockSize = 1024;
    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2) {
        //run test cases
//        t1();
//        t2();
//        t3();
//        t4();
        t5();

    }
    if (argc < 6) {
        cout << "Missing paramters, try again!";

        return -1;
    } else {
        sInputFilePath = argv[1];
        sOutputFilePath = sInputFilePath + "_processed.wav";

        sFilterType = argv[2];
        sMaxFilterDelay = std::stof(argv[3]);
        sFilterDelay = std::stof(argv[4]);
        sFilterGain = std::stof(argv[5]);
    }
    return applyFilter(sInputFilePath, sOutputFilePath, sFilterType, sMaxFilterDelay, sFilterDelay, sFilterGain, 1024);
}

int applyFilter(std::string sInputFilePath, std::string sOutputFilePath, std::string sFilterType, float sMaxFilterDelay, float sFilterDelay, float sFilterGain, int blockSize)
{
    // declare variables
    CAudioFileIf                *phAudioInputFile = 0;          //!< audio file objects
    CAudioFileIf                *phAudioOutputFile = 0;
    CAudioFileIf::FileSpec_t    stFileSpec;                     //!< file specs

    CCombFilterIf               *pcCombFilter = 0;                   //!< comb filter
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
    while (!phAudioInputFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioInputFile->readData(ppfAudioInputData, iNumFrames);
        cout << "\r" << "reading and writing";
        pcCombFilter->process(ppfAudioInputData, ppfAudioOutputData, (int)iNumFrames);
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

    return 1;
}

void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout << endl;

    return;
}

//////////////////////////////////////////////////////////////////////////////
// FIR: Output is zero if input freq matches feedforward
void     t1()
{
    cout << "Test 1: FIR: Output is zero if input freq matches feedforward" << endl;
    CAudioFileIf* phAudioOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    float** ppfAudioOutputData = 0;

    std::string sInputFilePath = "/Users/sandeepdasari/Downloads/2020-MUSI6106-ass1_stub/sine_441.wav";
    std::string sOutputFilePath = sInputFilePath + "_processed1.wav";

    // sending a sine wave of 441 Hz with 0 delay and -1 gain
    applyFilter(sInputFilePath, sOutputFilePath, "FIR", 0.1, 0, -1, 1024);

    cout<< "Done applying filters.. Begin test 2"<<endl;

    //////////////////////////////////////////////////////////////////////////////
    // let's re-open wav files and compare
    phAudioOutput->create(phAudioOutput);
    phAudioOutput->openFile(sOutputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioOutput->isOpen()) { //check
        cout << "Error: Cannot open, output file already open.";
        return ;
    }


    phAudioOutput->getFileSpec(stFileSpec); //get file specs
    // make buffers to compare
    ppfAudioOutputData = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        ppfAudioOutputData[i] = new float[1024];
    }

    while (!phAudioOutput->isEof())
    {
        long long iNumFrames = 1024;
        phAudioOutput->readData(ppfAudioOutputData, iNumFrames);
        for (int c = 0; c < stFileSpec.iNumChannels; c++) //compare the two arrays
        {
            for (int i = 0; i < iNumFrames; i++)
            {
                if ((ppfAudioOutputData[c][i]-0) > 0.01)
                {
                    cout << "Test 1 failed "<< ppfAudioOutputData[c][i] <<endl;
                    return;
                }
            }
        }
    }
    cout << "Test 1 passed!"<<endl;

    // cleaning up
    phAudioOutput->closeFile();

    CAudioFileIf::destroy(phAudioOutput);

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete[] ppfAudioOutputData[i];
    }

    delete[] ppfAudioOutputData;
    ppfAudioOutputData = 0;

    return;
}

//////////////////////////////////////////////////////////////////////////////
// IIR: amount of magnitude increase/decrease if input freq matches feedback
void     t2()
{
    cout << "Test 2:IIR: amount of magnitude increase/decrease if input freq matches feedback" << endl;

    CAudioFileIf* phAudioInput = 0;
    CAudioFileIf* phAudioOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    float** ppfAudioInputData = 0;
    float** ppfAudioOutputData = 0;

    std::string sInputFilePath = "/Users/sandeepdasari/Downloads/2020-MUSI6106-ass1_stub/sine_441.wav";
    std::string sOutputFilePath = sInputFilePath + "_processed1.wav";

    // sending a sine wave of 440 Hz with 0.1 feedback delay
    applyFilter(sInputFilePath, sOutputFilePath, "IIR", 0.1, 0, -1, 1024);

    cout<< "Done applying filters.. Begin test 2"<<endl;

    //////////////////////////////////////////////////////////////////////////////
    // let's re-open wav files and compare
    phAudioInput->create(phAudioInput);
    phAudioInput->openFile(sOutputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioInput->isOpen()) { //check
        cout << "Error: Cannot open, output file already open.";
        return ;
    }

    phAudioOutput->create(phAudioOutput);
    phAudioOutput->openFile(sOutputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioOutput->isOpen()) { //check
        cout << "Error: Cannot open, output file already open.";
        return ;
    }


    phAudioInput->getFileSpec(stFileSpec); //get file specs
    // make buffers to compare
    ppfAudioInputData = new float* [stFileSpec.iNumChannels];
    ppfAudioOutputData = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        ppfAudioInputData[i] = new float[1024];
        ppfAudioOutputData[i] = new float[1024];
    }

    while ((!phAudioInput->isEof()) || (!phAudioOutput->isEof()))
    {
        long long iNumFrames = 1024;
        phAudioInput->readData(ppfAudioInputData, iNumFrames);
        phAudioOutput->readData(ppfAudioOutputData, iNumFrames);
        for (int c = 0; c < stFileSpec.iNumChannels; c++) //compare the two arrays
        {
            for (int i = 0; i < iNumFrames; i++)
            {
                cout << "Test 2 incomplete: Failed" <<endl;
                return;
            }
        }
    }
    cout << "Test 2 passed!"<<endl;

    // cleaning up
    phAudioInput->closeFile();
    phAudioOutput->closeFile();

    CAudioFileIf::destroy(phAudioInput);
    CAudioFileIf::destroy(phAudioOutput);

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete[] ppfAudioInputData[i];
        delete[] ppfAudioOutputData[i];
    }

    delete[] ppfAudioInputData;
    delete[] ppfAudioOutputData;

    ppfAudioInputData = 0;
    ppfAudioOutputData = 0;

    return;
}
//////////////////////////////////////////////////////////////////////////////
// FIR/IIR: correct output result for VARYING input block size
void     t3()
{
    cout << "Test 3:correct output result for VARYING input block size " << endl;

    CAudioFileIf* phAudioOutputFile1 = 0;
    CAudioFileIf* phAudioOutputFile2 = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    float** ppfAudioOutputData1 = 0;
    float** ppfAudioOutputData2 = 0;

    std::string sInputFilePath = "/Users/sandeepdasari/Downloads/2020-MUSI6106-ass1_stub/vendetta.wav";
    std::string sOutputFilePath1 = sInputFilePath + "_processed1.wav";
    std::string sOutputFilePath2 = sInputFilePath + "_processed2.wav";

    // writes to phAudioOutputFile1 with 1024 blocksize
    applyFilter(sInputFilePath, sOutputFilePath1, "FIR", 5, 0.4, 0.7, 1024);

    // writes to phAudioOutputFile1 with 512 blocksize
    applyFilter(sInputFilePath, sOutputFilePath2, "FIR", 5, 0.4, 0.7, 512);

    cout<< "Done applying filters.. Begin test to compare output wavs"<<endl;


    //////////////////////////////////////////////////////////////////////////////
    // let's re-open wav files and compare
    phAudioOutputFile1->create(phAudioOutputFile1);
    phAudioOutputFile1->openFile(sOutputFilePath1, CAudioFileIf::kFileRead);
    if (!phAudioOutputFile1->isOpen()) { //check
        cout << "Error: Cannot open, output file already open.";
        return ;
    }

    phAudioOutputFile2->create(phAudioOutputFile2);
    phAudioOutputFile2->openFile(sOutputFilePath2, CAudioFileIf::kFileRead);
    if (!phAudioOutputFile1->isOpen()) { //check
        cout << "Error: Cannot open, output file already open.";
        return ;
    }


    phAudioOutputFile1->getFileSpec(stFileSpec); //get file specs
    // make buffers to compare
    ppfAudioOutputData1 = new float* [stFileSpec.iNumChannels];
    ppfAudioOutputData2 = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        ppfAudioOutputData1[i] = new float[1024];
        ppfAudioOutputData2[i] = new float[1024];
    }

    while ((!phAudioOutputFile1->isEof()) || (!phAudioOutputFile2->isEof()))
    {
        long long iNumFrames = 1024;
        phAudioOutputFile1->readData(ppfAudioOutputData1, iNumFrames);
        phAudioOutputFile2->readData(ppfAudioOutputData2, iNumFrames);
        for (int c = 0; c < stFileSpec.iNumChannels; c++) //compare the two arrays
        {
            for (int i = 0; i < iNumFrames; i++)
            {
                if (ppfAudioOutputData1[c][i] != ppfAudioOutputData2[c][i])
                {
                    cout << "Test 3 failed, buffers were different for different block sizes" << endl;
                    return;
                }
            }
        }
    }
    cout << "Test 3 passed, same .wav file for different block sizes"<<endl;

    // cleaning up
    phAudioOutputFile1->closeFile();
    phAudioOutputFile2->closeFile();

    CAudioFileIf::destroy(phAudioOutputFile1);
    CAudioFileIf::destroy(phAudioOutputFile2);

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete[] ppfAudioOutputData1[i];
        delete[] ppfAudioOutputData2[i];
    }

    delete[] ppfAudioOutputData1;
    delete[] ppfAudioOutputData2;

    ppfAudioOutputData1 = 0;
    ppfAudioOutputData2 = 0;

    return;
}

//////////////////////////////////////////////////////////////////////////////
// FIR/IIR: correct processing for zero input signal
void     t4()
{
    cout << "Test 4 :correct processing for zero input signal " << endl;

    CAudioFileIf* phZeroOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    float** ppfZeroOutputData = 0;

    std::string sInputFilePath = "/Users/sandeepdasari/Downloads/2020-MUSI6106-ass1_stub/vendetta.wav";
    std::string sOutputFilePath = sInputFilePath + "_processed1.wav";

    applyFilter(sInputFilePath, sOutputFilePath, "FIR", 5, 0.4, 0.7, 1024);
    cout<< "Done applying filter.. Begin test 4 to check for zero output signal"<<endl;

    //////////////////////////////////////////////////////////////////////////////
    // let's re-open wav files and compare
    phZeroOutput->create(phZeroOutput);
    phZeroOutput->openFile(sOutputFilePath, CAudioFileIf::kFileRead);
    if (!phZeroOutput->isOpen()) { //check
        cout << "Error: Cannot open, output file already open.";
        return ;
    }


    phZeroOutput->getFileSpec(stFileSpec); //get file specs
    ppfZeroOutputData = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        ppfZeroOutputData[i] = new float[1024];
    }

    while ((!phZeroOutput->isEof()) )
    {
        long long iNumFrames = 1024;
        phZeroOutput->readData(ppfZeroOutputData, iNumFrames);
        for (int c = 0; c < stFileSpec.iNumChannels; c++) //compare the two arrays
        {
            for (int i = 0; i < iNumFrames; i++)
            {
                if (ppfZeroOutputData[c][i] > 0.2)
                {
                    cout<<ppfZeroOutputData[c][i]<<endl;
                    cout << "Test 4 failed, buffer has non zero signal" << endl;
                    return;
                }
            }
        }
    }
    cout << "Test 4 passed, however it fails below 0.2!"<<endl;

    // cleaning up
    phZeroOutput->closeFile();

    CAudioFileIf::destroy(phZeroOutput);

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete[] ppfZeroOutputData[i];
    }

    delete[] ppfZeroOutputData;
    ppfZeroOutputData = 0;

    return;
}

//////////////////////////////////////////////////////////////////////////////
// FIR/IIR: Test for incorrect processing when delay param is greather than max delay
void     t5()
{
    cout << "Test 5: Test for incorrect processing when delay param is greather than max delay" << endl;
    CAudioFileIf* phAudioOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    float** ppfAudioOutputData = 0;

    std::string sInputFilePath = "/Users/sandeepdasari/Downloads/2020-MUSI6106-ass1_stub/sine_441.wav";
    std::string sOutputFilePath = sInputFilePath + "_processed1.wav";

    // sending a sine wave of 441 Hz with 0 delay and -1 gain
    applyFilter(sInputFilePath, sOutputFilePath, "FIR", 0.1, 0.2, -1, 1024);
    return;
}