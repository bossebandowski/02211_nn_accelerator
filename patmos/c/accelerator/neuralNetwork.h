#include "parameters.h"

typedef enum ACCELERATOR_STATE {IDLE, LOAD_NN, ERROR}ACCELERATOR_STATE; //TODO: add other states
// IO device addresses
volatile _IODEV int *IO_PTR_ACC = (volatile _IODEV int *) 0xf00c0000;

void fillNeuralNetwork()
{
    //TODO: Check the sequence
    for(int i = 0; i < 79400; i++)
    {
        *IO_PTR_ACC = weights[i];
    }
    for(int i = 0; i < 79400; i++)
    {
        *IO_PTR_ACC = biases[i];
    }
    for(int i = 0; i < 784; i++)
    {
        *IO_PTR_ACC = picture[i];
    }
}

ACCELERATOR_STATE getNeuralNetworkStatus()
{
    int regVal = *IO_PTR_ACC;
    if(regVal != IDLE && regVal != LOAD_NN)
    {
        return ERROR;
    }
    return (ACCELERATOR_STATE) regVal;
}