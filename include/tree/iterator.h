#ifndef TREE_ITERATOR_H
#define TREE_ITERATOR_H

#include "accessor.h"
#include "enums.h"
#include "virtual_accessor.h"

namespace detangled {

/// Objects of this class allow *sequentially* accessing `raw_tree` elements.
///
/// The constructed iterators are *BidirectionalIterator*s and can be
/// decremented from `end` state as well.
///
/// Typical usage:
/// ```
/// raw_tree<uint32_t> tree_elements(...);
/// ...
/// for (auto it = tree_elements.begin<traversal_order::pre, side::left>();
///      it != tree_elements.prelend(); ++it) {
///     ...
/// }
/// ```
///
/// The above allows accessing elements of the tree in pre-order (left first).
/// Note that `prelend` is a mnemonic which constructs
/// `raw_tree::end<traversal_order::pre, side::left>()`. See
/// `::RAW_TREE_MAKE_ALIAS` for details.
///
/// # Sequence definition
///
/// Unlike standard containers like `std::vector` the elements in a tree can be
/// accessed in a number of sequences. The traditionally defined ways are
/// `traversal_order::pre`, `traversal_order::in` and `traversal_order::post`.
/// This iterator template class allows instantiating the correct object
/// factory representing one of these.
///
/// This is a slight generalization over the `iterator`/`reverse_iterator` type
/// pair in standard containers. The generalization would fit better if we
/// called the standard iterators, `detangled::iterator<side::left>` and
/// `detangled::iterator<side::right>` instead.
///
/// @tparam C is the *tree* container type that we are trying to make the
///     iterator for. We specify this to DRY the code that we would have to
///     write for const-iterator,
/// @tparam order is the `::traversal_order` that we wish to follow.
/// @tparam wing is the `::side` of child / subtree nodes that we prefer during
///     traversal.
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

    // Construct a *begin* iterator with `node` as the root of a tree. This is
    // different from the raw `::accessor`s created through the accessor
    // constructors in that there the depth is explicitly specified.
    //
    // Here the depth and actual reference is set to the appropriate position
    // depending on `order` and `wing`. For in-order, right-wing, we would set
    // the reference to the rightmost descendant of `node`. See
    // `::accessor::next` NOTE regarding *end* and general documentation in
    // the file about *end* for more information.
    //
    // @param node is the node at which this iteration will be *root*-ed
    iterator(node_type &node);

    iterator &operator++();
    iterator operator++(int);

    iterator &operator--();
    iterator operator--(int);
};

}  // namespace detangled

#include "impl/iterator.hh"

#endif  // TREE_ITERATOR_H
