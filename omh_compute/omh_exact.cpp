#include <cassert>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iostream>

#include <combination.hpp>
#include <omh_exact.hpp>

mer_count_type count_mers(const std::string seq, const unsigned k) {
  mer_count_type res;

  if(seq.size() >= k) {
    for(unsigned i = 0; i < seq.size() - k + 1; ++i) {
      res[seq.substr(i, k)].push_back(i);
    }
  }

  return res;
}

double jaccard(const mer_count_type& mc1, const mer_count_type& mc2) {
  unsigned inter_size = 0, union_size = 0;
  for(const auto& mc : mc1) {
    ++union_size;
    inter_size += mc2.find(mc.first) != mc2.end();
  }

  for(const auto& mc : mc2)
    union_size += mc1.find(mc.first) == mc1.end();

  return (double)inter_size / (double)union_size;
}


double weighted_jaccard(const mer_count_type& mc1, const mer_count_type& mc2) {
  unsigned inter_size = 0, union_size = 0;
  for(const auto& mc : mc1) {
    const auto   it2  = mc2.find(mc.first);
    const size_t s2   = it2 == mc2.end() ? 0 : it2->second.size();
    inter_size       += std::min(mc.second.size(), s2);
    union_size       += std::max(mc.second.size(), s2);
  }

  for(const auto& mc : mc2) {
    const auto it1 = mc1.find(mc.first);
    if(it1 == mc1.end())
      union_size += mc.second.size();
  }

  return (double)inter_size / (double)union_size;
}

struct ind_pos {
  unsigned index, pos;
};
bool operator<(const ind_pos& x, const ind_pos& y) { return x.pos < y.pos; }

// Compute the proportion of the common k-mers that are in the same
// relative order between common_kmers1 and common_kmers2.
double proportion_kmers_in_order(const unsigned l,
                                 const std::vector<unsigned>& common_kmers1, const std::vector<unsigned>& common_kmers2) {
  unsigned same    = 0;
  unsigned choices = 0;
  unsigned choose[l];
  ind_pos positions1[l], positions2[l]; // array of (index, position) of the k-mers
  const unsigned inter_size = common_kmers1.size();

  init_combination(choose, choose + l, inter_size);
  do {
    for(unsigned i = 0; i < l; ++i) {
      positions1[i].index = i;
      positions1[i].pos   = common_kmers1[choose[i]];
      positions2[i].index = i;
      positions2[i].pos   = common_kmers2[choose[i]];
    }
    std::sort(positions1, positions1 + l);
    std::sort(positions2, positions2 + l);

    unsigned i = 0;
    for( ; i < l; ++i)
      if(positions1[i].index != positions2[i].index) break;
    same += i == l;
    ++choices;
  } while(next_combination(choose, choose + l, inter_size));

  return (double)same / (double)choices;
}

// // Same as above, simplified for the case l == 2
// double proportion_kmers_in_order3(const std::vector<unsigned>& common_kmers1, const std::vector<unsigned>& common_kmers2) {
//   using std::swap;
//   const unsigned inter_size = common_kmers1.size();
//   unsigned       same       = 0;

//   // Do partial sorting of largest element, then check order
//   for(unsigned i = 0; i < inter_size-2; ++i) {
//       for(unsigned j = i+1; j < inter_size-1; ++j) {
//         for(unsigned k = j+1; k < inter_size; ++k) {
//           if(common_kmers1[i] < common_kmers1[j]) swap(common_kmers1[i], common_kmers1[j]);
//           if(common_kmers1[j] < common_kmers1[k]) swap(common_kmers1[j], common_kmers1[k]);
//           if(common_kmers2[i] < common_kmers2[j]) swap(common_kmers2[i], common_kmers2[j]);
//           if(common_kmers2[j] < common_kmers2[k]) swap(common_kmers2[j], common_kmers2[k]);

//           same += (common_kmers1[i] < common_kmers1[j]) == (common_kmers2[i] < common_kmers2[j]);
//       }
//     }
//   return (double)same / (inter_size*(inter_size - 1)/2);
// }

// Same as above, simplified for the case l == 3
double proportion_kmers_in_order2(const std::vector<unsigned>& common_kmers1, const std::vector<unsigned>& common_kmers2) {
  const unsigned inter_size = common_kmers1.size();
  unsigned       same       = 0;

  for(unsigned i = 0; i < inter_size-1; ++i) {
      for(unsigned j = i+1; j < inter_size; ++j) {
        same += (common_kmers1[i] < common_kmers1[j]) == (common_kmers2[i] < common_kmers2[j]);
      }
    }
  return (double)same / (inter_size*(inter_size - 1)/2);
}

double omh_exact(const std::string seq1, const std::string seq2,
                 const unsigned k, const unsigned l) {
  const auto kmers1 = count_mers(seq1, k);
  const auto kmers2 = count_mers(seq2, k);

  switch(l) {
  case 0: return jaccard(kmers1, kmers2);
  case 1: return weighted_jaccard(kmers1, kmers2);
  default: break;
  }

  // Find the common k-mers and there positions in seq1 and seq2, and
  // compute the size of the union of uniquefied k-mers.
  size_t union_size = 0;
  std::vector<unsigned> common_kmers1, common_kmers2;
  for(auto it1 = kmers1.cbegin(); it1 != kmers1.cend(); ++it1) {
    const auto it2 = kmers2.find(it1->first);
    if(it2 == kmers2.end()) {
      union_size += it1->second.size();
    } else {
      union_size += std::max(it1->second.size(), it2->second.size());
      unsigned len = std::min(it1->second.size(), it2->second.size());
      common_kmers1.insert(common_kmers1.end(), it1->second.begin(), it1->second.begin() + len);
      common_kmers2.insert(common_kmers2.end(), it2->second.begin(), it2->second.begin() + len);
    }
  }
  for(auto it2 = kmers2.cbegin(); it2 != kmers2.cend(); ++it2) {
    const auto it1 = kmers1.find(it2->first);
    if(it1 == kmers1.end()) ++union_size;
  }
  auto inter_size = common_kmers1.size();

  // Compute probability that vectors contain elements from the intersection
  if(inter_size < l) return 0.0;
  double wjl = 1.0;
  for(unsigned i = 0; i < l; ++i)
    wjl *= (double)(inter_size - i) / (double)(union_size - i);

  // For every combination of k-mer choice, how many time in same order.
  const double in_order =
    (l == 2) ? proportion_kmers_in_order2(common_kmers1, common_kmers2) : proportion_kmers_in_order(l, common_kmers1, common_kmers2);

  return wjl * in_order;
}

