#ifndef __OMH_H__
#define __OMH_H__

#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <limits>

#include <xxhash.hpp>
#include <seeded_prg.hpp>

std::string reverse_complement(const std::string&);

struct mer_info {
  size_t pos;
  uint64_t hash;
  unsigned occ;
  mer_info(size_t p, unsigned o, uint64_t h)
    : pos(p)
    , hash(h)
    , occ(o)
  { }
};

// Compute the position in sequence of the k-mers picked by omh, and
// passed them 1 by 1 to block. block takes 3 arguments: i \in [m], j
// \in [l] and the position.
template<typename EngineT, typename BT>
void omh_pos(const std::string& seq, unsigned k, unsigned l, unsigned m, EngineT& prg, BT block) {
  if(seq.size() < k) return;
  const bool weight = l > 0;
  if(l == 0) l = 1;

  std::vector<mer_info> mers;
  std::unordered_map<std::string, unsigned> occurrences;
  size_t pos[l];

  //  Create list of k-mers with occurrence numbers
  for(size_t i = 0; i < seq.size() - k + 1; ++i) {
    auto occ = occurrences[seq.substr(i, k)]++;
    mers.emplace_back(i, occ, (uint64_t)0);
  }

  xxhash hash;
  for(unsigned i = 0; i < m; ++i) {
    const auto seed = prg();
    for(auto& meri : mers) {
      hash.reset(seed);
      hash.update(&seq.data()[meri.pos], k);
      if(weight) hash.update(&meri.occ, sizeof(meri.occ));
      meri.hash = hash.digest();
    }

    std::partial_sort(mers.begin(), mers.begin() + l, mers.end(), [&](const mer_info& x, const mer_info& y) { return x.hash < y.hash; });
    std::sort(mers.begin(), mers.begin() + l, [&](const mer_info& x, const mer_info& y) { return x.pos < y.pos; });
    for(unsigned j = 0; j < l; ++j)
      block(i, j, mers[j].pos);
  }
}

struct sketch {
  std::string       name;
  unsigned          k, l, m;
  std::vector<char> data;
  std::vector<char> rcdata;

  bool operator==(const sketch& rhs) const {
    return k == rhs.k && l == rhs.l && m == rhs.m && data == rhs.data && rcdata == rhs.rcdata;
  }

  // Read a sketch into this
  void read(std::istream& is);

  // Read a sketch
  static sketch from_stream(std::istream& is) {
    sketch sk;
    sk.read(is);
    return sk;
  }

  // Write sketch
  void write(std::ostream& os) const;
};

template<typename EngineT = std::mt19937_64>
class omh_sketcher {
protected:
  unsigned           m_k, m_l, m_m;
  LongSeed<EngineT>& m_seed;
  EngineT            m_prg;

  inline void write_sketch(std::ostream& os, const std::string& seq) {
    omh_pos(seq, m_k, m_l, m_m, m_prg, [&os, &seq, this](unsigned i, unsigned j, size_t pos) { os.write(seq.data() + pos, m_k); });
  }

  inline void compute_sketch(char* ptr, const char* seq) {
    omh_pos(seq, m_k, m_l, m_m, m_prg,
            [&ptr, &seq, this](unsigned i, unsigned j, size_t pos) { memcpy(ptr, seq + pos, m_k); ptr += m_k; });
  }

public:
  omh_sketcher(unsigned k, unsigned l, unsigned m, LongSeed<EngineT>& seed)
    : m_k(k), m_l(l), m_m(m)
    , m_seed(seed)
  { }

  // Compute the positions in seq of the k-mers picked by omh and append then to res
  void pos(const std::string& seq, std::vector<size_t>& res) {
    m_seed(m_prg);
    omh_pos(seq, m_k, m_l, m_m, m_prg, [&res](unsigned i, unsigned j, size_t pos) { res.push_back(pos); });
  }

  // Compute the positions in seq and return vector
  inline std::vector<size_t> pos(const std::string& seq) {
    std::vector<size_t> res;
    pos(seq, res);
    return res;
  }

  // Write sketch to an ostream
  void write(std::ostream& os, const std::string& name, const std::string& seq, bool rc = false) {
    m_seed(m_prg);
    os << '>' << name << ' ' << m_k << ' ' << m_l << ' ' << m_m << ' ' << rc << '\n';
    write_sketch(os, seq);
    os << '\n';
    if(rc) {
      m_seed(m_prg);
      std::string rcseq = reverse_complement(seq);
      write_sketch(os, rcseq);
      os << '\n';
    }
  }

  // Sketch from sequence
  void compute(const std::string& seq, sketch& sk, bool rc = false) {
    sk.k = m_k;
    sk.l = m_l;
    sk.m = m_m;
    sk.data.resize(std::max(sk.l, (unsigned)1) * sk.m * sk.k);
    m_seed(m_prg);
    compute_sketch(sk.data.data(), seq.data());

    if(rc) {
      m_seed(m_prg);
      std::string rcseq = reverse_complement(seq);
      sk.rcdata.resize(sk.l * sk.m * sk.k);
      compute_sketch(sk.rcdata.data(), rcseq.data());
    }
  }

  // Sketch from sequence
  sketch compute(const std::string& seq, bool rc = false) {
    sketch sk;
    compute(seq, sk, rc);
    return sk;
  }

  double compare(const std::string& s1, const std::string& s2, bool rc = false) {
    const auto sk1 = compute(s1, rc);
    const auto sk2 = compute(s2, rc);
    return compare_sketches(sk1, sk2);
  }
};


// Compare 2 sketches. Return agreement in [0, 1]
double compare_sketches(const sketch& sk1, const sketch& sk2, ssize_t m = -1, bool circular = false);


#endif /* __OMH_H__ */
