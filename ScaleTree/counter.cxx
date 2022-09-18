
#define NumCounters 100000

#include <assert.h>
#include <stdio.h>

#include <unordered_map>
#include <utility>
//using namespace std;

unsigned long long counters[NumCounters];
unsigned sizes[NumCounters];

FILE *histFile = NULL;

extern unsigned numData;

#define LogLinearSize 10
int linearSize;    /* size of the linear scale section */

#define CacheBlockNum 6463293
unsigned long Addr[6463293];

#define CacheBlockNum_1 903090
unsigned long Addr_1[903090];

#define CacheBlockNum_3 1674167
unsigned long Addr_3[1674167];

#define CacheBlockNum_7 1384388
unsigned long Addr_7[1384388];

#define CacheBlockNum_15 1357034
unsigned long Addr_15[1357034];

#define MAX_UINT 0xffffffff;
//typedef std::pair<unsigned, unsigned> P;
typedef std::unordered_map<unsigned long, std::pair<unsigned, unsigned>> MAP;
typedef std::unordered_map<unsigned long, unsigned> RESULT;

MAP counter;
RESULT result;

int _PrintResults_1(const char *fileName);
int _PrintResults_3(const char *fileName);
int _PrintResults_7(const char *fileName);
int _PrintResults_15(const char *fileName);


int CounterInitialize() {
  int i;
  int base = 2, logSize = LogLinearSize, cnt=1, log=2;
  sizes[0] = 0;
  while (cnt<=logSize) {
    sizes[cnt] = log;
    log *= base;
    cnt ++;
  }
  linearSize = sizes[logSize];

  for (i=logSize+1; i<NumCounters; i++)
    sizes[i] = sizes[i-1] + linearSize;

  for (i=0; i<NumCounters; i++) counters[i] = 0;

  return 0;
}


void RecordDistance(unsigned dis, unsigned long addr) {
  int li;
  if ((dis)==0){
    if(counters[0]<CacheBlockNum){
      Addr[counters[0]] = addr;
    }
    counters[0]++;
  }
  else {
   li = (dis) >> LogLinearSize;
   if (li > 0) {
     if(LogLinearSize+li<NumCounters)
       counters[LogLinearSize+li]++;
     else
       counters[NumCounters-1]++;
   } else {
    int t=0, td=(dis);
    assert(li==0);
    while (td>0) {
      t++;
      td = td>>1;
    }
    if(dis==1){
      Addr_1[counters[t]] = addr;
    }
    else if(dis<=3){
      Addr_3[counters[t]] = addr;
    }
    else if(dis<=7){
      Addr_7[counters[t]] = addr;
    }
    else if(dis<=15){
      Addr_15[counters[t]] = addr;
    }
    counters[t]++;
   }
  }
}



int _PrintResults(const char *fileName) {
  int i, lastNonZero=0;
  unsigned long long totAcc=0;
  trace = CompactScaleTree(trace);

  histFile = fopen(fileName,"w");
  assert(histFile!=NULL);

  fprintf(histFile," Total data is %u \n", numData);
  for (i=0; i<NumCounters; i++) {
    if (counters[i]==0) continue;
    if (i>lastNonZero) lastNonZero = i;
    totAcc += counters[i];
  }
  fprintf(histFile," Total access is %llu \n", totAcc);
  fprintf(histFile," Distance 0 \t%llu\n", counters[0]);
  for (i=1; i<=lastNonZero; i++)
    fprintf(histFile," Distance %d to %d \t%llu\n", sizes[i-1],
	    sizes[i]-1, counters[i]);

  fprintf(histFile," End tree size is %u \n", sizeTrace);
  _PrintResults_1("./result_1.txt");
  _PrintResults_3("./result_3.txt");
  _PrintResults_7("./result_7.txt");
  _PrintResults_15("./result_15.txt");
  // fprintf(histFile,"\n\n Top 20 reuse distance smallest \n");
  // for(int i = 0; i<CacheBlockNum; i++)
  // {
  //   fprintf(histFile," %lu \n", Addr[i]);
  // }
  fclose(histFile);
  return 0;
}
int _PrintResults_1(const char *fileName) {
  histFile = fopen(fileName,"w");
  for(int i = 0; i<CacheBlockNum_1; i++)
  {
    if(Addr_1[i]!=0){
      fprintf(histFile," %lu \n", Addr_1[i]);
    }
  }
  return 0;
}
int _PrintResults_3(const char *fileName) {
  histFile = fopen(fileName,"w");
  for(int i = 0; i<CacheBlockNum_3; i++)
  {
    if(Addr_3[i]!=0){
      fprintf(histFile," %lu \n", Addr_3[i]);
    }
  }
  return 0;
}
int _PrintResults_7(const char *fileName) {
  histFile = fopen(fileName,"w");
  for(int i = 0; i<CacheBlockNum_7; i++)
  {
    if(Addr_7[i]!=0){
      fprintf(histFile," %lu \n", Addr_7[i]);
    }
  }
  return 0;
}
int _PrintResults_15(const char *fileName) {
  histFile = fopen(fileName,"w");
  for(int i = 0; i<CacheBlockNum_15; i++)
  {
    if(Addr_15[i]!=0){
      fprintf(histFile," %lu \n", Addr_15[i]);
    }
  }
  return 0;
}
// void CounterInitialize()
// {
//   counter.clear();
//   result.clear();
// }

// void RecordDistance(unsigned dis, unsigned long addr)
// {
//   MAP::iterator it = counter.find(addr);
//   if(it != counter.end()){
//     it->second.first += dis;
//     it->second.second++;
//   }
//   else{
//     unsigned times = 1;
//     //P p1 = std::make_pair(dis,times);
//     counter.insert(MAP::value_type(addr,std::make_pair(dis,times)));
//   }
// }

void GetResult()
{
  for(MAP::iterator iter = counter.begin(); iter != counter.end(); iter++)
  {
    unsigned long addr = iter->first;
    unsigned dis = iter->second.first/iter->second.second;
    result.insert(RESULT::value_type(addr, dis));
  }
}

void FindMin(unsigned long &addr, unsigned &dis)
{
  unsigned min = MAX_UINT;
  RESULT::iterator rit;
  for(RESULT::iterator iter = result.begin(); iter != result.end(); iter++)
  {
    if(min > iter->second)
    {
      min = iter->second;
      rit = iter;
    }
  }

  addr = rit->first;
  dis = rit->second;
  result.erase(rit);
}




// int _PrintResults(const char *fileName) {
//   //std::cout<<"Print result"<<std::endl;
//   trace = CompactScaleTree(trace);

//   histFile = fopen(fileName,"w");
//   assert(histFile!=NULL);

//   GetResult();
//   //std::cout<<"compact scale Tree is work"<<std::endl;
//   int length = counter.size()/100;
//   //std::cout<<"length = "<<length<<std::endl;
//   int choosen = 0;
//   while(choosen <= length)
//   {
//     unsigned long addr;
//     unsigned dis;
//     FindMin(addr,dis);
//     fprintf(histFile,"0x%lx %u\n",addr,dis);
//     choosen++;
//   }

//   fclose(histFile);
//   return 0;
// }
