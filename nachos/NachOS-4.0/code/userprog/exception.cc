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

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      int result, op1, op2;

      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;

      case SC_Add:
	op1 = kernel->machine->ReadRegister(4);
	op2 = kernel->machine->ReadRegister(5);

	DEBUG(dbgSys, "Add " << op1 << " + " << op2 << "\n");
	
	/* Process SysAdd Systemcall*/
	result = SysAdd(op1, op2);

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

      case SC_OSAdd: 
	op1 = kernel->machine->ReadRegister(4);
	op2 = kernel->machine->ReadRegister(5);

	DEBUG(dbgSys, "Add " << op1 << " + " << op2 << "\n");
	
	result = myAdd(op1, op2);

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	
	kernel->machine->WriteRegister(2, (int)result);

	{
 	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	//myPrintF("The return value of OSAdd(%d, %d) is %d", op1, op2, (int)result);

	return;

        ASSERTNOTREACHED();

	break;

      case SC_OSSub:
	op1 = kernel->machine->ReadRegister(4);
	op2 = kernel->machine->ReadRegister(5);

	DEBUG(dbgSys, "Sub " << op1 << " - " << op2 << "\n");
	
	result = mySub(op1, op2);

	//myPrintF("The return value of OSSub(%d, %d) is %d", op1, op2, (int)result);
	
	DEBUG(dbgSys, "Sub returning with " << result << "\n");

	kernel->machine->WriteRegister(2, (int)result);

	{
          kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

        ASSERTNOTREACHED();

	break;

      case SC_OSDiv:
	op1 = kernel->machine->ReadRegister(4);
	op2 = kernel->machine->ReadRegister(5);

	DEBUG(dbgSys, "Div " << op1 << " / " << op2 << "\n");
	
	result = myDiv(op1, op2);

	//myPrintF("The return value of OSDiv(%d, %d) is %d", op1, op2, (int)result);

	DEBUG(dbgSys, "Div returning with " << result << "\n");

	kernel->machine->WriteRegister(2, (int)result);

	{
 	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

        ASSERTNOTREACHED();

	break;

      case SC_OSMul:
	op1 = kernel->machine->ReadRegister(4);
	op2 = kernel->machine->ReadRegister(5);

	DEBUG(dbgSys, "Mul " << op1 << " * " << op2 << "\n");
	
	result = myMul(op1, op2);

	//myPrintF("The return value of OSMul(%d, %d) is %d", op1, op2, (int)result);
	
	DEBUG(dbgSys, "Mul returning with " << result << "\n");

	kernel->machine->WriteRegister(2, (int)result);

	{
 	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

        ASSERTNOTREACHED();

	break;

      case SC_Print:
	DEBUG(dbgSys, "Print " << kernel->machine->ReadRegister(4) << " with length "
			       << kernel->machine->ReadRegister(5) << "\n");

	result = myPrint((char*)kernel->machine->ReadRegister(4),
			 (int)kernel->machine->ReadRegister(5));
 
	DEBUG(dbgSys, "Print returning with " << result << "\n");

	kernel->machine->WriteRegister(2, (int)result);

	{
 	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

        ASSERTNOTREACHED();

	break;

      case SC_PrintF:
	DEBUG(dbgSys, "PrintF " << kernel->machine->ReadRegister(4) << " with value " 
			<< kernel->machine->ReadRegister(5) << " and " 
			<< kernel->machine->ReadRegister(6) << " and "
			<< kernel->machine->ReadRegister(7) << "\n");

	result = myPrintF((char*)kernel->machine->ReadRegister(4),
			  (int)kernel->machine->ReadRegister(5),
			  (int)kernel->machine->ReadRegister(6),
			  (int)kernel->machine->ReadRegister(7));
 
	DEBUG(dbgSys, "PrintF returning with " << result << "\n");

	kernel->machine->WriteRegister(2, (int)result);

	{
 	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
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
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}
