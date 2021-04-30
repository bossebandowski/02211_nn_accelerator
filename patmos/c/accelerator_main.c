#include <machine/patmos.h>
#include <stdio.h>
#include <unistd.h>
#include "accelerator/neuralNetwork.h"
#include "accelerator/counter.h"

void loadNetworkCheck() {
    // transition to network load status
    printf("init state: %d \n", ADR_ACCELERATOR_STATUS);

    //cntReset();
    //memoryReadAndWrite8BitTest();
    fillNeuralNetwork();

    printf("state after network load: %d \n", ADR_ACCELERATOR_STATUS);
    
    int rsp = readFromMem(784);
    printf("Expected: %d, Read: %d \n",weights_1[0], rsp);
    rsp = readFromMem(10000);
    printf("Expected: %d, Read: %d \n",weights_1[9216], rsp);
    rsp = readFromMem(79183);
    printf("Expected: %d, Read: %d \n",weights_1[78399], rsp);
    rsp = readFromMem(79184);
    printf("Expected: %d, Read: %d \n",weights_2[0], rsp);
    rsp = readFromMem(79185);
    printf("Expected: %d, Read: %d \n",weights_2[1], rsp);
    rsp = readFromMem(80182);
    printf("Expected: %d, Read: %d \n",weights_2[998], rsp);
    rsp = readFromMem(80183);
    printf("Expected: %d, Read: %d \n",weights_2[999], rsp);
    rsp = readFromMem(80184);
    printf("Expected: %d, Read: %d \n",biases_1[0], rsp);
    rsp = readFromMem(80292);
    printf("Expected: %d, Read: %d \n",biases_2[8], rsp);
    rsp = readFromMem(80293);
    printf("Expected: %d, Read: %d \n",biases_2[9], rsp);

}

int readFromMem(int addr) {
    printf("checking value at address %d \n", addr);
    ADR_ACCELERATOR_SET_MEM_ADDR = addr;
    int val = ADR_ACCELERATOR_MEMORY_TEST_READ;
    val = ADR_ACCELERATOR_MEMORY_TEST_READ;
    return val;
}

int main() 
{
    printf("Program started\n");
    
    printf("Elapsed time: %d clock cycles, %f micros\n", cntRead(), cntReadMicros());

    loadNetworkCheck();

    printf("done");
    /*printf("Waiting for result\n");
    while(getNeuralNetworkStatus != (ACCELERATOR_STATE) READY)
    {
        sleep(100);
    }*/
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
