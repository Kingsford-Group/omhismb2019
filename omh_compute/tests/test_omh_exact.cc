#include <gtest/gtest.h>
#include <tuple>

#include <omh.hpp>
#include <omh_exact.hpp>
#include <common.hpp>


namespace {
TEST(EXACT, WJDeBruijn) {
  const auto mc1 = count_mers(dbg1, 4);
  const auto mc2 = count_mers(dbg2, 4);
  EXPECT_EQ(1.0, jaccard(mc1, mc2));
  EXPECT_EQ(1.0, weighted_jaccard(mc1, mc2));
}

TEST(EXACT, JdiffWF) {
  const auto dbg1 = std::string("AAAACCC");
  const auto dbg2 = std::string("AAAAACC");

  const auto mc1 = count_mers(dbg1, 4);
  const auto mc2 = count_mers(dbg2, 4);

  EXPECT_NEAR(3.0/4.0, jaccard(mc1, mc2), 0.01);
  EXPECT_NEAR(3.0/5.0, weighted_jaccard(mc1, mc2), 0.01);
}

TEST(EXACT, DeBruijn) {
  const unsigned k = 4, l = 2;

  const double score = omh_exact(dbg1, dbg2, k, l);
  EXPECT_NEAR(0.4, score, 0.01);
}

template<typename EngineT>
size_t random_pos(const std::string& s, EngineT& rng) {
  return std::uniform_int_distribution<unsigned>(0, s.size()-1)(rng);
}

template<typename EngineT>
std::string mutate(const std::string& seq, EngineT& rng) {
  std::string res(seq);

  const auto nb_mutations = std::uniform_int_distribution<unsigned>(0, res.size() / 2)(rng);
  enum MutationType { Mismatch, Insertion, Deletion };

  std::uniform_int_distribution<int> mutation_dist(0, 2);
  std::uniform_int_distribution<int> mismatch_dist(0, 3);

  for(unsigned i = 0; i < nb_mutations; ++i) {
    switch(mutation_dist(rng)) {
    case Mismatch:
      res[random_pos(res, rng)] = DNA[mismatch_dist(rng)];
      break;

    case Insertion:
      res.insert(random_pos(res, rng), 1, DNA[mismatch_dist(rng)]);
      break;

    case Deletion:
      res.erase(random_pos(res, rng), 1);
      break;
    }
  }

  return res;
}

TEST(EXACT, InOrder) {
  static const size_t size = 100;
  seed_type seed, seed_seq;
  auto rng = seed_seq.seeded();

  std::vector<unsigned> p1(100, 0), p2(100, 0);
  for(unsigned i = 0; i < size; ++i) {
    p1[i] = i;
    p2[i] = i;
  }
  std::shuffle(p1.begin(), p1.end(), rng);
  std::shuffle(p2.begin(), p2.end(), rng);

  const double in_order  = proportion_kmers_in_order(2, p1, p2);
  const double in_order2 = proportion_kmers_in_order2(p1, p2);
  EXPECT_NEAR(in_order2, in_order, 0.001);
}

//struct CompareParam { unsigned k, l; };
class CompareTest : public testing::TestWithParam<std::tuple<int,int>> { };

TEST_P(CompareTest, Compare) {
  const unsigned k          = std::get<0>(GetParam());
  const unsigned l          = std::get<1>(GetParam());
  const unsigned m          = 1000;
  const size_t   size       = l <= 2 ? 1000 : 200;
  const unsigned repetition = l <= 2 ? 10 : 1;

  for(unsigned r = 0; r < repetition; ++r) {
    seed_type seed, seed_seq;
    sketcher_type sketcher(k, l, m, seed);
    auto rng_seq = seed_seq.seeded();

    auto s1 = random_sequence(size, rng_seq, DNA);
    auto s2 = mutate(s1, rng_seq);

    const double exact_score = omh_exact(s1, s2, k, l);
    const double omh_score = sketcher.compare(s1, s2);

    EXPECT_NEAR(exact_score, omh_score, 0.1) << "Repetition " << r << " l " << l << " k " << k;
  }
}
INSTANTIATE_TEST_CASE_P(ExactOMH,
                        CompareTest,
                        testing::Combine(testing::Values(4, 7, 11, 20), testing::Values(0, 1, 2, 3)));

}
