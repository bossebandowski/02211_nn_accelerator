# 02211_nn_accelerator
A neural network accelerator for the patmos processor, implemented in chisel

## How to clone repository
1; sudo rm -r t-crest/patmos

2; Clone the repository directly into the t-crest folder without creating subdirectory

## Description of repository folders
patmos: contains everything related to development for patmos. This must be the base directory of all Patmos-related commands
Chisel source files has to be defined in patmos/hardware/src/main/scala/ and the IO device has to be defined in the .xml config file of the emulated FPGA
 
### testing
In order to run our predefined tests follow the following instructions:

1; cd t-crest/patmos

2; make clean

3; make tools

4; make emulator

5; make comp APP=accelerator_main

6; patemu tmp/accelerator_main.elf

