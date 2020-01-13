
#include <iostream>
#include <fstream>
#include <ctime>

#include "MUSI6106Config.h"
#include "AudioFileIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float                   **ppfAudioData = 0;

    CAudioFileIf            *phAudioFile = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;
    
    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    sInputFilePath = argv[1];
    sOutputFilePath = argv[2];
    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    
    stFileSpec.eFormat = CAudioFileIf::FileFormat_t::kFileFormatWav;
    stFileSpec.eBitStreamType = CAudioFileIf::BitStream_t::kFileBitStreamInt16;
    stFileSpec.iNumChannels = 2;
    stFileSpec.fSampleRateInHz = 44100.0;
    
    CAudioFileIf::create(phAudioFile);
    
    phAudioFile->openFile(sInputFilePath,CAudioFileIf::FileIoType_t::kFileRead , &stFileSpec);
    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    cout<< sOutputFilePath <<endl;
    hOutputFile.open(sOutputFilePath, std::fstream::out);
    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    long long lenSec;
    phAudioFile->getLength(lenSec);
    
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output text file (one column per channel)
    
    
    //////////////////////////////////////////////////////////////////////////////
    // clean-up (close files and free memory)
    CAudioFileIf::destroy(phAudioFile);
    hOutputFile.close();
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

