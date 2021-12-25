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
#define MAX_LENGTH 1000

#include "kernel.h"
#include "synchconsole.h"
#include "filesys.h"
#include <time.h>
//#include "synchcons.h"


FileSystem fs;
//SynchConsole gConsole;

void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}


// prototype: int ReadNum()
int SysReadNum(){
  int result = 0;
  bool valid = true;
  bool negative = false;

  // *** Process the first input character ***
  char c = kernel->synchConsoleIn->GetChar();
  // loop while c is newline character
  while(c == '\n')
    c = kernel->synchConsoleIn->GetChar();
  // check negative character
  if(c == '-')
    negative = true;
  else if('0' <= c && c <= '9')
    result = c - '0';
  else
    valid = false;

  // *** Process remain characters ***
  while(true){
    c = kernel->synchConsoleIn->GetChar();
    if('0' <= c && c <= '9' && valid)
      result = result * 10 + c - '0';
      // if overflow happen, result will be wrong naturally
    else if(c != '\n'){
      // input invalid character
      // sentinel is FALSE, return result zero
      valid = false;
      result = 0;
    }
    else
      // press Enter and terminate input
      break;
  }
  if(negative)
    return result * -1;
  else
    return result;
}



// prototype: void PrintNum(int number)
void SysPrintNum(){
  // read 'number' from 4th register
  int number = kernel->machine->ReadRegister(4);
  // convert 'number' to char array
  // default maximum length is 100
  int cnt = 0;
  char arr[MAX_LENGTH];

  // check the negative number
  bool negative = false;
  if(number < 0){
    negative = true;
    number = - number;
  }
  // separate 
  while(true){
    int digit = number % 10;
    number /= 10;
    arr[cnt++] = (char) (digit + '0');
    if(number == 0)
      break;
  }
  // insert '-' character to negative number
  if(negative)
    arr[cnt++] = '-';
    
  // create an object to handle printing
  SynchConsoleOutput sco(NULL);
  // print all char from char array
  for(int i = cnt - 1; i >= 0; --i)
    sco.PutChar(arr[i]);
}


// prototype: char ReadChar()
char SysReadChar(){
  char c = kernel->synchConsoleIn->GetChar();
  // loop while char equal '\n'
  while(c == '\n')
    c = kernel->synchConsoleIn->GetChar();
  
  // accept only the first character
  // truncate the redundant characters
  while(true){
    char r = kernel->synchConsoleIn->GetChar();
    if(r == '\n')
      break;
  }
  return c;
}



// prototype: void PrintChar(char character)
void SysPrintChar(){
  // read 'character' from 4th register
  char c = kernel->machine->ReadRegister(4);
  SynchConsoleOutput sco(NULL);
  sco.PutChar(c);
}

