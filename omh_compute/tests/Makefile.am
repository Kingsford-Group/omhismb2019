#########
# TESTS #
#########
check_PROGRAMS = all_tests
TESTS = all_tests

all_tests_SOURCES = tests/test_combination.cc			 \
                    tests/test_combination.cc tests/test_glob.cc \
                    tests/test_omh.cc tests/test_omh_exact.cc	 \
                    tests/test_seed_prg.cc tests/common.cc	 \
                    tests/common.hpp
all_tests_CXXFLAGS = $(AM_CXXFLAGS) $(GTEST_CFLAGS) -I$(top_srcdir)/tests
all_tests_LDFLAGS = $(GTEST_LDFLAGS)
all_tests_LDADD = $(builddir)/libomh.a $(GTEST_LIBS) -lgtest -lgtest_main
