#include <machine/patmos.h>
#include <stdio.h>
#include <unistd.h>
#include "accelerator/neuralNetwork.h"
#include "accelerator/counter.h"

int main() 
{
    printf("Program started\n");
    cntReset();
    // Transfer data from parameters.h
    fillNeuralNetwork();
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
