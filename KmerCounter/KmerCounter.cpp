#include "KmerCounter.hpp"
#include <iostream>
#include "kmer.h"
#include <fstream>

#include <seqan/seq_io.h>
#include "../HashUtils/hashutil.h"
#include <seqan/parallel.h>
#include <limits>
#include <omp.h>
#include <stdexcept>

#include <gqf.h>
using namespace std;
using namespace seqan;
#define QBITS_LOCAL_QF 16

/* dump the contents of a local QF into the main QF */
static inline void dump_local_qf_to_main(QF* local, QF* main )
{
  #pragma omp critical
  {
    QFi local_cfi;
    if (qf_iterator(local, &local_cfi, 0)) {
      do {
        uint64_t key = 0, value = 0, count = 0;
        qfi_get(&local_cfi, &key, &value, &count);
        //qf_spin_lock((int*)&main_qf_lock,true);
        //main_qf_lock=false;
        qf_insert(main, key, count, true, true);
      } while (!qfi_next(&local_cfi));
      qf_reset(local);
    }
  }
}

static inline void insertToLevels(uint64_t item,QF* local,QF* main)
{
  if(!qf_insert(main, item%main->metadata->range, 1,
										 true, false)) {
				qf_insert(local, item%local->metadata->range, 1,
									false, false);
				// check of the load factor of the local QF is more than 50%
				if (qf_space(local)>80) {
					dump_local_qf_to_main(local,main);
				}
			}


}


void loadIntoMQF(string sequenceFilename,int ksize,int noThreads, Hasher *hasher,QF * memoryMQF){
  SeqFileIn seqFileIn(sequenceFilename.c_str());
  StringSet<CharString> ids;
  StringSet<Dna5String> reads;
  omp_set_num_threads(noThreads);
  QF* localMQF;
  bool moreWork=true;
  uint64_t numReads=0;
  #pragma omp parallel private(ids,reads,localMQF) shared(seqFileIn,moreWork,numReads)
  {
    localMQF= new QF();
    qf_init(localMQF, (1ULL << QBITS_LOCAL_QF), memoryMQF->metadata->key_bits,
    0,memoryMQF->metadata->fixed_counter_size, true,"", 2038074761);
    while(moreWork)
    {
      SEQAN_OMP_PRAGMA(critical)
      {
        clear(reads);
        clear(ids);
        seqan::readRecords(ids, reads, seqFileIn,10000);
        moreWork=!atEnd(seqFileIn);
        numReads+=10000;
      }

      for(auto read:reads){
start_read:
        if(length(read)<ksize)
        {
          continue;
        }

        uint64_t first = 0;
        uint64_t first_rev = 0;
        uint64_t item = 0;

        for(int i=0; i<ksize; i++) {
          //First kmer
          uint8_t curr = kmer::map_base(read[i]);
          if (curr > DNA_MAP::G) {
            // 'N' is encountered
            //read = read.substr(i+1, length(read));

            erase(read,0,i+1);
            //continue;
            goto start_read;
          }
          first = first | curr;
          first = first << 2;
        }
        first = first >> 2;
        first_rev = kmer::reverse_complement(first, ksize);


        if (kmer::compare_kmers(first, first_rev))
        item = first;
        else
        item = first_rev;
        item = hasher->hash(item)%memoryMQF->metadata->range;
        insertToLevels(item,localMQF,memoryMQF);

        uint64_t next = (first << 2) & BITMASK(2*ksize);
        uint64_t next_rev = first_rev >> 2;

        for(uint32_t i=ksize; i<length(read); i++) {
          //next kmers
          //cout << "K: " << read.substr(i-K+1,K) << endl;
          uint8_t curr = kmer::map_base(read[i]);
          if (curr > DNA_MAP::G) {
            // 'N' is encountered
            //continue;
            //read = read.substr(i+1, length(read));

            erase(read,0,i+1);

            goto start_read;
          }
          next |= curr;
          uint64_t tmp = kmer::reverse_complement_base(curr);
          tmp <<= (ksize*2-2);
          next_rev = next_rev | tmp;
          if (kmer::compare_kmers(next, next_rev))
          item = next;
          else
          item = next_rev;


          item = hasher->hash(item)%memoryMQF->metadata->range;
          insertToLevels(item,localMQF,memoryMQF);
          next = (next << 2) & BITMASK(2*ksize);
          next_rev = next_rev >> 2;
        }
      }

    }

    dump_local_qf_to_main(localMQF,memoryMQF);
    qf_destroy(localMQF);
  }

}

