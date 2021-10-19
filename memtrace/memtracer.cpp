#include <stdio.h>
#include "pin.H"
FILE * trace;


// update lastmem info and write to file (load)
void RecordMemRead(THREADID thread_id, ADDRINT addr, UINT32 size)
{
    fprintf(trace,"%lu r %#lx %u\n", thread_id, addr, size);
}

// Print a memory write record
void RecordMemWrite(THREADID thread_id, ADDRINT addr, UINT32 size)
{
    fprintf(trace,"%lu w %#lx %u\n", thread_id, addr, size);
}


// Is called for every instruction and instruments reads and writes
void Instruction(INS ins, void *v)
{
	if(INS_hasKnownMemorySize(ins) == false) {
		return;
	}

	// instruments loads using a predicated call, i.e.
	// the call happens iff the load will be actually executed
	// (this does not matter for ia32 but arm and ipf have predicated instructions)
	if (INS_IsMemoryRead(ins))
	{
		INS_InsertPredicatedCall(
			ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
			IARG_THREAD_ID,
			IARG_MEMORYREAD_EA,
			IARG_MEMORYREAD_SIZE,
			IARG_END);
	}

	// instruments stores using a predicated call, i.e.
	// the call happens iff the store will be actually executed
	if (INS_IsMemoryWrite(ins))
	{
		INS_InsertPredicatedCall(
			ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
			IARG_THREAD_ID,
			IARG_MEMORYWRITE_EA,
			IARG_MEMORYWRITE_SIZE,
			IARG_END);
	}
}



void Fini(INT32 code, void *v) {fclose(trace);}

int main(int argc, char * argv[]) {
	trace = fopen("itrace.out", "w");
	PIN_Init(argc,argv);
	INS_AddInstrumentFunction(Instruction, 0);
	PIN_AddFiniFunction(Fini, 0);
    PIN_StartProgram();
    return 0;
}

