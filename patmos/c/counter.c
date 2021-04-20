#include <machine/patmos.h>
#include <stdio.h>
#include <unistd.h>
int main() 
{ 
    volatile _IODEV int *io_ptr = (volatile _IODEV int *) 0xf00b0000;
    volatile _IODEV int *io_ptr2 = (volatile _IODEV int *) 0xf00b0000;
    int val;
    val = *io_ptr;
    printf("%d\n", val);
    for(int i =0; i <10;i++)
    {
        val = *io_ptr;
        printf("%d\n", val);
    }
}
