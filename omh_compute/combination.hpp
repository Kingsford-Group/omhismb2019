#ifndef __PERMUTATION_H__
#define __PERMUTATION_H__

#include <iterator>

template<typename Iterator>
Iterator init_combination(Iterator first, const Iterator last, const typename std::iterator_traits<Iterator>::value_type n) {
  typename std::iterator_traits<Iterator>::value_type eval = 0;
  for( ; first != last && eval < n; ++first, ++eval)
    *first = eval;
  return first;
}

template<typename Iterator>
bool next_combination(const Iterator first, const Iterator last, const typename std::iterator_traits<Iterator>::value_type n) {
  Iterator cur = last;
  auto eval = n;
  while(cur != first) {
    --cur;
    --eval;
    if(*cur < eval)
      break;
  }

  if(cur == first && *cur == eval) return false;
  eval = ++*cur;
  for(++cur, ++eval; cur != last; ++cur, ++eval)
    *cur = eval;
  return true;
}

#endif /* __PERMUTATION_H__ */
