// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------


//To debug: ./nachos -d u -x .....
void
ExceptionHandler(ExceptionType which)
{

    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;

      case SC_Add:
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	ASSERTNOTREACHED();

	break;
	//(Hoàng) Handling ReadNum system call:
	case SC_ReadNum: //(Hoàng) Prototype: int ReadNum()
	  
	DEBUG(dbgSys, "Reading number " << "\n");
	
	/* Process ReadNum Systemcall*/
	
	result = SysReadNum(); //(Hoàng) defined in ksyscall.h

	DEBUG(dbgSys, "ReadNum returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result); //(Hoàng) return result in the 2nd register ($v0 in MIPS)
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	return;
	ASSERTNOTREACHED();
	break;
	//(Hoàng) handling PrintNum system call:
	case SC_PrintNum: //(Hoàng) Prototype: void PrintNum(int $a0)
	{
	int number = kernel->machine->ReadRegister(4); // (Hoàng) Take the argument in the 4th register ($a0 in MIPS)
	DEBUG(dbgSys, "Writing number " << number << "\n");
	
	/* Process PrintNum Systemcall*/
	
	SysPrintNum(); //(Hoàng) Defined in ksyscall.h

	DEBUG(dbgSys, "Print complete\n");
	}
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	return;
	ASSERTNOTREACHED();
	break;
	//(Hoàng) Handling ReadChar system call
	case SC_ReadChar: //(Hoàng) Prototype: char ReadChar()
	  
	DEBUG(dbgSys, "Reading character " << "\n");
	
	/* Process ReadNum Systemcall*/
	
	result = SysReadChar(); //(Hoàng) defined in ksyscall.h

	DEBUG(dbgSys, "ReadChar returning with " << (char)result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (char)result); //same as ReadNum
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	return;
	ASSERTNOTREACHED();
	break;
	//(Hoàng) Handling PrintChar system call, principles same as PrintNum
	case SC_PrintChar: //(Hoàng) Prototype: void PrintChar(char $a0)
	{
	char ch = (char)kernel->machine->ReadRegister(4); //4th register = $a0
	DEBUG(dbgSys, "Writing character \'" << ch << "\'" << "\n");
	
	/* Process ReadNum Systemcall*/
	
	SysPrintChar(); //(Hoàng) defined in ksyscall.h

	DEBUG(dbgSys, "Print complete\n");
	}
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	return;
	ASSERTNOTREACHED();
	break;
	//(Hoàng) Handling ReadString system call
	case SC_ReadString: //(Hoàng) Prototype: void ReadString(char* buffer, int length)
	  
	DEBUG(dbgSys, "Reading string" << "\n");
	
	/* Process ReadString Systemcall*/
	
	SysReadString(); //(Hoàng) Defined in ksyscall.h
	
		DEBUG(dbgSys, "Done");
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	return;
	ASSERTNOTREACHED();
	break;
	

    default:
	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      break;
	//(Hoàng) handling other exceptions. Only show error message and halt for now.
	//(Hoàng) Exceptions are defined in ../machine/machine.h
	case PageFaultException:
		cerr << "No valid translation found\n";
		SysHalt();
		break;
	case ReadOnlyException:
		cerr << "Write attempt to read-only page detected\n";
		SysHalt();
		break;
	case BusErrorException:
		cerr << "Invalid physical address after translation\n";
		SysHalt();
		break;
	case AddressErrorException:
		cerr << "Invalid address space" << "\n";
		SysHalt();
		break;
	case OverflowException:
		cerr << "Arithmetic operators resulted in integer overflow\n";
		SysHalt();
		break;
	case IllegalInstrException:
		cerr << "Intruction not defined\n";
		SysHalt();
		break;
	case NoException:
		return;
		break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}