void dumpMQF(QF * MQF,int ksize,std::string outputFilename){
  IntegerHasher Ihasher(BITMASK(2*ksize));
  ofstream output(outputFilename.c_str());
  QFi qfi;
  qf_iterator(MQF, &qfi, 0);
  do {
    uint64_t key, value, count;
    qfi_get(&qfi, &key, &value, &count);
    string kmer=kmer::int_to_str(Ihasher.Ihash(key),ksize);
    output<<kmer<<"\t"<<count<<endl;
  } while(!qfi_next(&qfi));
}

bool isEnough(vector<uint64_t> histogram,uint64_t noSlots,uint64_t fixedSizeCounter,uint64_t slotSize)
{
  cout<<"noSlots= "<<noSlots<<endl
      <<"fcounter= "<<fixedSizeCounter<<endl
      <<"slot size= "<<slotSize<<endl;

  noSlots=(uint64_t)((double)noSlots*0.95);
  for(uint64_t i=1;i<1000;i++)
  {
    uint64_t usedSlots=1;
    if(i>((1ULL)<<fixedSizeCounter)-1)
    {
      uint64_t nSlots2=0;
      __uint128_t capacity;
      do{
        nSlots2++;
        capacity=((__uint128_t)(1ULL)<<(nSlots2*slotSize+fixedSizeCounter))-1;
      //  cout<<"slots num "<<nSlots2<<" "<<capacity<<endl;
    }while((__uint128_t)i>capacity);
      usedSlots+=nSlots2;
    }
    cout<<"i= "<<i<<"->"<<usedSlots<<" * "<<histogram[i]<<endl;
    if(noSlots>=(usedSlots*histogram[i]))
    {
      noSlots-=(usedSlots*histogram[i]);
    }
    else
    {
      cout<<"failed"<<endl<<endl;
      return false;
    }

  }
  cout<<"success"<<endl<<endl;
  return true;
}
void estimateMemRequirement(std::string ntcardFilename,
   uint64_t slotSize,uint64_t tagSize,
   uint64_t *res_noSlots,uint64_t *res_fixedSizeCounter, uint64_t *res_slotSize
   , uint64_t *res_memory)
{
  uint64_t noDistinctKmers=0,totalNumKmers=0;
  vector<uint64_t> histogram(1000,0);
  ifstream ntcardFile(ntcardFilename);
  string f;
  uint64_t count;
  while(ntcardFile>>f>>count)
  {
    if(count==numeric_limits<uint64_t>::max())
      continue;
    if(f=="F0")
      noDistinctKmers=count;
    else if(f=="F1")
      totalNumKmers=count;
    else{
      f=f.substr(1,f.size());
      int n=atoi(f.c_str());
      histogram[n]=count;
    }
  }
  *res_memory=numeric_limits<uint64_t>::max();
  for(int i=8;i<64;i++)
  {
    uint64_t noSlots=(1ULL)<<i;
    if(noSlots<noDistinctKmers)
      continue;
    bool moreWork=false;
    for(uint64_t slotSize2=slotSize;slotSize2<slotSize+2;slotSize2++){
      for(uint64_t fixedSizeCounter=1;fixedSizeCounter<slotSize;fixedSizeCounter++)
      {
        if(isEnough(histogram,noSlots,fixedSizeCounter,slotSize2))
        {
          uint64_t tmpMem=((noSlots/8000)*(fixedSizeCounter+slotSize2+tagSize));
          if(*res_memory>tmpMem)
          {
            *res_memory=tmpMem;
            *res_fixedSizeCounter=fixedSizeCounter;
            *res_noSlots=noSlots;
            *res_slotSize=slotSize2;
            moreWork=true;
          }
        }
      }
    }
    if(!moreWork && *res_memory!=numeric_limits<uint64_t>::max())
      break;
  }
  if(*res_memory==numeric_limits<uint64_t>::max())
  {
    throw std::overflow_error("Data limits exceeds MQF capabilities(> uint64). Check if ntcard file is corrupted");
  }


}
