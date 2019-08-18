#ifndef TREE_TRAVERSER_H
#define TREE_TRAVERSER_H

/// traverser implements a single static method 'next'.
///
/// Typical usage:
/// ```
///     Accessor<...> a(...);
///     ...
///     traverser<decltype(a), traversal_order::pre, side::left>::next(a);
///     ...
/// ```
/// which would move the `a` accessor to the next inorder position when starting
/// from left side.
///
/// This functionality could be in a few other locations instead of as is, the
/// following notes cover some details:
/// - This is not a static function because we need to do partial
///   specialization.
/// - This is not a member function of `::accessor` because there are multiple
///   types of accessors which implement the accessor *concept* with varying
///   degrees of efficiency. Doing an inorder traversal of such an accessor has
///   nothing to do with the accessor itself and making this functionality a
///   member function would result in code duplication.
/// - This is not a part of `::iterator` because given an accessor the ability
///   to *traverse* it to the *next* inorder position is different from a
///   pre-baked C++ construct which ties the accessor along with the traversal
///   order. OTOH this functionality will definitely need to be used in the
///   iterator.
/// - The other way to implement it is using CRTP but that would result in less
///   readability (more structs, more template parameters).
///
/// @tparam order is the `::traversal_order` which this class template will
///     follow.
/// @tparam wing denotes the side which the we prefer in the traversal.
template <typename A, traversal_order order, side wing>
class traverser {
  public:
    /// Creates an accessor which points to the *first* element according to
    /// `order` on `wing`.
    ///   For example:
    ///     <in, left> would move to the leftmost descendant of the tree.
    ///     <pre, right> would move to the root element of the tree.
    ///     ...
    /// Complexity:
    ///   O(1) for preorder, O(lg(n)) for others.
    template <typename Container>
    static A begin(Container &c);

    /// Moves this accessor to the *next* element according to `order` on
    /// `wing` side.
    ///
    /// @note Calling next on *end* node attempts to move to the *first*
    /// element of the wing-sided order traversal. This may fail as an
    /// operation (but not compilation) if the accessor the operation is
    /// performed on is not bidirectional.
    ///
    /// Complexity:
    ///   O(1) amortized but specific instances of `traverser::next` may be
    ///   more expensive and cost would depend on the shape of the tree.
    static void next(A &a);
};

#include "impl/traverser.hh"

#endif  // TREE_TRAVERSER_H
