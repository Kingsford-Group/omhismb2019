#! /bin/sh -e

# Add the proper -I/path/to/include that contains xxhash.h if needed
CXXFLAGS=

g++ $CXXFLAGS    -I. -std=c++11 -O3    -c -o omh.o omh.cpp
g++ $CXXFLAGS    -I. -std=c++11 -O3    -c -o omh_exact.o omh_exact.cpp
g++ $CXXFLAGS    -I. -std=c++11 -O3    -c -o parse_file.o parse_file.cpp
ar sruv    libomh.a omh.o omh_exact.o parse_file.o
g++    -I. -std=c++11 -O3     -L.   omh_sketch.cc  -lomh -lxxhash   -o omh_sketch
g++    -I. -std=c++11 -O3     -L.   omh_compare_sketch.cc  -lomh -lxxhash   -o omh_compare_sketch
g++    -I. -std=c++11 -O3     -L.   omh_compare_exact_seq.cc  -lomh -lxxhash   -o omh_compare_exact_seq
