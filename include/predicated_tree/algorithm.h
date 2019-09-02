#ifndef PREDICATED_TREE_ALGORITHM_H
#define PREDICATED_TREE_ALGORITHM_H

namespace detangled {

/// Return true iff `pos` is pointing to a (sub)tree which has no violations of
/// `HeightPredicate`.
///
/// This is analogous to `std::is_heap` but `std::is_heap` works on random
/// access iterators and makes assumptions about the distance of child nodes.
///
/// @tparam AccessorType is an `accessor` type of type (:-/). It must have
///     standard tree-accessor methods available on it.
/// @tparam HeightPredicate is a BinaryPredicate as described in
///     `predictae_tree` `H`.
template <typename AccessorType, typename HeightPredicate>
bool is_heap(AccessorType pos, const HeightPredicate& = HeightPredicate());

}  // namespace detangled

#include "impl/algorithm.hh"

#endif  // PREDICATED_TREE_ALGORITHM_H
