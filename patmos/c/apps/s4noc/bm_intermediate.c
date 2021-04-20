/*
  Producer/intermediate/consumer benchmark for the S4NOC paper.
  Trim the DELAY of the producer until no tokens are lost.

  Author: Martin Schoeberl and Luca Pezzarossa
*/

#include <stdio.h>
#include <machine/spm.h>
#include "../../libcorethread/corethread.h"

#include "s4noc.h"

#define PRODUCER_CORE 1
#define INTERMEDIATE_CORE 8
#define CONSUMER_CORE 3
#define SEND_SLOT_PRODU_TO_INTER 0
#define SEND_SLOT_INTER_TO_CONSU 0

//#define LEN  20 // in words
//#define BUF_LEN 16 // in words

volatile _UNCACHED int started_producer;
volatile _UNCACHED int started_intermediate;
volatile _UNCACHED int started_consumer;
volatile _UNCACHED int finished_producer;
volatile _UNCACHED int finished_intermediate;
volatile _UNCACHED int finished_consumer;
volatile _UNCACHED int end_flag;
volatile _UNCACHED int result;
volatile _UNCACHED int time;
//volatile _UNCACHED int array[LEN];

void consumer(void* arg) {

  volatile _SPM int *s4noc = (volatile _SPM int *) (S4NOC_ADDRESS);
  int sum = 0;

  // Get started
  started_consumer = 1;

  //for (int i=0; i<LEN; ++i) {
  for (int i=0; i<LEN/BUF_LEN; ++i) {
    for (int j=0; j<BUF_LEN; ++j) {
      while (!s4noc[RX_READY]) {;}
      sum += s4noc[IN_DATA];
      //array[i*BUF_LEN + j] = s4noc[IN_DATA];
      //array[i] = s4noc[IN_DATA];
    }
  }
  time = *timer_ptr - time;
  result = sum;
  finished_consumer = 1;
  // Join threads
  int ret = 0;
	corethread_exit(&ret);
	return;
}


void producer(void* arg) {

  volatile _SPM int *s4noc = (volatile _SPM int *) (S4NOC_ADDRESS);
  int val = 0;
  
  // Get started
  started_producer = 1;

  // Give the other threads some head start to be ready (0.1s)
  *dead_ptr = 8000000;
  val = *dead_ptr;

  // Start timing
  time = *timer_ptr;

  for (int i=0; i<LEN/BUF_LEN; ++i) {
    for (int j=0; j<BUF_LEN; ++j) {
      while (!s4noc[TX_FREE]) {;}
      *dead_ptr = DELAY/2;
      val = *dead_ptr;
      s4noc[SEND_SLOT_PRODU_TO_INTER] = 1;
      //s4noc[SEND_SLOT_PRODU_TO_INTER] = i*BUF_LEN + j;      
      *dead_ptr = DELAY/2;
      val = *dead_ptr;
    }
  }
  
  finished_producer = 1;

  while (end_flag==0) {
    while (!s4noc[TX_FREE]) {;}
    *dead_ptr = DELAY/2;
    val = *dead_ptr;
    s4noc[SEND_SLOT_PRODU_TO_INTER] = 0;
    *dead_ptr = DELAY/2;
    val = *dead_ptr;
  }

  // Join threads
  int ret = 0;
	corethread_exit(&ret);
	return;
}