// prototype: int RandomNum()
int SysRandomNum(){
  srand(time(0));
  return rand();
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
 //memset(kernelBuf,0,limit+1);

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


// prototype: void ReadString(char* buffer, int length)
char* SysReadString() {
  int addr = kernel->machine->ReadRegister(4); // address of the buffer of user space
  int length = kernel->machine->ReadRegister(5);
	char* sysBuff = new char[length + 1];

  //The loop below reads characters input from user, forming a string.
	int idx = 0;
	while (idx < length){
		char ch = kernel->synchConsoleIn->GetChar();
		if (ch == '\n' || ch == 0){
			sysBuff[idx] = '\0';
			break;
		}
		sysBuff[idx] = ch;
		idx++;
	}
  sysBuff[idx] = '\0'; // The string is created.
	System2User(addr, length, sysBuff); // Copy content from kernel buffer to user buffer
	delete[] sysBuff;
}


// prototype: void PrintString(char* buffer)
void SysPrintString(){
  int addr = kernel->machine->ReadRegister(4); // Get the address of the buffer of user space
  char* sysBuff = User2System(addr, MAX_LENGTH); // copy buffer from user space to kernel space
  int i = 0;
  while(i < MAX_LENGTH){
    char c = sysBuff[i];
    if(c != '\0') 
      kernel->synchConsoleOut->PutChar(c);
    else 
      break;
    ++i;
  }
  // deallocate memory
  delete[] sysBuff;    
}

int SysCreateFile() //Prototype: int CreateFile(char *name);
{
  int addr = kernel->machine->ReadRegister(4); // Get the address of the buffer of user space
  char* sysBuff = User2System(addr, MAX_LENGTH); // copy buffer from user space to kernel space
  if (strlen(sysBuff) == 0) return -1;
  if (!sysBuff) return -1;
  
  bool code = fs.Create(sysBuff);  //Create file
  return code? 0 : -1;          //Return status code
}
//=======================================================================================

OpenFileId SysOpen() //Prototype: OpenFileId Open(char *name, int type);
{
  int addr = kernel->machine->ReadRegister(4);
  int type = kernel->machine->ReadRegister(5);
  char* name = User2System(addr, 256);
  
  if (strcmp(name, "stdin") == 0) 
  {
    return 0;
  }
  if (strcmp(name, "stdout") == 0)
    return 1;

  if (fs.idx <= 9 && fs.idx >= 0)
  {
    if ((fs.openFiles[fs.idx] = fs.Open(name)) != NULL)
    {  
      fs.type[fs.idx - 1] = type; 
      return fs.idx - 1;
    }
  }
  return -1;
}

int SysClose() //Prototype: int Close(OpenFileId id);
{
  int fileid = kernel->machine->ReadRegister(4);
  if (fileid >= 0 && fileid <= 9)
  {
    if (fs.openFiles[fileid])
    {
      delete fs.openFiles[fileid];
      fs.openFiles[fileid] = NULL;
      return 0;
    }
  }
  return -1;
}

int SysRead()   //Prototype: int Read(char *buffer, int size, OpenFileId id)
{
  int virtualAddr = kernel->machine->ReadRegister(4),
        charcount = kernel->machine->ReadRegister(5);
  OpenFileId openFileId = kernel->machine->ReadRegister(6);
  if (openFileId > 9 || openFileId < 0) return -1;     //File id out of table -> error
  if (!fs.openFiles[openFileId]) return -4;            //Reading an unopened file -> error
  char* sysBuff = new char[charcount + 1];
  char ch; int idx = 0, size;
  switch (openFileId)
  {
    case 0:
          // size = gConsole.Read(sysBuff, charcount);
          // System2User(virtualAddr, size, sysBuff);
          // delete[] sysBuff;
          // return size;
    case 1:
      return -2;      //Reading stdout -> error
    default:
          size = fs.openFiles[openFileId]->Read(sysBuff, charcount);
          if (size > 0) System2User(virtualAddr, size, sysBuff); else {delete[] sysBuff; return -3;}   //Can't read jack shit/null file -> error
          delete[] sysBuff;
          return size;
  }
  return -1;    //Dummy return point
}

int SysWrite()  //Prototype: int Write(char *buffer, int size, OpenFileId id);
{
  int virtualAddr = kernel->machine->ReadRegister(4),
        charcount = kernel->machine->ReadRegister(5);
  OpenFileId openFileId = kernel->machine->ReadRegister(6);
  if (openFileId > 9 || openFileId < 0) return -1;//File ID out of the table -> error
  if (!fs.openFiles[openFileId]) return -4;       //Writing an unopened file -> error
  if (!fs.type[openFileId] == 1) return -2;       //Writing a read-only file -> error
  char* sysBuff = User2System(virtualAddr, charcount + 1);
  char ch; int size;
  switch (openFileId)
  {
    case 1:
    case 0:
      return -2;                                  //Writing in stdin -> error
    default:
          size = fs.openFiles[openFileId]->Write(sysBuff, charcount);
          delete[] sysBuff;
          return size;
  }
  return -1;      //Dummy return point
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
