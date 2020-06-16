#include <iostream>
#include <fstream>
#include <random>

#include <omh.hpp>
#include <omh_exact.hpp>
#include <parse_file.hpp>
#include <omh_compare_exact.hpp>


int main(int argc, char *argv[]) {
  omh_compare_exact args(argc, argv);

  std::ifstream is1(args.seq1_arg);
  if(!is1.good())
    omh_compare_exact::error() << "Failed to open first sequence file '" << args.seq1_arg << '\'';
  std::ifstream is2(args.seq2_arg);
  if(!is2.good())
    omh_compare_exact::error() << "Failed to open first sequence file '" << args.seq2_arg << '\'';

  std::string name1, seq1, name2, seq2;
  if(!parse_entry(is1, name1, seq1) || !parse_entry(is2, name2, seq2))
    omh_compare_exact::error() << "Failed to read sequences";

  double score = omh_exact(seq1, seq2, args.k_arg, args.l_arg);

  if(args.rc_flag) {
    const auto rc2 = reverse_complement(seq2);
    score = std::max(score, omh_exact(seq1, rc2, args.k_arg, args.l_arg));
  }

  std::cout << name1 << ' ' << name2 << ' ' << score << '\n';

  return 0;
}
