
#include <omh.hpp>
#include <iostream>

char comp(char c) {
  switch(c) {
  case 'a': return 't';
  case 'c': return 'g';
  case 'g': return 'c';
  case 't': return 'a';
  case 'A': return 'T';
  case 'C': return 'G';
  case 'G': return 'C';
  case 'T': return 'A';
  }
  return c;
}

std::string reverse_complement(const std::string& sequence) {
  std::string res(sequence.size(), '\0');

  auto sf = sequence.begin();
  auto sl = sequence.end() - 1;
  auto rf = res.begin();
  auto rl = res.end() - 1;

  for( ; sf < sl; ++sf, --sl, ++rf, --rl) {
    *rf = comp(*sl);
    *rl = comp(*sf);
  }
  if(sf == sl) {
    *rf = comp(*sf);
  }
  return res;
}

double compare_sketch_pair(const char* p1, const char* p2, unsigned m, unsigned k, unsigned l, bool circular) {
  const unsigned block = std::max(l, (unsigned)1) * k;
  unsigned count = 0;
  if(!circular || l < 2) {
    for(unsigned i = 0; i < m; ++i, p1 += block, p2 += block)
      count += memcmp(p1, p2, block) == 0;
  } else {
    for(unsigned i = 0; i < m; ++i, p1 += block, p2 += block) {
      for(unsigned j = 0; j < l; ++j) {
        if(memcmp(p1, p2 + j * k, block - j * k) == 0 && memcmp(p1 + block - j * k, p2, j * k) == 0) {
          ++count;
          break;
        }
      }
    }
  }
  return (double)count / m;
}

double compare_sketches(const sketch& sk1, const sketch& sk2, ssize_t m, bool circular) {
  if(sk1.k != sk2.k || sk1.l != sk2.l) return -1; // Different k or l
  if(m < 0) m = std::min(sk1.m, sk2.m);
  if(m > sk1.m || m > sk2.m) return -1;  // Too short

  const unsigned block = std::max(sk1.l, (unsigned)1) * sk1.k;
  if(sk1.data.size() < m * block || sk2.data.size() < m * block) return -1; // Truncated
  const double fwd_score = compare_sketch_pair(sk1.data.data(), sk2.data.data(), m, sk1.k, sk1.l, circular);

  double bwd_score = 0.0;
  if(!sk1.rcdata.empty()) {
    bwd_score = compare_sketch_pair(sk1.rcdata.data(), sk2.data.data(), m, sk1.k, sk1.l, circular);
  } else if(!sk2.rcdata.empty()) {
    bwd_score = compare_sketch_pair(sk1.data.data(), sk2.rcdata.data(), m, sk1.k, sk1.l, circular);
  }

  return std::max(fwd_score, bwd_score);
}

void sketch::read(std::istream& is) {
  if(is.get() != '>') throw std::runtime_error("Invalid format, expecting '>'");
  bool rc;
  is >> name >> k >> l >> m >> rc;
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  if(!is.good()) throw std::runtime_error("Invalid header");
  data.resize(std::max(l, (unsigned)1) * m * k);
  is.read(data.data(), data.size());
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  if(!is.good()) throw std::runtime_error("Error reading sketch");
  if(rc) {
    rcdata.resize(std::max(l, (unsigned)1) * m * k);
    is.read(rcdata.data(), rcdata.size());
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if(!is.good()) throw std::runtime_error("Error reading rc sketch");
  }
}

void sketch::write(std::ostream& os) const {
  os << '>' << name << ' ' << k << ' ' << l << ' ' << m << ' ' << !rcdata.empty() << '\n';
  os.write(data.data(), data.size());
  os << '\n';
  if(!rcdata.empty()) {
    os.write(rcdata.data(), rcdata.size());
    os << '\n';
  }
}