void intermediate(void* arg) {

  volatile _SPM int *s4noc = (volatile _SPM int *) (S4NOC_ADDRESS);
  volatile _SPM int *spm_base = (volatile _SPM int *) 0x00000000;
  volatile _SPM int *receive_A = &spm_base[0];
  volatile _SPM int *send_A = &spm_base[BUF_LEN];
  volatile _SPM int *receive_B = &spm_base[2*BUF_LEN];
  volatile _SPM int *send_B = &spm_base[3*BUF_LEN];
  volatile _SPM int *receiving;
  volatile _SPM int *sending;
  volatile _SPM int *working_input;
  volatile _SPM int *working_output;
  volatile _SPM int *tmp_input;
  volatile _SPM int *tmp_output;
  int val = 0;

  receiving = receive_A;
  sending = send_A;
  working_input = receive_B;
  working_output = send_B;
  
  // Get started
  started_intermediate=1;
  
  // Fill up
  for (int j=0; j<BUF_LEN; ++j) {
    while (!s4noc[RX_READY]) {;}
    receive_A[j] = s4noc[IN_DATA];
  }
  // Fill up
  for (int j=0; j<BUF_LEN; ++j) {
    send_A[j] = receive_A[j]; // Processing
    while (!s4noc[RX_READY]) {;}
    receive_B[j] = s4noc[IN_DATA];

  }
  // Continue
  for (int i=0; i<LEN/BUF_LEN-2; ++i) {
    for (int j=0; j<BUF_LEN; ++j) {
      working_output[j] = working_input[j]; // Processing
      while (!s4noc[RX_READY]) {;}
      receiving[j] = s4noc[IN_DATA];
      while (!s4noc[TX_FREE]) {;}
      s4noc[SEND_SLOT_INTER_TO_CONSU] = sending[j];
    }
    tmp_output = working_output;
    tmp_input = working_input;
    working_output = sending;
    working_input = receiving;
    sending = tmp_output;
    receiving = tmp_input;
  }
  // Flush out
  for (int j=0; j<BUF_LEN; ++j) {
    send_B[j] = receive_B[j]; // Processing
    while (!s4noc[TX_FREE]) {;}
    s4noc[SEND_SLOT_INTER_TO_CONSU] = send_A[j];

  }
  // Flush out
  for (int j=0; j<BUF_LEN; ++j) {
    while (!s4noc[TX_FREE]) {;}
    s4noc[SEND_SLOT_INTER_TO_CONSU] = send_B[j];
  }  
    
  finished_intermediate=1;

  while (end_flag==0) {
    while (!s4noc[TX_FREE]) {;}
    s4noc[SEND_SLOT_INTER_TO_CONSU] = 0;
  }
  
  // Join threads
  int ret = 0;
	corethread_exit(&ret);
	return;
}

// The main acts as producer
int main() {

  int val = 0;

  end_flag = 0;
  result = 0;
  started_producer = 0;
  started_intermediate = 0;
  started_consumer = 0;
  finished_producer = 0;
  finished_intermediate = 0;
  finished_consumer = 0;

  printf("Producer/intermediate/consumer benchmark for the S4NOC paper:\n");
  printf("  Delay: %d\n", DELAY);
  printf("  Number of cores: %d\n", get_cpucnt());
  printf("  Total packets sent: %d\n", LEN);
  printf("  Buffer size: %d\n", BUF_LEN);

  printf("Runnning test:\n");
  corethread_create(CONSUMER_CORE, &consumer, NULL);
  while(started_consumer == 0) {;}
  printf("  Consumer is ready.\n");

  corethread_create(INTERMEDIATE_CORE, &intermediate, NULL);
  while(started_intermediate == 0) {;}
  printf("  Intermediate is ready.\n");

  corethread_create(PRODUCER_CORE, &producer, NULL);
  while(started_producer == 0) {;}
  printf("  Producer has started.\n");
  
  while(finished_producer == 0) {;}
  printf("  Producer has finished.\n");
  
  while(finished_intermediate == 0) {;}
  printf("  Intermediate has finished.\n");

  while(finished_consumer == 0) {;}
  printf("  Consumer has finished.\n");
    
  *dead_ptr = 8000000;
  val = *dead_ptr;
  
  printf("Results: \n");
  printf("  %d valid pakets out of of %d received.\n", result, LEN); 
  printf("  Reception time of %d cycles -> %g cycles per received packet.\n", time, 1. * time/LEN);
  
  // Join threads
  int *retval;
  end_flag = 1;
  corethread_join(PRODUCER_CORE, (void **)&retval);
  corethread_join(INTERMEDIATE_CORE, (void **)&retval);
  corethread_join(CONSUMER_CORE, (void **)&retval);
  
  //for (int i=0; i<LEN/BUF_LEN; ++i) {
  //  for (int j=0; j<BUF_LEN; ++j) {
  //    printf("Array[%d] = %d;  ", i*BUF_LEN + j, array[i*BUF_LEN + j]);
  //  }
  // }

  printf("End of program.\n");
  return val;  
}















