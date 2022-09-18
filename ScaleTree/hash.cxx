
#include <stdlib.h>
#include <iostream>
#define HashSize 50000000
//#define HashSize ((1<<25)-1)
#define CandidateSize 1024
#define null 0
using namespace std;

typedef struct HashEntry {
  unsigned long addr;
  unsigned long long cycle;  /* the time of the last access to addr */
  struct HashEntry *hshNxt;
} HashEntry;

HashEntry **Hash;
HashEntry * candidate;
int used=0;
unsigned Hashallocated=0;

void allocateCandidate(){
  candidate = (HashEntry*) malloc(sizeof(HashEntry)*CandidateSize);
  Hashallocated+=CandidateSize*sizeof(HashEntry);
  used=0;
}

int HashInitialize() {
  //printf("Initialization\n");
  int i;
  allocateCandidate();
  Hash=(HashEntry**)malloc(sizeof(HashEntry*)*HashSize);

  for (i=0; i<HashSize; i++) Hash[i]=null;
  return 0;
}

void HashFree(){
    int i;

    for(i=0; i<HashSize; i++)
    {
      //if(Hash[i] != null)
	      Hash[i] = null;
    }
    //free(Hash);
    /*
  for(i=0; i<HashSize; i++)
  {
    HashEntry * head;
    HashEntry * temp;
    head = Hash[i];
    while(head != NULL)
    {
      temp = head;
      head = head->hshNxt;
      free(temp);
    }
    //free(Hash[i]);
  }*/
  //free(Hash);
  //free(candidate);
  //Hash = null;
}

/* addr is accessed at cyc, Cycle cyc means never accessed before
 * insert addr at the head if it is not found
 */
unsigned long long HashSearchUpdate(unsigned long addr, unsigned long long cyc) {
  unsigned long hshKey;
  HashEntry *entry;
  unsigned long long oldCyc;
  //  hshKey = addr % HashSize;
  //cout<<3<<endl;
  hshKey = addr;// & HashSize;
  //cout<<0<<endl;
  entry = Hash[hshKey];
  //cout<<1<<endl;
  while (entry != null && entry->addr != addr) entry=entry->hshNxt;
  if (entry!=null) {
    oldCyc = entry->cycle;
    entry->cycle = cyc;
    return oldCyc;
  }
  else {
    //cout<<2<<endl;
    //entry = (HashEntry *) malloc(sizeof(HashEntry));
    entry = &candidate[used++];
    if (used==CandidateSize)
      allocateCandidate();
    entry->addr = addr;
    entry->cycle = cyc;
    entry->hshNxt = Hash[hshKey];
    Hash[hshKey] = entry;
    return cyc;  /* means that a previous record is not found */
  }
}

void printAvalue()
{
//  fprintf(stderr, "Hash[11469690] = %x ", Hash[11469690]);
}
