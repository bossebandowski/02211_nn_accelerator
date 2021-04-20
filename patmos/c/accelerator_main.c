#include <machine/patmos.h>
#include <stdio.h>
#include <unistd.h>
#include "accelerator/neuralNetwork.h"
#include "accelerator/counter.h"
int main() 
{ 
    // Just tester code, nothing important
    printf("ok");
    int val;
    val = *IO_PTR_CNT;
    printf("%d\n", val);
    for(int i =0; i <10;i++)
    {
        val = *IO_PTR_CNT;
        printf("%d\n", val);
    }

    val = *IO_PTR_CNT;
    printf("%d\n", val);
    for(int i =0; i <10;i++)
    {
        *IO_PTR_CNT = 15;
        val = *IO_PTR_CNT;
        printf("%d\n", val);
    }

    // Program will stuck here, because the device is not ready yet
    getNeuralNetworkStatus();
}
