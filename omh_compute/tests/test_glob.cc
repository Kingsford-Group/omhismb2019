#include <gtest/gtest.h>
#include <unistd.h>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>

#include <glob.hpp>

namespace {
struct unlinktmp {
#define TMPPREFIX "tmpfile"
  char* name;
  int   fd;

  unlinktmp()
    : name(strdup(TMPPREFIX "XXXXXX"))
    , fd(name ? mkstemp(name) : -1)

  {
    if(!name || fd == -1)
      throw std::bad_alloc();
  }
  ~unlinktmp() {
    unlink(name);
    free(name);
  }
};

TEST(GLOB, Exists) {
  unlinktmp tmpfile;
  glob_files doto(TMPPREFIX "*");

  EXPECT_LE(1, doto.size());
  EXPECT_TRUE(std::any_of(doto.begin(), doto.end(),
                          [&tmpfile](char* file) {
                            return !strcmp(tmpfile.name, file);
                          }));
}

TEST(GLOB, Bad) {
  glob_files bad("___doesntexists___*");
  EXPECT_EQ(0, bad.size());
}
}
