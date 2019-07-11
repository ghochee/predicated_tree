#ifndef TREE_RAW_TREE_ITERATOR_H
#define TREE_RAW_TREE_ITERATOR_H

#include "accessor.h"

template <typename T>
template <traversal_order order, side wing>
class raw_tree<T>::iterator
    : public accessor<T>,
      public std::iterator<std::forward_iterator_tag, T> {
  public:
    using accessor<T>::accessor;
    iterator(raw_tree<T> &node);

    // Automatic conversions between the various iterator types. This is a
    // transliteration (the position doesn't change only the direction in which
    // it will head will change). This behaviour is similar to forward and
    // reverse iterators.
    template <traversal_order other_order, side other_wing>
    iterator(const iterator<other_order, other_wing> &other);

    iterator &operator++();
    iterator operator++(int);
};

#include "raw_tree_iterator.hh"

#endif  // TREE_RAW_TREE_ITERATOR_H
