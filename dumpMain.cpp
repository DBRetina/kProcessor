#include <iostream>
#include <string>
#include "ThirdParty/CLI11.hpp"
#include <vector>
#include <stdint.h>
#include <gqf.h>
#include "KmerCounter/KmerCounter.hpp"
#include "KmerCounter/kmer.h"
#include "Utils/utils.hpp"
#include <cmath>

using namespace std;


int dump_main(int argc, char *argv[]){
  CLI::App app;
  string input_file="";
  int k;
  string outputKmers="";

  app.add_option("-i,--input", input_file,
   "MQF file")->required()->check(CLI::ExistingFile);
  app.add_option("-k,--kmer-length",k,"kmer length")->required();
  app.add_option("-o,--output-kmers", outputKmers,
    "Output in the format of Kmer\tCount. Available only in Exact Counting")->group("I/O");

  CLI11_PARSE(app, argc, argv);

  QF qf;
  qf_deserialize(&qf,input_file.c_str());

  dumpMQF(&qf,k,outputKmers);
  return 0;
}
