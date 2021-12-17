#include "syscall.h"
#include "copyright.h"
#define maxlen 32
#include <iostream.h>
int
main()
{
 int len;
 char filename[maxlen +1];
 /*Create a file*/
 if (Create("text.txt") < 0)
 {
    std::cerr << "Error occured creating file";
 }
 else
 {
    std::cerr << "File created.";
 }
 Halt();
} 