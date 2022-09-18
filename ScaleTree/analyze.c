//#define CACHELINE 5 /* 32 byte */
//#define NULL 0
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <math.h>
#include <algorithm>
#include <queue>
#include <sstream>
#include <fstream>
#include <time.h>

#include<iostream>
#include<fstream>

#include "hash.cxx"
using namespace std;
typedef struct tree_node Tree;
struct tree_node {
    Tree * left, * right;
    unsigned long long item;    /* last access time of this node */
    unsigned nodeWt;  /* weight of the node */
    unsigned weight;  /* weight of the entire subtree, including this node */
    unsigned maxSize;    /* maximal size of the node */
    Tree *prev, *next; /* nodes that are next to this node in the sorted order */
};

Tree * trace = NULL;              /* the scale-tree trace  */
float errorRate = 0.001;              /* 0.1 means 90% accurate */

unsigned numData;
unsigned long long curCycle = 0;

#include "scaleTree.c"
#include "counter.cxx"

unsigned power=1;

int _DataAccess(unsigned long addr) {
  //cout<<addr<<endl;
  unsigned long long lastAccCyc;
  unsigned dis = 0;
  //int i;

/*  addr = addr >> CACHELINE;*/

  lastAccCyc = HashSearchUpdate(addr, curCycle);
  
  trace = QueryScaleTree(lastAccCyc, curCycle, trace, &dis);
  if (lastAccCyc == curCycle) { /* a new element */
    numData ++;
    if (numData - ((numData >> 10)<<10)==0) {
      power=1;
      unsigned tmp = numData;
      while (tmp>0) {
	tmp = tmp >> 6;
	power ++;
      }
    }
  }
  else
    RecordDistance(dis,addr);

  if (sizeTrace > 10*10 * 100 * power) {
    unsigned oldSize = sizeTrace;
    trace = CompactScaleTree(trace);
    //printf("compact the scale tree from size %u to %u \n",oldSize, sizeTrace);
    assert(sizeTrace <= oldSize/2);
  }

  curCycle ++;
  return dis;
}

void PrintSize(){
  printf("hash allocated %u,  hash table size %u; scaletree size %u.\n",Hashallocated, numData,sizeTrace);
}

int main(){
	HashInitialize();
	CounterInitialize();
	std::ifstream in("/home/zhiwei2/last/data.txt");
	std::string filename;
	std::string line;

  if (in) // 有该文件
  {
      while (getline(in, line)) // line中不包括每行的换行符
      {
      //    cout << line << endl;
          _DataAccess(stoi(line));
      }
  }
  else // 没有该文件
  {
      cout << "no such file" << endl;
  }
  //PrintSize();
  _PrintResults("./result_whole.txt");
  return 0;
}
