#include "syscall.h"

int main()
{
    int i;
    for (i = 0; i < 5; ++i)
    {
        Wait("lananh");
        PrintString("LanAnh ");
        PrintNum(i);
        PrintChar('\n');
        Signal("nghia");
    }
    Exit(0);
}