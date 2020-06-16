#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

#include <seeded_prg.hpp>
#include <omh.hpp>
#include <common.hpp>

namespace {
TEST(RC, Random) {
  const std::string rc = reverse_complement(std::string("ACGTacgt"));
  EXPECT_EQ(std::string("acgtACGT"), rc);

  seed_type             seed;
  auto                  rng  = seed.seeded();
  static const size_t size = 1001;
  std::string s = random_sequence(size, rng, DNA);
  std::string ss = reverse_complement(reverse_complement(s));
  EXPECT_EQ(s, ss);
}

TEST(OMH, omh_pos) {
  static const size_t   size = 1000;
  static const unsigned k    = 6, l = 3, m = 500;
  seed_type             seed;
  auto                  rng  = seed.seeded();
  const auto            str  = random_sequence(size, rng);

  unsigned n = 0;
  omh_pos(str, k, l, m, rng,
          [&n](unsigned i, unsigned j, size_t pos) {
            EXPECT_EQ(n++, i * l + j);
            EXPECT_TRUE(pos <= size - k);
          });

  // Test if string too small
  for(unsigned i = 0; i < k-1; ++i) {
    const std::string small(i, '\0');
    n = 0;
    omh_pos(small, k, l, m, rng,
            [&n](unsigned i, unsigned j, size_t pos) { ++n; });
    EXPECT_EQ((unsigned)0, n);
  }
}

TEST(OMH, Identical) {
  const auto s = std::string('A', 50);
  const unsigned k = 5, l = 2, m = 20;

  seed_type     seed;
  sketcher_type sketcher(k, l, m, seed);

  const auto sk1 = sketcher.compute(s);
  const auto sk2 = sketcher.compute(s);
  const auto pos1 = sketcher.pos(s);
  const auto pos2 = sketcher.pos(s);

  EXPECT_EQ(k, sk1.k);
  EXPECT_EQ(l, sk1.l);
  EXPECT_EQ(m, sk1.m);
  EXPECT_EQ(m * l * k, (unsigned)sk1.data.size());
  EXPECT_EQ(m * l, (unsigned)pos1.size());

  EXPECT_EQ(k, sk2.k);
  EXPECT_EQ(l, sk2.l);
  EXPECT_EQ(m, sk2.m);
  EXPECT_EQ(m * l * k, (unsigned)sk2.data.size());
  EXPECT_EQ(m * l, (unsigned)pos2.size());


  for(unsigned i = 0; i < m * l * k; ++i) {
    EXPECT_EQ(sk1.data[i], sk2.data[i]);
  }

  for(unsigned i = 0; i < m * l; ++i) {
    EXPECT_EQ(pos1[i], pos2[i]);
  }
}

TEST(OMH, Circular) {
  static const size_t size = 1000;
  static const unsigned k = 7, l = 3, m = 500;
  seed_type           seed;
  auto                rng  = seed.seeded();
  sketcher_type sketcher(k, l, m, seed);
  const auto          s    = random_sequence(size, rng);
  const auto          rs   = s.substr(size/2) + s.substr(0, size/2);

  const auto sks = sketcher.compute(s);
  const auto skr = sketcher.compute(rs);

  const double lscore = compare_sketches(sks, skr); // Compare linear sequences
  const double cscore = compare_sketches(sks, skr, m, true); // Compare circular sequences

  EXPECT_LT(0.9, cscore);
  EXPECT_GT(0.9, lscore);
}

TEST(OMH, DeBruijn) {
  const auto s1 = std::string("1111011001010000111");
  const auto s2 = std::string("0000101001111011000");
  const unsigned k = 4, l = 2, m = 1000;

  seed_type     seed;
  sketcher_type sketcher(k, l, m, seed);

  const auto sk1 = sketcher.compute(s1);
  const auto sk2 = sketcher.compute(s2);

  // Count number of hash collisions
  const auto cmp = compare_sketches(sk1, sk2);
  EXPECT_NEAR(0.4, cmp, 0.1);
}

TEST(OMH, ReadWrite) {
  static const size_t size = 100;
  static const unsigned k = 7, l = 4, m = 20;
  static const char* name = "test1";

  seed_type         seed;
  auto              rng = seed.seeded();
  const auto        str = random_sequence(size, rng);
  sketcher_type     sketcher(k, l, m, seed);
  std::stringstream st1, st2;
  std::stringstream rcst1, rcst2;

  sketcher.write(st1, std::string(name), str);
  sketcher.write(rcst1, std::string(name), str, true);
  auto sk = sketcher.compute(str);
  auto rcsk = sketcher.compute(str, true);
  sk.name = name;
  sk.write(st2);
  rcsk.name = name;
  rcsk.write(rcst2);

  st1.seekg(0);
  st2.seekg(0);
  rcst1.seekg(0);
  rcst2.seekg(0);

  const auto sk1 = sketch::from_stream(st1);
  const auto sk2 = sketch::from_stream(st2);
  const auto rcsk1 = sketch::from_stream(rcst1);
  const auto rcsk2 = sketch::from_stream(rcst2);

  EXPECT_TRUE(sk == sk1);
  EXPECT_TRUE(sk == sk2);
  EXPECT_EQ(1.0, compare_sketches(sk, sk1));
  EXPECT_TRUE(rcsk == rcsk1);
  EXPECT_TRUE(rcsk == rcsk2);
  EXPECT_EQ(1.0, compare_sketches(rcsk, rcsk1));
}

TEST(OMH, RC) {
  static const size_t   size = 1000;
  static const unsigned k    = 3, l = 2, m = 20;
  seed_type             seed;
  auto                  rng  = seed.seeded();
  sketcher_type         sketcher(k, l, m, seed);

  const std::string seq = random_sequence(size, rng, DNA);
  const std::string rcs = reverse_complement(seq);

  const auto seqk1 = sketcher.compute(seq);
  const auto seqk2 = sketcher.compute(seq, true);
  const auto rcsk1 = sketcher.compute(rcs);
  const auto rcsk2 = sketcher.compute(rcs, true);

  EXPECT_EQ(1.0, compare_sketches(seqk1, seqk2));
  EXPECT_TRUE(1.0 > compare_sketches(seqk1, rcsk1));
  EXPECT_EQ(1.0, compare_sketches(seqk1, rcsk2));
  EXPECT_EQ(1.0, compare_sketches(seqk2, rcsk1));
  EXPECT_EQ(1.0, compare_sketches(seqk2, rcsk2));
  EXPECT_EQ(1.0, compare_sketches(rcsk1, rcsk2));
}

// TEST(OMH, Random) {
//   static const size_t size = 100;
//   const auto s1 = random_sequence(
// }
}
