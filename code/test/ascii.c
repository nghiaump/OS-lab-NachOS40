/* ascii.c
 *	Simple program to print the printable ASCII characters
 *  Using PrintChar syscall
 */

#include "syscall.h"

int
main()
{
  
  int i;
  Wait("exexe");
  // Printable character from 0 to 127
  PrintString("Printing printable ASCII characters\n");
  for(i = 32; i < 128; ++i){
    PrintNum(i);
    PrintChar('\t');
    PrintChar((char)i);
    PrintChar('\n');
  }

  // Print the newline character
  PrintChar('\n');
  Exit(0);
  /* not reached */
}
