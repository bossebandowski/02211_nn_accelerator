#include "parameters.h"

typedef enum ACCELERATOR_STATE {IDLE, LOAD_NN, ERROR, READY}ACCELERATOR_STATE; //TODO: add other states

// IO device addresses
volatile _IODEV int *IO_PTR_ACC = (volatile _IODEV int *) 0xf00c0000;

#define ADR_ACCELERATOR_BASE            0xf00c0000

#define ADR_ACCELERATOR_FILL            *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0x0))
#define ADR_ACCELERATOR_STATUS          *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0x4))


void fillNeuralNetwork()
{
    printf("Parameter transfer to accelerator has started\n");
    printf("Transfering image\n");
    for(int i = 0; i < 784; i++)
    {
        ADR_ACCELERATOR_FILL = picture[i];
        /*if((i+1) % 100 ==0)
        {
            printf("%d/784 pixel transferred\n",i + 1);
        }*/
    }
    printf("Image transferred successfully\n");
    printf("Transfering weights\n");
    for(int i = 0; i < 79400; i++)
    {
        ADR_ACCELERATOR_FILL = weights[i];
        /*if((i+1) % 10000 ==0)
        {
            printf("%d/79400 weight transferred\n",i + 1);
        }*/
    }
    printf("Weights transferred successfully\n");
    printf("Transfering biases\n");
    for(int i = 0; i < 79400; i++)
    {
        ADR_ACCELERATOR_FILL = biases[i];
        /*if((i+1) % 10000 ==0)
        {
            printf("%d/79400 bias transferred\n",i + 1);
        }*/
    }
    printf("Biases transferred successfully\n");
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