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
#define MAX_FILE_LENGTH 32

#include "kernel.h"
#include "synchconsole.h"
#include "filesys.h"
#include "openfile.h"
#include <time.h>
#include "main.h"
//#include "synchcons.h"

FileSystem fs;
// SynchConsole gConsole;

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

// prototype: int ReadNum()
int SysReadNum()
{
  int result = 0;
  bool valid = true;
  bool negative = false;

  // *** Process the first input character ***
  char c = kernel->synchConsoleIn->GetChar();
  // loop while c is newline character
  while (c == '\n')
    c = kernel->synchConsoleIn->GetChar();
  // check negative character
  if (c == '-')
    negative = true;
  else if ('0' <= c && c <= '9')
    result = c - '0';
  else
    valid = false;

  // *** Process remain characters ***
  while (true)
  {
    c = kernel->synchConsoleIn->GetChar();
    if ('0' <= c && c <= '9' && valid)
      result = result * 10 + c - '0';
    // if overflow happen, result will be wrong naturally
    else if (c != '\n')
    {
      // input invalid character
      // sentinel is FALSE, return result zero
      valid = false;
      result = 0;
    }
    else
      // press Enter and terminate input
      break;
  }
  if (negative)
    return result * -1;
  else
    return result;
}

// prototype: void PrintNum(int number)
void SysPrintNum()
{
  // read 'number' from 4th register
  int number = kernel->machine->ReadRegister(4);
  // convert 'number' to char array
  // default maximum length is 100
  int cnt = 0;
  char arr[MAX_LENGTH];

  // check the negative number
  bool negative = false;
  if (number < 0)
  {
    negative = true;
    number = -number;
  }
  // separate
  while (true)
  {
    int digit = number % 10;
    number /= 10;
    arr[cnt++] = (char)(digit + '0');
    if (number == 0)
      break;
  }
  // insert '-' character to negative number
  if (negative)
    arr[cnt++] = '-';

  // create an object to handle printing
  SynchConsoleOutput sco(NULL);
  // print all char from char array
  for (int i = cnt - 1; i >= 0; --i)
    sco.PutChar(arr[i]);
}

// prototype: char ReadChar()
char SysReadChar()
{
  char c = kernel->synchConsoleIn->GetChar();
  // loop while char equal '\n'
  while (c == '\n')
    c = kernel->synchConsoleIn->GetChar();

  // accept only the first character
  // truncate the redundant characters
  while (true)
  {
    char r = kernel->synchConsoleIn->GetChar();
    if (r == '\n')
      break;
  }
  return c;
}

// prototype: void PrintChar(char character)
void SysPrintChar()
{
  // read 'character' from 4th register
  char c = kernel->machine->ReadRegister(4);
  SynchConsoleOutput sco(NULL);
  sco.PutChar(c);
}

// prototype: int RandomNum()
int SysRandomNum()
{
  srand(time(0));
  return rand();
}

