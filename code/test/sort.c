/* sort.c
 *	Simple program to sort an integer array.
 */

#include "syscall.h"
#define MAX_ARRAY 100

int
main()
{
  int n, i, j, tmp, flag;
  int a[MAX_ARRAY];
  PrintString("Nhap so luong phan tu: ");
  n = ReadNum();
  PrintString("Nhap gia tri phan tu (Moi so ket thuc bang Enter):\n");
  for(i = 0; i < n; ++i){
    a[i] = ReadNum();
  }
  PrintString("Nhap 1 (tang dan) hoac 0 (giam dan) (default: tang dan): ");
  flag = ReadNum();
  if(flag == 0){
    for(i = 0; i < n - 1; ++i){
      for(j = 0; j < n - i - 1; ++j){
        if(a[j] < a[j + 1]){
          tmp = a[j];
          a[j] = a[j + 1];
          a[j + 1] = tmp;
        }
      }
    }
  }
  else {
    for(i = 0; i < n - 1; ++i){
      for(j = 0; j < n - i - 1; ++j){
        if(a[j] > a[j + 1]){
          tmp = a[j];
          a[j] = a[j + 1];
          a[j + 1] = tmp;
        }
      }
    }
  }
  PrintString("Mang sau khi sap xep la:\n");
  for(i = 0; i < n; ++i){
    PrintNum(a[i]);
    PrintChar(' ');
  }
  PrintChar('\n');
  Halt();
}
