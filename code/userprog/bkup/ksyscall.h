/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"
#include <string>


void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

int SysReadNum() //(Hoàng) handles ReadNum syscall. Prototype: int ReadNum()
{
  int result = 0;
  while (true)
  {
    char ch = kernel->synchConsoleIn->GetChar(); //Reads each input character
    if (ch == '\n' || ch < '0' || ch > '9') {break;} else // If user presses enter or 
                                                          // A non-numeric character is input,
                                                          // Stop reading.
    result = result * 10 + ch - 48;     //Change result according to the characters
  }
  return result;
}

void SysPrintNum() //(Hoàng) handles PrintNum syscall. Prototype: void PrintNum(int number)
{
  int number = kernel->machine->ReadRegister(4);  //Reads `number` from 4th register ($4 or $a0)
  SynchConsoleOutput outConsole(NULL);  //Initializes an output console. NULL = screen output.
  string s; 
  //Converts int to string object
  while (number != 0)
  {
    int digit = number % 10;
    s.push_back(digit + 48);
    number /= 10;
  }
  string r;
  //Outputs the string, character in character.
  for (int i = s.size() - 1; i > -1; i--)
    outConsole.PutChar(s[i]);
  outConsole.PutChar('\n');
}

char SysReadChar() //(Hoàng) Handles ReadChar syscall. Prototype: char ReadChar(). Similar to ReadNum.
{
  SynchConsoleInput* inConsole = new SynchConsoleInput(NULL);
            //Initializes an input console. NULL = keyboard input.
  char ch;
  ch = inConsole->GetChar();
  delete inConsole;
  return ch;
}

void SysPrintChar() //(Hoàng) Handles PrintChar syscall. prototype: void PrintChar(char ch). Similar to PrintNum
{
  char ch = (char)kernel->machine->ReadRegister(4);
  SynchConsoleOutput outConsole(NULL);
  outConsole.PutChar(ch);
  outConsole.PutChar('\n');
}


char* SysReadString() //(Hoàng) Handles ReadString syscall. 
//Prototype: void ReadString(char[] buffer, int length)
      //or:  void ReadString(char* buffer, int length)
{
  int addr =           kernel->machine->ReadRegister(4);
	//This is the value of `buffer`, which is a pointer (as an int). $4 = $a0 in MIPS.
  int length = 		     kernel->machine->ReadRegister(5);
  //This is the value of `length`. $5 = $a1 in MIPS.
	char* b = new char[length + 1];
  //Creates a new char* string. This is the source for `buffer` to copy from.
  //This is necessary since `buffer` is at user space, while system calls are
  //handled in kernel space.

  //The loop below reads characters input from user, forming a string.
	int idx = 0;
	while (idx < length)
	{
		char ch = kernel->synchConsoleIn->GetChar();
		if (ch == '\n' || ch == 0)
		{
			b[idx] = '\0';
			break;
		}
		b[idx] = ch;
		idx++;
	}
  b[idx] = '\0'; //The string is created.
	System2User(addr, length, b); // This method copies content from `b` to `buffer`.
                                // Details below.
	delete[]b;  //Free `b`.
}


/*
Input: - User space address (int)
 - Limit of buffer (int)
Output:- Buffer (char*)
Purpose: Copy buffer from User memory space to System memory space
*/
char* User2System(int virtAddr,int limit)
{
 int i;// index
 int oneChar;
 char* kernelBuf = NULL;

 kernelBuf = new char[limit +1];//need for terminal string
 if (kernelBuf == NULL)
 return kernelBuf;
 memset(kernelBuf,0,limit+1);

 //printf("\n Filename u2s:");
 for (i = 0 ; i < limit ;i++)
 {
 kernel->machine->ReadMem(virtAddr+i,1,&oneChar);
 kernelBuf[i] = (char)oneChar;
 //printf("%c",kernelBuf[i]);
 if (oneChar == 0)
 break;
 }
 return kernelBuf;
} 

/*
Input: - User space address (int)
 - Limit of buffer (int)
 - Buffer (char[])
Output:- Number of bytes copied (int)
Purpose: Copy buffer from System memory space to User memory space
*/
int System2User(int virtAddr,int len,char* buffer)
{ 
 if (len < 0) return -1;
 if (len == 0)return len;
 int i = 0;
 int oneChar = 0 ;
 do{
 oneChar= (int) buffer[i];
 kernel->machine->WriteMem(virtAddr+i,1,oneChar);
 i ++;
 }while(i < len && oneChar != 0);
 return i;
} 

#endif /* ! __USERPROG_KSYSCALL_H__ */
