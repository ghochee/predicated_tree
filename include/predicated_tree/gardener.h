#ifndef TREE_GARDENER_H
#define TREE_GARDENER_H

#include "predicated_tree.h"

namespace detangled {

/// Transforms trees. This normally involves converting trees with one
/// predicate system to another predicate system. Base case is simply to strip
/// out the tree from the predicates and insert into another tree which has the
/// destination predicates, but partial specializations exist for when LOut ==
/// LIn, HIn == indifferent etc.
///
/// When `LOut == LIn` heaps `tree` using `HOut` predicate while keeping
/// relative `L` order unchanged.
template <typename T, typename HIn, typename LIn, typename HOut, typename LOut>
class gardener {
  public:
    static predicated_tree<T, HOut, LOut> craft(
        predicated_tree<T, HIn, LIn> &ptree, const HOut = HOut(),
        const LOut = LOut());
};

}  // namespace detangled

#include "impl/gardener.hh"

#endif  // TREE_GARDENER_H
