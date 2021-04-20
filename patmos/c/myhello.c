
#include <machine/spm.h>

int main() {

  volatile _SPM int *uart_status = (volatile _SPM int *) 0xF0080000;
  volatile _SPM int *uart_data = (volatile _SPM int *) 0xF0080004;

  *uart_data = 'H';
}
