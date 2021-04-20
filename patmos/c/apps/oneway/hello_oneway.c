/*
  Small test program for the One-Way Shared Memory.

  Fills TX memories with core specific patterns and reads out
  the RX memory of core 0 to see which blocks of data come from where.

  Author: Martin Schoeberl
*/

#include <stdio.h>
#include <machine/spm.h>
#include "../../libcorethread/corethread.h"

#define CNT 4
#define WORDS 256

// The main function for the other threads on the other cores
void work(void* arg) {

  _iodev_ptr_t txMem = (_iodev_ptr_t) PATMOS_IO_ONEWAYMEM;

  int id = get_cpuid();
  for (int i=0; i<CNT; ++i) {
    for (int j=0; j<WORDS; ++j) {
      txMem[i*WORDS +j] = id*0x10000 + i*0x100 + j;
    }
  }
}

int main() {

  _iodev_ptr_t rxMem = (_iodev_ptr_t) PATMOS_IO_ONEWAYMEM;

  for (int i=1; i<get_cpucnt(); ++i) {
    corethread_create(i, &work, NULL); 
  }

  printf("Number of cores: %d\n", get_cpucnt());
  for (int i=0; i<CNT; ++i) {
    for (int j=0; j<4; ++j) {
      printf("%08x\n", rxMem[i*WORDS + j]);
    }
  }
  return 0;
}
