#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <random>
#include <cstring>

#include <seeded_prg.hpp>
#include <omh.hpp>

// Types
typedef LongSeed<std::mt19937_64> seed_type;
typedef omh_sketcher<std::mt19937_64> sketcher_type;

// De Bruijn sequences for k=4
extern const std::string dbg1;
extern const std::string dbg2;

extern const char* alphabet;
extern const char* DNA;
template<typename EngineT>
std::string random_sequence(size_t size, EngineT& rng, const char* letters = alphabet) {
  std::string str;

  std::uniform_int_distribution<char> rng_letter(0, strlen(letters) - 1);
  for(size_t i = 0; i < size; ++i)
    str += letters[rng_letter(rng)];

  return str;
}


#endif /* __COMMON_H__ */
