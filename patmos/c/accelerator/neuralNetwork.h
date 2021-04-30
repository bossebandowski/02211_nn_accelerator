#include "parameters.h"

typedef enum ACCELERATOR_STATE {IDLE, LOAD_NN, ERROR, READY}ACCELERATOR_STATE; //TODO: add other states

// IO device addresses
volatile _IODEV int *IO_PTR_ACC = (volatile _IODEV int *) 0xf00c0000;

#define ADR_ACCELERATOR_BASE                        0xf00c0000

#define ADR_ACCELERATOR_FILL                        *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0x0))
#define ADR_ACCELERATOR_STATUS                      *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0x4))
#define ADR_ACCELERATOR_MEMORY_TEST_READ            *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0xC))
#define ADR_ACCELERATOR_SET_MEM_ADDR                *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0x10))


void fillNeuralNetwork()
{
    // transition to network load state
    ADR_ACCELERATOR_FILL = 0;

    
    printf("transitioned to state: %d \n", ADR_ACCELERATOR_STATUS);

    printf("Parameter transfer to accelerator has started\n");
    //printf("Transfering image\n");
    /*for(int i = 0; i < 784; i++)
    {
        ADR_ACCELERATOR_FILL = picture[i];
    }*/
    //printf("Image transferred successfully\n");
    printf("Transfering weights\n");
    for(int i = 0; i < 78400; i++)
    {
        ADR_ACCELERATOR_FILL = weights_1[i];
        /*if((i+1) % 10000 ==0)
        {
            printf("%d/79400 weight transferred\n",i + 1);
        }*/
    }
    for(int i = 0; i < 1000; i++)
    {
        ADR_ACCELERATOR_FILL = weights_2[i];
        /*if((i+1) % 10000 ==0)
        {
            printf("%d/79400 weight transferred\n",i + 1);
        }*/
    }
    printf("Weights transferred successfully\n");
    printf("Transfering biases\n");
    for(int i = 0; i < 100; i++)
    {
        ADR_ACCELERATOR_FILL = biases_1[i];
        /*if((i+1) % 10000 ==0)
        {
            printf("%d/79400 bias transferred\n",i + 1);
        }*/
    }
    for(int i = 0; i < 10; i++)
    {
        ADR_ACCELERATOR_FILL = biases_2[i];
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
