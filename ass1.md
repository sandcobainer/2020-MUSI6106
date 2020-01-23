In this assignment, you are given a pure virtual base class CCombFilterIf. Given this interface, you implement both parametric FIR and IIR comb filters and call the filters from the main function, which reads audio, processes the data in a loop, and writes audio. The deliverable is a link to your github repository (link to the correct branch/time of submission).

Update you project from the branch ass1_stub (Links to an external site.)

[10] Design your interface and code structure
Inspired by CAudioFileIf/CAudioFile, create the files CombFilter.h/.cpp in the appropriate folder locations and populate them with your classes/functions etc.
[20] Implement an FIR comb filter (DAFX: fircomb.m, see M_files_chap02.zip)
[20] Implement an IIR comb filter (DAFX: iircomb.m, see M_files_chap02.zip)
[15] Modify the main function
The command line application instantiates your class with the correct parameters, read the parameters from the command line (file name, effect-parameters), reads and processes audio data in blocks and writes the result to an output wav file.
[5] Real-world example
Process two short audio files of your choice and verify your results by running the effect in Matlab and computing the difference between outputs. Add the plots to a new file assignment1.md in your repository
[30] Add tests to your main function
If your main is called without command line arguments, automatically run tests verifying your implementation. Implement the following test (a function each):
FIR: Output is zero if input freq matches feedforward
IIR: amount of magnitude increase/decrease if input freq matches feedback
FIR/IIR: correct result for VARYING input block size
FIR/IIR: correct processing for zero input signal
One more additional MEANINGFUL test to verify your filter implementation
