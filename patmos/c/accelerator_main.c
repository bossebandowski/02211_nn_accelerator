#include <machine/patmos.h>
#include <stdio.h>
#include <unistd.h>
int main() 
{ 
    volatile _IODEV int *io_ptr_acc = (volatile _IODEV int *) 0xf00c0000;
    volatile _IODEV int *io_ptr_cnt = (volatile _IODEV int *) 0xf00b0000;
    int val;
    
    val = *io_ptr_cnt;
    printf("%d\n", val);
    for(int i =0; i <10;i++)
    {
        val = *io_ptr_cnt;
        printf("%d\n", val);
    }
    
    val = *io_ptr_acc;
    printf("%d\n", val);
    for(int i =0; i <10;i++)
    {
        val = *io_ptr_acc;
        printf("%d\n", val);
    }
}
