
#ifndef __OMH_EXACT_H__
#define __OMH_EXACT_H__

#include <unordered_map>
#include <vector>


typedef std::unordered_map<std::string, std::vector<unsigned>> mer_count_type;

mer_count_type count_mers(const std::string seq, const unsigned k);

double jaccard(const mer_count_type& mc1, const mer_count_type& mc2);

double weighted_jaccard(const mer_count_type& mc1, const mer_count_type& mc2);

double proportion_kmers_in_order(const unsigned l,
                                 const std::vector<unsigned>& common_kmers1, const std::vector<unsigned>& common_kmers2);
double proportion_kmers_in_order2(const std::vector<unsigned>& common_kmers1, const std::vector<unsigned>& common_kmers2);

double omh_exact(const std::string seq1, const std::string seq2,
                 const unsigned k, const unsigned l);

#endif /* __OMH_EXACT_H__ */
