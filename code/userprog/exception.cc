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
#include "synchconsole.h"
#define MAX_LENGTH 1000
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

void ModifyReturnPoint(){
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
}

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);
	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	// declare some variables for receiving the result
	int result = 0, i = 0;
	char resultChar = '\0';
	char* s = NULL;
    
	// CONTROLLING STRUCTURE
    switch (which) {
    case SyscallException:
      switch(type) {
      case SC_Halt:
		DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
		SysHalt();
		ASSERTNOTREACHED();
		break;
// ===================================================================
    case SC_Add:
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));
	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	ModifyReturnPoint();
	return;		
	ASSERTNOTREACHED();
	break;

// ====================================================================
	case SC_ReadNum:
	DEBUG(dbgSys, "Read an integer\n");
	// Call SysReadNum() defined in ksyscall.h
	result = SysReadNum();	
	DEBUG(dbgSys, "The number is " << result << "\n");

	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);	
	ModifyReturnPoint();
	return;		
	ASSERTNOTREACHED();
	break;
//======================================================================
// =====================================================================
	case SC_PrintNum:
	DEBUG(dbgSys, "Print an integer\n");
	// Call SysPrintNum() defined in ksyscall.h
	SysPrintNum();		
	DEBUG(dbgSys, "Print complete\n");	

	ModifyReturnPoint();
	return;		
	ASSERTNOTREACHED();
	break;
//=======================================================================

// ======================================================================
	case SC_ReadChar:
	DEBUG(dbgSys, "Reading a character\n");
	// Call SysReadChar() defined in ksyscall.h
	result = SysReadChar();		
	DEBUG(dbgSys, "The character is " << (char)result << "\n");

	/* Prepare Result */
	kernel->machine->WriteRegister(2, result);	
	ModifyReturnPoint();
	return;		
	ASSERTNOTREACHED();
	break;
//========================================================================

// ======================================================================
	case SC_PrintChar:
	DEBUG(dbgSys, "Printing a character\n");
	// Call the function SysPrintChar() defined in ksyscall.h
	SysPrintChar();		
	DEBUG(dbgSys, "Print complete\n");

	ModifyReturnPoint();
	return;		
	ASSERTNOTREACHED();
	break;
//========================================================================

// ======================================================================
	case SC_RandomNum:
	DEBUG(dbgSys, "Randomize an integer\n");
	// Call SysRandomNum() defined in ksyscall.h
	result = SysRandomNum();		
	DEBUG(dbgSys, "The number is " << result << "\n");

	/* Prepare Result */
	kernel->machine->WriteRegister(2, result);	
	ModifyReturnPoint();
	return;		
	ASSERTNOTREACHED();
	break;
//========================================================================

// ======================================================================
	case SC_ReadString:
	DEBUG(dbgSys, "Reading a string\n");
	// Call SysReadString() defined in ksyscall.h
	SysReadString();		
	DEBUG(dbgSys, "Read complete\n");
	
	ModifyReturnPoint();
	return;		
	ASSERTNOTREACHED();
	break;
//========================================================================

// ======================================================================
	case SC_PrintString:
	DEBUG(dbgSys, "Printing a string\n");
	// Call the function SysPrintString() defined in ksyscall.h
	SysPrintString();	
	DEBUG(dbgSys, "Print complete\n");

	ModifyReturnPoint();
	return;		
	ASSERTNOTREACHED();
	break;
//========================================================================
	case SC_CreateFile:
	DEBUG(dbgSys, "Creating file " << (char*)kernel->machine->ReadRegister(4));
	result = SysCreateFile();
	kernel->machine->WriteRegister(2, result);
	if (result == 0) cerr << "Creation successful";
	else cerr << "Creation failed";    
	DEBUG(dbgSys, "Creation completed");
	ModifyReturnPoint();
	return;
	ASSERTNOTREACHED();
	break;
//========================================================================
	case SC_OpenFile:
	DEBUG(dbgSys, "Opening file " << (char*)kernel->machine->ReadRegister(4));
	result = SysOpen();
	if (result > 0) {DEBUG(dbgSys, "Creation completed. ID = " << result); cerr << "Creation completed. ID = " << result; }
	else {DEBUG(dbgSys, "Creation failed")};
	kernel->machine->WriteRegister(2, result);
	ModifyReturnPoint();
	return;
	ASSERTNOTREACHED();
	break;
//========================================================================
	case SC_CloseFile:
	DEBUG(dbgSys, "Closing file " << (char*)kernel->machine->ReadRegister(4));
	result = SysClose();
	if (result == 0) {DEBUG(dbgSys, "Close completed."); cerr << "Close completed"; }
	else {DEBUG(dbgSys, "Close failed"); cerr << "Close failed";};
	kernel->machine->WriteRegister(2, result);
	ModifyReturnPoint();
	return;
	ASSERTNOTREACHED();
	break;
//========================================================================
    default:
	cerr << "Unexpected system call " << type << "\n";
	break;
    }
    break;
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
		cerr << "Instruction not defined\n";
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
