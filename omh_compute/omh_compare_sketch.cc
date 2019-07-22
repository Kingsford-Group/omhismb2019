#include <iostream>
#include <fstream>

#include <omh.hpp>
#include <omh_compare_sketch.hpp>


int main(int argc, char *argv[]) {
  omh_compare_sketch args(argc, argv);

  std::ifstream is1(args.sk1_arg);
  if(!is1.good())
    omh_compare_sketch::error() << "Can't open first sketch '" << args.sk1_arg << '\'';
  std::ifstream is2(args.sk2_arg);
  if(!is2.good())
    omh_compare_sketch::error() << "Can't open second sketch '" << args.sk2_arg << '\'';

  const auto sk1 = sketch::from_stream(is1);
  const auto sk2 = sketch::from_stream(is2);

  const auto score = compare_sketches(sk1, sk2, args.m_given ? args.m_arg : -1, args.circular_flag);
  if(score < 0.0)
    omh_compare_sketch::error() << "Incompatible sketches";

  std::cout << sk1.name << ' ' << sk2.name << ' ' << score << '\n';

  return 0;
}
