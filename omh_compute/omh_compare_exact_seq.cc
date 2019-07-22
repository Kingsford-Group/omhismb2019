#include <iostream>
#include <fstream>
#include <random>

#include <omh.hpp>
#include <omh_exact.hpp>
#include <parse_file.hpp>
#include <omh_compare_exact.hpp>


int main(int argc, char *argv[]) {
  omh_compare_exact args(argc, argv);

  double score = omh_exact(args.seq1_arg, args.seq2_arg, args.k_arg, args.l_arg);

  if(args.rc_flag) {
    const auto rc2 = reverse_complement(args.seq2_arg);
    score = std::max(score, omh_exact(args.seq1_arg, rc2, args.k_arg, args.l_arg));
  }

  std::cout << score << '\n';

  return 0;
}
