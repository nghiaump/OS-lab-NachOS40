// Demo PrintString syscall
#include "syscall.h"

int
main()
{   
    PrintString("Demo PrintString syscall:\n");
    PrintString("Cu nhan tai nang 2019\n");
    Halt();
    /* not reached */
}
