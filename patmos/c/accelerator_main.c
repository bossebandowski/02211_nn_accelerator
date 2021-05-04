#include <machine/patmos.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "accelerator/neuralNetwork.h"

void loadNetworkCheck() {
    printf("init state: %d \n", ADR_ACCELERATOR_STATUS);

    fillNeuralNetwork(true);

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

    
    printf("==================\n");
    printf("network check done\n");
    printf("==================\n");

}

void loadImg(bool v) {
    // transition to infload
    ADR_ACCELERATOR_INPUT = 1;
    if (v) {
        printf("inf load state: %d \n", ADR_ACCELERATOR_STATUS);
    }
    for(int i = 0; i < 784; i++) {
        ADR_ACCELERATOR_INPUT = picture[i];
    }
}

void loadInfCheck() {
    // check initial state (should be idle after network load, otherwise nonn)
    printf("init state: %d \n", ADR_ACCELERATOR_STATUS);
    // check state (should be infload)
    printf("loading img...\n");
    loadImg(true);

    printf("done\n");

    for (int i = 0; i < 30; i++) {
        printf("Expected: %d, Read: %d \n", picture[i*20], readFromMem(i*20));
    }

    printf("Expected: %d, Read: %d \n", picture[783], readFromMem(783));


    printf("==================\n");
    printf("img check done\n");
    printf("==================\n");
    printf("state after inf load: %d \n", ADR_ACCELERATOR_STATUS);
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
    

    // loadNetworkCheck();
    // loadInfCheck();

    fillNeuralNetwork(false);
    cntReset();
    loadImg(false);
    printf("Output register content: %d \n", ADR_ACCELERATOR_RESULT);
    printf("Output register content: %d \n", ADR_ACCELERATOR_RESULT);

    printf("Elapsed time: %d clock cycles, %f micros\n", cntRead(), cntReadMicros());

    printf("Result by patmos:%d\n", calculateNNCPU());

}
