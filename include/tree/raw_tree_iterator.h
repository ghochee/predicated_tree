#ifndef TREE_RAW_TREE_ITERATOR_H
#define TREE_RAW_TREE_ITERATOR_H

#include "accessor.h"

template <typename T>
template <traversal_order order, side wing>
class raw_tree<T>::iterator
    : public accessor<T>,
      public std::iterator<std::bidirectional_iterator_tag, T> {
  public:
    using accessor<T>::accessor;

    // Construct a 'begin' iterator with 'node' as the root of a tree. This is
    // different from the raw accessors created through the accessor
    // constructors in that there the depth is explicitly specified.
    //
    // Here the depth and actual reference is set to the appropriate position
    // depending on 'order' and 'wing'. For in-order, right-wing, we would set
    // the reference to the rightmost descendant of 'node'.
    iterator(raw_tree<T> &node);

    iterator &operator++();
    iterator operator++(int);

    iterator &operator--();
    iterator operator--(int);
};

#include "raw_tree_iterator.hh"

#endif  // TREE_RAW_TREE_ITERATOR_H
