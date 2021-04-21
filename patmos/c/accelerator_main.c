#include <machine/patmos.h>
#include <stdio.h>
#include <unistd.h>
#include "accelerator/neuralNetwork.h"
#include "accelerator/counter.h"

int main() {
    int val;
    // Testing state machine
    // init: 15 in output_reg, nonn
    val = *IO_PTR_ACC;
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
    for (int i = 0; i < 200000; i++) {
        *IO_PTR_ACC = 0;
    }
    val = *IO_PTR_ACC;
    printf("got: %d\n", val);
     
}
