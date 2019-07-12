#ifndef TREE_ITERATOR_H
#define TREE_ITERATOR_H

#include "accessor.h"

template <class C, traversal_order order, side wing>
class iterator : public virtual_accessor<C>,
                 public std::iterator<std::bidirectional_iterator_tag,
                                      typename C::value_type> {
  public:
    using base_type = virtual_accessor<C>;
    using container_type = typename base_type::container_type;
    using node_type = typename base_type::node_type;
    using value_type_t = typename base_type::value_type_t;

    using base_type::virtual_accessor;

    // Construct a 'begin' iterator with 'node' as the root of a tree. This is
    // different from the raw accessors created through the accessor
    // constructors in that there the depth is explicitly specified.
    //
    // Here the depth and actual reference is set to the appropriate position
    // depending on 'order' and 'wing'. For in-order, right-wing, we would set
    // the reference to the rightmost descendant of 'node'. See
    // accessor<T>::next NOTE regarding 'end' and general documentation in the
    // file about 'end' for more information.
    iterator(node_type &node);

    iterator &operator++();
    iterator operator++(int);

    iterator &operator--();
    iterator operator--(int);
};

#include "iterator.hh"

#endif  // TREE_ITERATOR_H
