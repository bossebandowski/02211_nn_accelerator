#include <machine/patmos.h>
#include <stdio.h>
#include <unistd.h>
#include "accelerator/neuralNetwork.h"
#include "accelerator/counter.h"

void memoryReadAndWrite8BitTest()
{
    for(int8_t i = -110; i < 110; i++)
    {
        ADR_ACCELERATOR_FILL = i;
    }
    printf("220 value have written to memory\n");
    int errors = 0;
    for(uint32_t i = -110; i < 110; i++)
    {
        int8_t readed = ADR_ACCELERATOR_MEMORY_TEST_READ_8;
        if(readed != i)
        {
            printf("Expected: %d, Readed: %d\n",i, readed);
            errors ++;
        }
    }
    printf("8 bit tests finished. %d erros\n", errors);

    printf("Stating 32 bit tests...\n");
    for(int32_t i = 0; i < 110; i ++)
    {
        ADR_ACCELERATOR_FILL_32 = i;
    }
    printf("20000 value have written to memory\n");
    errors = 0;
    for(int32_t i = 0; i < 110; i ++)
    {
        int32_t readed = ADR_ACCELERATOR_MEMORY_TEST_READ_32;
        if(readed != i)
        {
            printf("Expected: %d, Readed: %d\n",i, readed);
            errors ++;
        }
    }
}

int main() 
{
    printf("Program started\n");
    cntReset();
    memoryReadAndWrite8BitTest();
    // Transfer data from parameters.h
    //fillNeuralNetwork();

    printf("Elapsed time: %d clock cycles, %f micros\n", cntRead(), cntReadMicros());

    printf("Waiting for result\n");
    while(getNeuralNetworkStatus != (ACCELERATOR_STATE) READY)
    {
        sleep(100);
    }
}

void stateTransitionTest()
{
    int val;
    cntReset();
    val = *IO_PTR_ACC;
    printf("Counter test >>> Elapsed time: %d clock cycles, %f micros\n", cntRead(), cntReadMicros());
    printf("expected: 15; got: %d\n", val);
    // first write: transition to loadnn
    *IO_PTR_ACC = 1;
    val = *IO_PTR_ACC;
    printf("expected: 1; got: %d\n", val);
    *IO_PTR_ACC = 0;
    val = *IO_PTR_ACC;
    printf("got: %d\n", val);
    *IO_PTR_ACC = 0;
    val = *IO_PTR_ACC;
    printf("got: %d\n", val);
    *IO_PTR_ACC = 0;
    val = *IO_PTR_ACC;
    printf("got: %d\n", val);
    // 200k writes to transition to idle state
    cntReset();
    for (int i = 0; i < 200000; i++) {
        *IO_PTR_ACC = 0;
    }
    printf("Parameter transfer took %d clock cycles\n", cntRead());
    val = *IO_PTR_ACC;
    printf("got: %d\n", val);   
}
