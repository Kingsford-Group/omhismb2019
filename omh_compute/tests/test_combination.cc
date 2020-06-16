#include <gtest/gtest.h>
#include <combination.hpp>

namespace  {
TEST(Permutation, Short) {
  std::vector<unsigned> vec(3);

  auto it = init_combination(vec.begin(), vec.end(), 2);
  EXPECT_EQ(vec.begin() + 2, it);
}

// n \choose k, works only for small value
size_t nchoosek(unsigned n, unsigned k) {
  size_t num = 1, den = 1;
  for(unsigned i = 0; i < k; ++i) {
    num *= n - i;
    den *= i + 1;
  }

  return num / den;
}

TEST(Permutation, Size) {
  static const unsigned n = 5, k = 3;
  std::vector<unsigned> vec(k);

  auto it = init_combination(vec.begin(), vec.end(), n);
  EXPECT_EQ(it, vec.end());
  for(unsigned i = 0; i < vec.size(); ++i)
    EXPECT_EQ(i, vec[i]);

  size_t       count         = 0;
  const size_t expected_size = nchoosek(n, k);
  do {
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    ++count;
  } while(count <= expected_size && next_combination(vec.begin(), vec.end(), n));
  EXPECT_EQ(expected_size, count);
}
} // namespace