/*
Input: - User space address (int)
 - Limit of buffer (int)
Output:- Buffer (char*)
Purpose: Copy buffer from User memory space to System memory space
*/
char *User2System(int virtAddr, int limit)
{
  int i; // index
  int oneChar;
  char *kernelBuf = NULL;

  kernelBuf = new char[limit + 1]; // need for terminal string
  if (kernelBuf == NULL)
    return kernelBuf;
  // memset(kernelBuf,0,limit+1);

  // printf("\n Filename u2s:");
  for (i = 0; i < limit; i++)
  {
    kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
    kernelBuf[i] = (char)oneChar;
    // printf("%c",kernelBuf[i]);
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
int System2User(int virtAddr, int len, char *buffer)
{
  if (len < 0)
    return -1;
  if (len == 0)
    return len;
  int i = 0;
  int oneChar = 0;
  do
  {
    oneChar = (int)buffer[i];
    kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
    i++;
  } while (i < len && oneChar != 0);
  return i;
}

// prototype: void ReadString(char* buffer, int length)
char *SysReadString()
{
  int addr = kernel->machine->ReadRegister(4); // address of the buffer of user space
  int length = kernel->machine->ReadRegister(5);
  char *sysBuff = new char[length + 1];

  // The loop below reads characters input from user, forming a string.
  int idx = 0;
  while (idx < length)
  {
    char ch = kernel->synchConsoleIn->GetChar();
    if (ch == '\n' || ch == 0)
    {
      sysBuff[idx] = '\0';
      break;
    }
    sysBuff[idx] = ch;
    idx++;
  }
  sysBuff[idx] = '\0';                // The string is created.
  System2User(addr, length, sysBuff); // Copy content from kernel buffer to user buffer
  delete[] sysBuff;
}

// prototype: void PrintString(char* buffer)
void SysPrintString()
{
  int addr = kernel->machine->ReadRegister(4);   // Get the address of the buffer of user space
  char *sysBuff = User2System(addr, MAX_LENGTH); // copy buffer from user space to kernel space
  int i = 0;
  while (i < MAX_LENGTH)
  {
    char c = sysBuff[i];
    if (c != '\0')
      kernel->synchConsoleOut->PutChar(c);
    else
      break;
    ++i;
  }
  // deallocate memory
  delete[] sysBuff;
}

int SysCreateFile() // Prototype: int CreateFile(char *name);
{
  int addr = kernel->machine->ReadRegister(4);   // Get the address of the buffer of user space
  char *sysBuff = User2System(addr, MAX_LENGTH); // copy buffer from user space to kernel space
  if (strlen(sysBuff) == 0)
    return -1;
  if (!sysBuff)
    return -1;

  bool code = kernel->fileSystem->Create(sysBuff); // Create file
  return code ? 0 : -1;                            // Return status code
}
//=======================================================================================

OpenFileId SysOpen() // Prototype: OpenFileId Open(char *name, int type);
{
  // DEBUG(dbgSys, "Vao duoc SysOpen()\n");
  int addr = kernel->machine->ReadRegister(4);
  int type = kernel->machine->ReadRegister(5);
  char *name = User2System(addr, 256);

  if (strcmp(name, "stdin") == 0)
  {
    return 0;
  }
  if (strcmp(name, "stdout") == 0)
    return 1;
  /* Hoang was here
  if (kernel->fileSystem->idx <= 9 && kernel->fileSystem->idx >= 0)
  {
    if ((kernel->fileSystem->openFiles[kernel->fileSystem->idx] = kernel->fileSystem->Open(name)) != NULL)
    {
      kernel->fileSystem->type[kernel->fileSystem->idx - 1] = type;
      return kernel->fileSystem->idx;
    }
  }
  end Hoang */

  int freeSlot = kernel->fileSystem->findFreeSlot();
  if (freeSlot != -1) // Chi xu li khi con slot trong
  {
    kernel->fileSystem->type[freeSlot] = type;
    if (type == 0 || type == 1) // chi xu li khi type = 0 hoac 1
    {
      kernel->fileSystem->openFiles[freeSlot] = kernel->fileSystem->Open(name);
      if (kernel->fileSystem->openFiles[freeSlot] != NULL) // Mo file thanh cong
      {
        delete[] name;
        return freeSlot;
      }

      else if (type == 2) // quy uoc type 2 la cho console input
      {
        delete[] name;
        return 0;
      }

      else if (type == 3) // quy uoc type 3 la cho console output
      {
        delete[] name;
        return 1;
      }
    }
  }
  delete[] name;
  return -1;
}

int SysClose() // Prototype: int Close(OpenFileId id);
{
  int fileid = kernel->machine->ReadRegister(4);
  if (fileid >= 0 && fileid <= 9)
  {
    if (kernel->fileSystem->openFiles[fileid])
    {
      delete kernel->fileSystem->openFiles[fileid];
      kernel->fileSystem->openFiles[fileid] = NULL;
      return 0;
    }
  }
  return -1;
}

int SysRead() // Prototype: int Read(char *buffer, int size, OpenFileId id)
{
  int virtualAddr = kernel->machine->ReadRegister(4),
      charcount = kernel->machine->ReadRegister(5);
  OpenFileId openFileId = kernel->machine->ReadRegister(6);
  if (openFileId > 9 || openFileId < 0)
    return -1; // File id out of table -> error
  if (!kernel->fileSystem->openFiles[openFileId])
    return -4; // Reading an unopened file -> error
  char *sysBuff = new char[charcount + 1];
  char ch;
  int idx = 0, size;
  switch (openFileId)
  {
  case 0: //Read stdin
    while (idx < charcount)
    {
      char ch = kernel->synchConsoleIn->GetChar();
      if (ch == '\n' || ch == 0)
      {
        sysBuff[idx] = '\0';
        break;
      }
      sysBuff[idx] = ch;
      idx++;
    }
    sysBuff[idx] = '\0';                // The string is created.
    System2User(virtualAddr, charcount, sysBuff); // Copy content from kernel buffer to user buffer
    delete[] sysBuff;
    return idx;
  case 1:
    return -2; // Reading stdout -> error
  default:  //Read files
    size = kernel->fileSystem->openFiles[openFileId]->Read(sysBuff, charcount);
    if (size > 0)
      System2User(virtualAddr, size, sysBuff);
    else
    {
      delete[] sysBuff;
      return -3;
    } // Can't read jack shit/null file -> error
    delete[] sysBuff;
    return size;
  }
  return -1; // Dummy return point
}

int SysWrite() // Prototype: int Write(char *buffer, int size, OpenFileId id);
{
  int virtualAddr = kernel->machine->ReadRegister(4),
      charcount = kernel->machine->ReadRegister(5);
  OpenFileId openFileId = kernel->machine->ReadRegister(6);
  if (openFileId > 9 || openFileId < 0)
    return -1; // File ID out of the table -> error
  if (kernel->fileSystem->openFiles[openFileId] == 0)
    return -4; // Writing an unopened file -> error
  if (kernel->fileSystem->type[openFileId] == 1)
    return -2; // Writing a read-only file -> error
  char *sysBuff = User2System(virtualAddr, charcount + 1);
  char ch;
  int size;
  int i;
  switch (openFileId)
  {
  case 1: //Write stdout
    i = 0;
    while (i < MAX_LENGTH)
    {
      char c = sysBuff[i];
      if (c != '\0')
        kernel->synchConsoleOut->PutChar(c);
      else
        break;
      ++i;
    }
    // deallocate memory
    delete[] sysBuff;
    return i;
  case 0:
    return -2; // Writing in stdin -> error
  default:
    size = kernel->fileSystem->openFiles[openFileId]->Write(sysBuff, charcount);
    delete[] sysBuff;
    return size;
  }
  return -1; // Dummy return point
}

SpaceId SysExec()
{
  int virtAddr = kernel->machine->ReadRegister(4); // doc dia chi ten chuong trinh tu thanh ghi r4
  char *name = User2System(virtAddr, MAX_FILE_LENGTH + 1); // Lay ten chuong trinh, nap vao kernel
  DEBUG(dbgSys, "dbg name = ");
  DEBUG(dbgSys, name);
  if (name == NULL)
  {
    DEBUG('a', "\n Not enough memory in System");
    printf("\n Not enough memory in System");
    // machine->WriteRegister(2, -1);
    // IncreasePC();
    delete[] name;
    return -1;
  }

  OpenFile *oFile = kernel->fileSystem->Open(name);
  if (oFile == NULL)
  {
    printf("\nExec:: Can't open this file.");
    // machine->WriteRegister(2,-1);
    // IncreasePC();
    delete[] name;
    return -1;
  }

  delete oFile;

  // Return child process id
  SpaceId tmp = kernel->pTab->ExecUpdate(name);
  delete[] name;
  return tmp;
}
int SysJoin()
{
  int tmp = kernel->machine->ReadRegister(4);
  int result = kernel->pTab->JoinUpdate(tmp);
  if (result == -1)
  {
    DEBUG(dbgSys, "Error!\n");
  }
  else
  {
    DEBUG(dbgSys, "Successfully!\n");
  }
  return result;
}
void SysExit()
{
  int exitCode = kernel->machine->ReadRegister(4);
  if (exitCode == 0)
  {
    int result = kernel->pTab->ExitUpdate(exitCode);
    if (kernel->currentThread->space != 0)
      delete kernel->currentThread->space;
    kernel->currentThread->Finish();
    DEBUG(dbgSys, "Successfully!\n");
  }
  else
  {
    DEBUG(dbgSys, "Error " << exitCode << "\n");
  }
}

int SysCreateSemaphore()
{
  int virtAddr = kernel->machine->ReadRegister(4);
  int semval = kernel->machine->ReadRegister(5);

  char *name = User2System(virtAddr, MAX_FILE_LENGTH + 1);
  if (name == NULL)
  {
    DEBUG('a', "\n Not enough memory in System");
    printf("\n Not enough memory in System");
    // machine->WriteRegister(2, -1);
    delete[] name;
    // IncreasePC();
    return -1;
  }

  int res = kernel->sTab->Create(name, semval);

  if (res == -1)
  {
    DEBUG('a', "\n Khong the khoi tao semaphore");
    printf("\n Khong the khoi tao semaphore");
    // machine->WriteRegister(2, -1);
    delete[] name;
    // IncreasePC();
    return -1;
  }

  delete[] name;
  // machine->WriteRegister(2, res);
  // IncreasePC();
  return res;
}
int SysWait()
{
  int virtAddr;
  char *name;
  virtAddr = kernel->machine->ReadRegister(4);
  name = User2System(virtAddr, MAX_FILE_LENGTH + 1);
  DEBUG(dbgSys, "Debug name of Wait() call: ");
  DEBUG(dbgSys, name);
  //printf("name SysWait = %s", name);
  if (name == NULL)
  {
    DEBUG('a', "\n Not enough memory in System");
    printf("\n Not enough memory in System");
    // machine->WriteRegister(2, -1);
    delete[] name;
    // IncreasePC();
    return -1;
  }

  int res = kernel->sTab->Wait(name);

  if (res == -1)
  {
    DEBUG('a', "\n Khong ton tai ten semaphore nay!");
    printf("\n Khong ton tai ten semaphore nay (Wait)!\n");
    delete[] name;
    return -1;
  }

  delete[] name;
  return res;
}
int SysSignal()
{
  int virtAddr = kernel->machine->ReadRegister(4);

  char *name = User2System(virtAddr, MAX_FILE_LENGTH + 1);
  DEBUG(dbgSys, "Debug name of Signal() call: ");
  DEBUG(dbgSys, name);
  if (name == NULL)
  {
    DEBUG('a', "\n Not enough memory in System");
    printf("\n Not enough memory in System");
    delete[] name;
    return -1;
  }
  
  int res = kernel->sTab->Signal(name);
  if (res == -1)
  {
    DEBUG('a', "\n Khong ton tai ten semaphore nay!");
    printf("\n Khong ton tai ten semaphore nay!(Signal)\n");
    // machine->WriteRegister(2, -1);
    delete[] name;
    // IncreasePC();
    return -1;
  }

  delete[] name;
  // machine->WriteRegister(2, res);
  // IncreasePC();
  return res;
}

int SysSeek(){
  // Input: Vi tri(int), id cua file(OpenFileID)
			// Output: -1: Loi, Vi tri thuc su: Thanh cong
			// Cong dung: Di chuyen con tro den vi tri thich hop trong file voi tham so la vi tri can chuyen va id cua file
			int pos = kernel->machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
			int id = kernel->machine->ReadRegister(5); // Lay id cua file
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 14)
			{
				printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
				return -1;
			}
			// Kiem tra file co ton tai khong
			if (kernel->fileSystem->openFiles[id] == NULL)
			{
				printf("\nKhong the seek vi file nay khong ton tai.");
				return -1;
			}
			// Kiem tra co goi Seek tren console khong
			if (id == 0 || id == 1)
			{
				printf("\nKhong the seek tren file console.");
				return -1;
			}
			// Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
			pos = (pos == -1) ? kernel->fileSystem->openFiles[id]->Length() : pos;
			if (pos > kernel->fileSystem->openFiles[id]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
			{
				printf("\nKhong the seek file den vi tri nay.");
				return -1;
			}
			else
			{
				// Neu hop le thi tra ve vi tri di chuyen thuc su trong file
				kernel->fileSystem->openFiles[id]->Seek(pos);
				return pos;
			}
}

#endif /* ! __USERPROG_KSYSCALL_H__ */