# 02211_nn_accelerator
A neural network accelerator for the patmos processor, implemented in chisel

## How to clone repository
1; sudo rm -r t-crest/patmos

2; Clone the repository directly into the t-crest folder without creating subdirectory

## Description of repository folders
patmos: contains everything related to development for patmos. This must be the base directory of all Patmos-related command
src: countains Chisel hardware codes. It is testable here. After making succesfull unittests the Chisel source files has to be copied into patmos/hardware/src/main/scala/ and the IO device has to be defined in the .xml config file of the emulated FPGA
 
### testing
`sbt test` to run all specs in `src/test/scala`

`sbt "testOnly [specName without .scala]"` to run single test bench

example:
`sbt "testOnly NeuronSpec"`

###Emulate patmos:
Add Chisel hardware source files to t-crest/patmos/hardware/src/main/scala/

Define the hardware in t-crest/patmos/hardware/config/altde2-115.xml according to the Patmos handbook

Add the C source code to t-crest/patmos/c/<PROGRAM_NAME>.c

`cd t-crest/patmos`

Build tools: `make tools`

Build emulator: `make emulator`

Compile .c program to .elf file: `make comp APP=<PROGRAM_NAME>`

OR

Compile .c program to .out file: `patmos-clang c/<PROGRAM_NAME>.c <PROGRAM_NAME>.out`

Emulate .elf running: `patemu tmp/<PROGRAM_NAME>`

OR

Emualte .out running: `patemu <PROGRAM_NAME>.out`

