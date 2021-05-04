#include "parameters.h"
#include "counter.h"

// IO device addresses
volatile _IODEV int *IO_PTR_ACC = (volatile _IODEV int *) 0xf00c0000;

#define ADR_ACCELERATOR_BASE                        0xf00c0000

#define ADR_ACCELERATOR_INPUT                       *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0x0))
#define ADR_ACCELERATOR_STATUS                      *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0x4))
#define ADR_ACCELERATOR_RESULT                      *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0x8))
#define ADR_ACCELERATOR_MEMORY_TEST_READ            *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0xC))
#define ADR_ACCELERATOR_SET_MEM_ADDR                *((volatile _IODEV unsigned int *) (ADR_ACCELERATOR_BASE + 0x10))




void fillNeuralNetwork(bool v) {
    if (v) {
        printf("Loading weights and biases into NN\n");
    }
    // transition to network load state
    ADR_ACCELERATOR_INPUT = 0;
    
    if (v) {
        printf("transitioned to state: %d \n", ADR_ACCELERATOR_STATUS);
        printf("Parameter transfer to accelerator has started\n");
        printf("Transfering weights\n");
    }
    
    for(int i = 0; i < 78400; i++)
    {
        ADR_ACCELERATOR_INPUT = weights_1[i];
    }
    
    for(int i = 0; i < 1000; i++)
    {
        ADR_ACCELERATOR_INPUT = weights_2[i];
    }
    if (v) {
        printf("Weights transferred successfully\n");
        printf("Transfering biases\n");
    }
    for(int i = 0; i < 100; i++)
    {
        ADR_ACCELERATOR_INPUT = biases_1[i];
    }
    
    for(int i = 0; i < 10; i++)
    {
        ADR_ACCELERATOR_INPUT = biases_2[i];
    }
    if (v) {
        printf("Biases transferred successfully\n");
    }
}

int calculateNNCPU()
{
    int layer1[100];
    int layer2[10];

    // Reset everything for safety (Does not count in execution time)
    for(int i = 0; i < 100; i++)
    {
        layer1[i] = 0;
    }
    for(int i = 0; i < 10; i++)
    {
        layer2[i] = 0;
    }

    //Execution time starts from here
    cntReset();
    // First layer
    // Calculate neuron values
    int weightIndex = 0;
    for(int i = 0; i < 100; i++)
    {
        for(int p = 0; p < 784; p++)
        {
            layer1[i] += picture[p] * weights_1[weightIndex];
            weightIndex ++;
        }
    }
    // Add biases
    for(int i = 0; i < 100; i++)
    {
        layer1[i] += biases_1[i];
    }

    // Second layer
    // Calculate neuron values
    weightIndex = 0;
    for(int i = 0; i < 10; i++)
    {
        for(int fl = 0; fl < 100; fl ++)
        {
            //printf("%d\n", layer1[i]);
            layer2[i] += layer1[fl] * weights_2[weightIndex];
            weightIndex ++;
        }
    }
    // Add biases
    for(int i = 0; i < 10; i++)
    {
        layer2[i] += biases_2[i];
    }

    // Find maximum
    int result = -1;
    for(int i = 0; i < 10; i++)
    {
        printf("%d\n", layer2[i]);
        if(layer2[i] >= result)
        {
            result = i;
        }
    }

    double micros = cntReadMicros();
    printf("Elapsed time: %f micros\n", micros);
    return result;
}