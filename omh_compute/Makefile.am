ACLOCAL_AMFLAGS = -Im4

# Yaggo automatic rules with silencing
V_YAGGO = $(V_YAGGO_$(V))
V_YAGGO_ = $(V_YAGGO_$(AM_DEFAULT_VERBOSITY))
V_YAGGO_0 = @echo "  YAGGO   " $@;
.yaggo.hpp:
	$(V_YAGGO)$(YAGGO) -o $@ $<

YAGGO_SOURCES = $(patsubst %,%.hpp, $(bin_PROGRAMS))
BUILT_SOURCES = $(YAGGO_SOURCES)
noinst_HEADERS = $(YAGGO_SOURCES)

AM_DEFAULT_SOURCE_EXT = .cc
AM_CXXFLAGS = -I$(top_srcdir) $(XXHASH_CFLAGS)
AM_LDFLAGS = -pthread $(XXHASH_LDFLAGS)
LIBS = $(XXHASH_LIBS)
bin_PROGRAMS = omh_sketch omh_compare_sketch omh_compare_exact	\
               omh_compare_exact_seq omh_compare_sketches

noinst_LIBRARIES = libomh.a
libomh_a_SOURCES = omh.cpp omh.hpp omh_exact.cpp omh_exact.hpp	 \
                   parse_file.cpp parse_file.hpp combination.hpp \
                   xxhash.hpp seeded_prg.hpp glob.hpp
LDADD = $(builddir)/libomh.a

include tests/Makefile.am
