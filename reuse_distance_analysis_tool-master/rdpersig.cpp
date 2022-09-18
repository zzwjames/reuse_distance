/*
 *  This file contains an ISA-portable PIN tool for tracing memory accesses.
 */

//    Execute via "../Bin/pin -t rdtrace -- PROGRAM"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h> // have getpagesize
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "pin.H"
#include "time.h"
#include "ScaleTree/analyze.c"

#define LOGBUFFERSIZE 24
#define BUFFERSIZE (1<<LOGBUFFERSIZE)
//#define PAGESIZE (1<<12)

#if defined(TARGET_MAC)
#define TASK_START "_nanos::WorkDescriptor::startTask"
#define TASK_FINISH "_nanos::WorkDescriptor::finishTask"
#else
#define TASK_START "nanos::WorkDescriptor::startTask"
#define TASK_FINISH "nanos::WorkDescriptor::finishTask"
#endif

char FILE_OUTPUT_NAME[100];

const char * logfile="result/rd_hist.txt";

/* xshen*/
//FILE * traceFile;

int numInstr = 0;
int logblocksize=6;
#ifdef multiplethread
  pthread_mutex_t _mt_lock;
#endif
int bufferindex=0;
unsigned long *buffer;
long buffersprocessed=0;
time_t starttime;
int lastpagekeyB = 1;

bool start_record = false;
ADDRINT current_task = 0;

FILE *testFile;

void processBuffer(){
  for (int i=0;i<bufferindex;i++){
    _DataAccess(buffer[i]>> logblocksize);
  }
  bufferindex = 0;
}


int Init() {
    buffer = (unsigned long *)malloc(BUFFERSIZE*sizeof(unsigned long *));
    HashInitialize();
    CounterInitialize();
    return 0;
}

void task_start(ADDRINT id)
{

  cout<<"task"<<id<<" start"<<endl;
  if(id != 1)
  {
    PIN_LockClient();
    HashFree();
    HashInitialize();
    CounterInitialize();
    current_task = id;
    start_record = true;
    PIN_UnlockClient();
  }
}

void task_finish(ADDRINT id)
{
  cout<<"task"<<id<<" end"<<endl;

  if(current_task == id)
  {
    //printf("%lu\n",current_task);
    PIN_LockClient();
    processBuffer();
    char filename[64];
    sprintf(filename, "%lu.out", id);
    //printf("%s\n",filename);
    _PrintResults(filename);
    //HashFree();
    start_record = false;
    PIN_UnlockClient();
  }

}

// Record memory read
void RecordMemRead(void * addr)
{
  if(start_record)
  {
    buffer[bufferindex++]=(unsigned long) addr;
    if (bufferindex==BUFFERSIZE)
      processBuffer();
  }
}

// Record memory write
void RecordMemWrite(void * addr)
{
  if(start_record)
  {
    buffer[bufferindex++]=(unsigned long) addr;
    if (bufferindex == BUFFERSIZE)
      processBuffer();
  }

}

// Is called for every instruction and instruments reads and writes
void Instruction(INS ins, void *v)
{
    // instruments memory accesses using a predicated call, i.e.
    // the call happens iff the access will be actually executed
    // (this does not matter for ia32 but arm and ipf have predicated instructions)

    if (INS_IsMemoryRead(ins))
    {
        INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
            IARG_MEMORYREAD_EA,
            IARG_END);
    }
    if (INS_HasMemoryRead2(ins))
    {
        INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
            IARG_MEMORYREAD2_EA,
            IARG_END);
    }
    if (INS_IsMemoryWrite(ins)){
        INS_InsertCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
            IARG_MEMORYWRITE_EA,
            IARG_END);
  }
}

