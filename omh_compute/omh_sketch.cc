#include <iostream>
#include <fstream>
#include <omh.hpp>
#include <random>

#include <seeded_prg.hpp>
#include <parse_file.hpp>
#include "omh_sketch.hpp"

typedef std::mt19937_64 rng_type;

// Read a fasta file and create a sketch for each entry in file
int main(int argc, char *argv[]) {
  const omh_sketch args(argc, argv);

  std::ofstream os;
  if(args.output_given) {
    os.open(args.output_arg);
    if(!os.good())
      omh_sketch::error() << "Failed to open output file '" << args.output_arg << '\'';
  }

  LongSeed<rng_type> seed(args.seedout_given ? args.seedout_arg : nullptr,
                          args.seedin_given  ? args.seedin_arg  : nullptr);
  omh_sketcher<rng_type> sketcher(args.k_arg, args.l_arg, args.m_arg, seed);

  std::string name, seq;
  for(const auto seq_file : args.sequence_arg) {
    std::ifstream is(seq_file);
    if(!is.good())
      omh_sketch::error() << "Failed to open file '" << seq_file << '\'';

    while(parse_entry(is, name, seq, !args.case_flag)) {
      if(!args.output_given) {
        os.open(name + ".sketch");
        if(!os.good())
          omh_sketch::error() << "Failed to write to '" << (name + ".sketch") << '\'';
      }
      sketcher.write(os, name, seq, args.rc_flag);
      if(!args.output_given)
        os.close();
    }
  }

  return 0;
}
