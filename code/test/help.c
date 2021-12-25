/*  help.c
 *	Simple program to print general information
 */

#include "syscall.h"

int
main()
{
  PrintString("*************************************************************************************\n");
  PrintString("I/Thanh vien:\n");
  PrintString("1. Do Nguyen Duy Hoang\t19120077\n");
  PrintString("2. Le Pham Lan Anh\t19120447\n");
  PrintString("3. Nguyen Dai Nghia\t19120735\n\n\n\n");
  PrintString("II/Chuong trinh sort:\nSap xep n so nguyen (n <= 100) theo thu tu tang dan hoac giam dan\n\n");
  PrintString("III/Chuong trinh ascii:\nIn ra cac ky tu in duoc cua bang ma ASCII\n");
  PrintString("*************************************************************************************\n");
  Halt();
}