/*
VOID Image(IMG img, VOID *v)
{
    // Instrument the malloc() and free() functions.  Print the input argument
    // of each malloc() or free(), and the return value of malloc().
    //
    //  Find and only need to find the malloc() function. //NOT j_malloc
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec) )
    {
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn) )
        {
            string rtnName = RTN_Name(rtn);

	          if (rtnName.find("start") != string::npos){
              string demangledNameNoParams = PIN_UndecorateSymbolName(rtnName, UNDECORATION_NAME_ONLY);
              //cout<<demangledNameNoParams<<endl;
              if(demangledNameNoParams == TASK_START&& RTN_Valid(rtn)){
                //cout<<rtnName<<"  "<<demangledNameNoParams<<endl;
                RTN_Open(rtn);
                // Instrument start_crush() to print the input argument value and the return value.
                // Find the task schedule point
        	      RTN_InsertCall(rtn, IPOINT_AFTER,AFUNPTR(task_start),
                              //IARG_THREAD_ID,
                              //IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                              IARG_FUNCRET_EXITPOINT_VALUE,
                              IARG_END);
        	      RTN_Close(rtn);
              }
            }

            else if (rtnName.find("finish") != string::npos){
              string demangledNameNoParams = PIN_UndecorateSymbolName(rtnName, UNDECORATION_NAME_ONLY);
              //cout<<demangledNameNoParams<<endl;
              if(demangledNameNoParams == TASK_FINISH&& RTN_Valid(rtn)){
                //cout<<rtnName<<"  "<<demangledNameNoParams<<endl;
                RTN_Open(rtn);
                // Instrument start_crush() to print the input argument value and the return value.
                // Find the task schedule point
        	      RTN_InsertCall(rtn, IPOINT_AFTER,
                              AFUNPTR(task_finish),
                              //IARG_THREAD_ID,
                              IARG_FUNCRET_EXITPOINT_VALUE,
                              IARG_END);
        	      RTN_Close(rtn);
              }
            }
          }
      }
  }
*/

VOID Image(IMG img, VOID *v)
{
    // Instrument the malloc() and free() functions.  Print the input argument
    // of each malloc() or free(), and the return value of malloc().
    //
    //  Find and only need to find the malloc() function. //NOT j_malloc
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec) )
    {
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn) )
        {
            string rtnName = RTN_Name(rtn);

	          if (rtnName.find("start") != string::npos){
              string demangledNameNoParams = PIN_UndecorateSymbolName(rtnName, UNDECORATION_NAME_ONLY);
              //cout<<demangledNameNoParams<<endl;
              if(demangledNameNoParams == TASK_START&& RTN_Valid(rtn)){
                //cout<<rtnName<<"  "<<demangledNameNoParams<<endl;
                RTN_Open(rtn);
                // Instrument start_crush() to print the input argument value and the return value.
                // Find the task schedule point
        	      RTN_InsertCall(rtn, IPOINT_AFTER,AFUNPTR(task_start),
                              //IARG_THREAD_ID,
                              IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
                              IARG_END);
        	      RTN_Close(rtn);
              }
            }

            else if (rtnName.find("finish") != string::npos){
              string demangledNameNoParams = PIN_UndecorateSymbolName(rtnName, UNDECORATION_NAME_ONLY);
              //cout<<demangledNameNoParams<<endl;
              if(demangledNameNoParams == TASK_FINISH&& RTN_Valid(rtn)){
                //cout<<rtnName<<"  "<<demangledNameNoParams<<endl;
                RTN_Open(rtn);
                // Instrument start_crush() to print the input argument value and the return value.
                // Find the task schedule point
        	      RTN_InsertCall(rtn, IPOINT_AFTER,
                              AFUNPTR(task_finish),
                              //IARG_THREAD_ID,
                              IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
                              IARG_END);
        	      RTN_Close(rtn);
              }
            }
          }
      }
  }


INT32 Usage()
{
    return -1;
}



void Fini(INT32 code, void *v)
{
  processBuffer();
  time_t endtime=time(NULL);
  fprintf(stderr, "RDH: %ld seconds.\n",endtime-starttime);
  //_PrintResults(logfile);
  fflush(NULL);
}

int main(int argc, char *argv[])
{
  starttime =  time (NULL);
  if(PIN_Init(argc, argv))
  {
      return Usage();
      //return Usage2();
  }

  // Initialize Symbols, we need them to report functions and lines
    PIN_InitSymbols();

  int i;

  for (i=0;i<argc;i++){
    if (strstr(argv[i],"-blocksize")==argv[i]){ //max block size in bits
      sscanf(argv[i],"-blocksize%d",&logblocksize);
      printf("block size in bits is %d\n",logblocksize);
    }
    if (strstr(argv[i],"-Log")==argv[i]){ //max block size in bits
     logfile=argv[i]+4;
    }
  }
  Init();
  printf("Files used:  %s\n", logfile);

  fflush(NULL);

#ifdef multiplethread
  pthread_mutex_init(&_mt_lock,NULL);
#endif

  //traceFile = fopen("atrace.out", "w");

  //Init(); //initialize scaleTree for reuse distance calculation

  INS_AddInstrumentFunction(Instruction, 0);
  //TRACE_AddInstrumentFunction(BBLinstruction,0);
  IMG_AddInstrumentFunction(Image, 0);
  PIN_AddFiniFunction(Fini, 0);

  // Never returns
  PIN_StartProgram();

  return 0;
}
