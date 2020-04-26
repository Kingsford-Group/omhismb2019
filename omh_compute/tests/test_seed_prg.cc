#include <algorithm>
#include <gtest/gtest.h>
#include <seeded_prg.hpp>
#include <common.hpp>
#include <sstream>

namespace {
TEST(seeded_prg, defaultseed) {
  seed_type seed1(false); // Default seed
  seed_type seed2(false); // Default seed
  seed_type seed3(true); // Random seed

  auto prg1 = seed1.seeded();
  auto prg2 = seed2.seeded();
  auto prg3 = seed3.seeded();
  bool equal12 = true;
  bool equal13 = true;
  for(int i = 0; i < 1000; ++i) {
    const auto x1 = prg1();
    equal12 = equal12 && (x1 == prg2());
    equal13 = equal13 && (x1 == prg3());
  }
  EXPECT_TRUE(equal12);
  EXPECT_FALSE(equal13);
}

TEST(seeded_prg, badseed) {
  const char* str1 = "1 2 3 4 5";
  const char* str2 = "toto 2 3";

  std::istringstream is1(str1);
  EXPECT_THROW(seed_type seed1(nullptr, &is1), std::runtime_error);

  std::istringstream is2(str2);
  EXPECT_THROW(seed_type seed1(nullptr, &is2), std::runtime_error);
}

TEST(seeded_prg, readwriteseed) {
  std::stringstream seedout;
  seed_type seed1(&seedout, nullptr, true);
  seedout.seekg(0);
  seed_type seed2(nullptr, &seedout, false);

  auto prg1 = seed1.seeded();
  auto prg2 = seed2.seeded();
  for(int i = 0; i < 1000; ++i)
    EXPECT_EQ(prg1(), prg2());
}

} // namespace
