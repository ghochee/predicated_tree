#ifndef PREDICATED_TREE_PREDICATED_TREE_H
#define PREDICATED_TREE_PREDICATED_TREE_H

#include <functional>
#include <optional>

#include "util/predicates.h"
#include "util/type_traits.h"

namespace detangled {

/// @addtogroup predicated Predicated Trees
///
/// On top of standard trees we add the notion of *predicates*. This container
/// makes ordering guarantees under these predicates. This would be analogous
/// to giving a predicate to a `std::vector` to make it a `std::multiset` where
/// the `std::multiset` makes ordering guarantees under the predicate.
///
/// `detangled::predicated_tree`s effect this notion by allowing clients to
/// build containers which would maintain upto two predicates. The first
/// predicate is linked to the tree in a *heap tree*-like fashion and the
/// second predicate is linked to the tree in a *binary search tree*-like
/// arrangement.
///
/// Conversion constructors are used for converting from one type to another.
/// For example a tree built with predicates `H0` and `L0` is transformed to a
/// tree with predicates `H1` and `L0` using a simpler operation than necessary
/// when constructing a completely different tree.  This templated constructor
/// allows transformations by exploiting relations between *predicates*.
///
/// This group contains assets which are used for building and using such trees
/// compounded with predicates.
/// @{

/// Predicated trees are containers which make *ordering* guarantees under
/// specified predicates.
///
/// They are containers built on top of standard trees (e.g.
/// `detangled::raw_tree`). They support all operations that a raw_tree would.
///
/// # Essence of Predicated Tree
///
/// The *relation* between a raw_tree and a predicated_tree is analogous to the
/// *relation* between a `std::vector` and an `std::multiset`. This the easiest
/// way to *grok* the essence of predicated_tree. This can be repeated to
/// emphasize.
///
/// @verbatim
/// The relation between a raw_tree and a predicated_tree is analogous to the
/// relation between a std::vector and a std::multiset.
/// @endverbatim
///
/// A `std::multiset` would take a predicate (SWO) and maintain inserted
/// elements under the ordering of the predicate.
///
/// A `predicated_tree` would take (upto) two (possibly orthogonal) predicates.
/// One predicate (`H`) is maintained as a *heap*-like partial ordering. The
/// second property (`L`) is maintained as a left-right (in-order traversal)
/// property. More specifically:
/// - inorder traversal of the tree gives values increasing in `L` predicate.
/// - max-heap-like property is maintained over the `H` predicate.
///
/// The *specification* of the properties which are to be upheld is done by
/// supplying `Height` and `Left` predicates.
///
/// Both predicates are:
/// * Binary predicates meeting the strict-weak-ordering requirements.
/// * Not necessarily related.
///
/// Both the predicates have to support:
///
/// ```
/// bool operator()(const T &first, const T &second) const;
/// ```
///
/// # Simple Examples
///
/// TODO(ghochee): Simple working code example.
///
/// # Special cases
///
/// The following special cases exist for the behaviour of the trees depending
/// on the specified predicates. These special cases are *special* only in the
/// sense of understanding. Available operations and behaviour of the tree
/// container objects remains the same.
///
/// ## Indifferent Predicates
///
/// [indifferent](@ref indifferent) predicates would return false for all
/// comparisons (i.e. not affect ordering in any way). Depending on where
/// client code specifies indifferent predicate we have different behaviours of
/// the predicated_tree.
///
/// | Height Indifferent | Left Indifferent | Result             |
/// |--------------------|------------------|--------------------|
/// |  No                |  No              | Predicated Tree    |
/// |  No                |  Yes             | Max heap           |
/// |  Yes               |  No              | Binary search tree |
/// |  Yes               |  Yes             | Simple tree        |
///
/// If viewed in the above sense, the dual predicate tree seems like a
/// subsumation of the properties of heaps and binary search trees. It is
/// however a little more powerful in that heap modifications are effected
/// keeping in mind the binary search tree property and vice a versa. Put
/// another way, all insertions and deletions of elements in a predicated tree
/// would be done keeping *both* predicates intact.
///
/// ## Identical predicates
///
/// If the same predicates are specified for `height` and `left` then we get
/// the equivalent of the linked list.
///
/// # Handling Equality
///
/// Values are 'equal' on a predicate if `P(a, b)` and `P(b, a)` are both
/// false. Values can be `H`-equal, `L`-equal or just equal if they are equal
/// on both `H` and `L`.
///
/// A pair of equal nodes are placed in parent-left-child arrangement. All
/// subsequent equal nodes are inserted in the right subtree of the lower node.
/// This ensures unambiguous navigation through a constructed tree when
/// searching for values or ranges of values etc.
///
/// # Shape Uniqueness (aside)
///
/// If both H and L are total orderings (viz. exactly one of P(a, b) or P(b, a)
/// is true) then given a set of unique values the tree shape generated by this
/// data structure is unique. Intuitively the proof of this is as follows:
///
/// Given v0, v1, v2 unique values in order of height (v0 taller than v1 taller
/// than v2). This does not lose us generality because we haven't said anything
/// about their left-ordering and given any three values there will always be a
/// tallest and a second tallest value. There are `3! = 6` sequences possible
/// on left-right ordering of v0, v1, v2. The generated shapes for each are as
/// follows (a>b means a is left of b):
///
/// @verbatim
///
/// v0>v1>v2         v0                  v0>v2>v1         v0
///                   |                                    |
///                   -----v1                              -----v1
///                         |                                    |
///                         -----v2                        v2-----
///
///
/// v1>v0>v2         v0                  v1>v2>v0         v0
///                  |                                    |
///            v1---------v2                        v1-----
///                                                  |
///                                                  -----v2
///
///
/// v2>v0>v1         v0                  v2>v1>v0         v0
///                   |                                    |
///             v2---------v1                        v2-----
///                                                   |
///                                                   -----v1
///
/// @endverbatim
///
/// Other than the listed arrangements no other arrangement is possible which
/// maintains the properties. Note also that when the tallest value is in the
/// middle in the left-ordering (`v1>v0>v2` and `v2>v0>v1`) the height ordering
/// information between `v1` and `v2` is lost. This is fine because height
/// property correctness is only tested from a node through it's ancestors till
/// the root.
///
/// Given the above and a list of values v0, v1, ..., vN for all three
/// consecutive values, exactly one of the above arrangements can be done. Note
/// that when a node has to be on the right (or left) child of another node as
/// described above, this really means that the node must be in the right (or
/// left) *sub-tree*. Each node is part of three consecutive-triples and since
/// the child can be anywhere in a subtree, each nodes position can be 'fixed'
/// to meet all requirements simultaneously.
///
/// @tparam T similar to the `T` param for `detangled::raw_tree<T>`.
/// @tparam H predicate which governs `Height` comparison operations in the
///     tree. `Height`, `H`, *tall*, *short* are nouns used in documentation to
///     refer to this property and outcomes. This is defaulted to `taller` if
///     `has_taller<T>` else it becomes `indifferent`.
/// @tparam L predicate which governs `Left` comparison in the tree. `Left`,
///     `L`, *left*, *right*, *side*, *wing* are nouns used in documentation and
///     code to describe notes related to it. This is defaulted to
///     `std::less<T>` if `has_less<T>` else it becomes `indifferent`.
template <class T,
          class H = typename ::std::conditional<
              has_taller<T>::value, taller<T>, indifferent<T>>::type,
          class L = typename ::std::conditional<
              has_less<T>::value, ::std::less<T>, indifferent<T>>::type>
class predicated_tree {
    static_assert(valid_predicate<T, H>::value,
                  "Specified height predicate cannot work with value type.");
    static_assert(valid_predicate<T, L>::value,
                  "Specified left predicate cannot work with value type.");
  public:
    using value_type = T;

    /// Create an empty tree which behaves under the influence of `H` and `L`.
    /// This tree can subsequently be modified with `insert` and `erase`
    /// operations.
    predicated_tree(const H = H(), const L = L());

    /// Takes ownership of `tree` which is assumed to be correct under `H` and
    /// `L`. This is the standard way to construct a `predicated_tree` from a
    /// well behaved (under the predicates) `raw_tree`.
    predicated_tree(raw_tree<T> &&tree, const H = H(), const L = L());

    /// Overload which accepts `predicated_tree`s differing only in the
    /// `Height` predicate. This constructor modifies the incoming tree by
    /// *re-heaping* the nodes according the `H`.
    ///
    /// @tparam HIn which is the height predicate of the incoming tree.
    template <class HIn>
    predicated_tree(predicated_tree<T, HIn, L> &&ptree, const H = H(),
                    const L = L());

    // The following two methods are equivalent to the following:
    // - Disregard the height property of 'value'.
    // - Navigate to the leaf node to which value would attach using only
    //   left-ness.
    // - Consider the ancestry path P starting from this leaf node to the root
    //   traced through parent pointers.
    //
    // P is a non-decreasing-in-height sequence of values.
    // 'lower_bound' is the equivalent of lower_bound(value) on the sequence P.
    // 'upper_bound' is the equivalent of upper_bound(value) on the sequence P.
    //
    // lower_bound is also lower (in height) than the upper_bound which is
    // higher (in height).
    //
    // Similar to STL bounds, lower_bound == upper_bound if value is absent.
    // [lower_bound, upper_bound) form an 'equal' range. Note that inorder
    // traversal from lower_bound through upper_bound will give all the
    // equal-valued nodes. Note that this guarantee is met even if nodes are
    // not constructed as described in class description (right subtree of
    // lower_bound being all equal nodes). If all equal nodes form a left
    // leaning chain the bounds and in-order traversal properties will still be
    // correct.
    //
    // NOTE: There is an important difference between lower_bound in standard
    // STL sequences and lower_bound here. In STL sequences lower_bound is
    // reached first from the search start point (::begin typically). Here the
    // upper_bound is encountered first.
    //
    // Complexity:
    //   O(lg(N)).
    // TODO(ghochee): Take hint positions.
    accessor<const raw_tree<T>> upper_bound(const T &value) const;
    accessor<const raw_tree<T>> lower_bound(const T &value) const;
    // Same as above but returns 'end' if 'value' is not found in the tree.
    accessor<const raw_tree<T>> find(const T &value) const;

    // Insert 'value' into a tree. 'pos' is a hint for the location where we
    // might start our search. If 'pos' is incorrect, insertion is still done
    // at the right location but with an extra cost.
    // Complexity:
    //   O(lg(N)).
    // @tparam U is a type compatible with T for construction.
    // @param value is type and category which can be used for construction of
    //   T type objects.
    template <typename U>
    accessor<const raw_tree<T>> insert(
        U &&value,
        accessor<const raw_tree<T>> hint = accessor<const raw_tree<T>>());
    // Insert 'start' and 'end' range of iterators at 'pos' or subtree of 'pos'.
    // Complexity:
    //   O(D * lg(N + D)) where D is std::distance(start, end).
    template <typename It>
    accessor<const raw_tree<T>> insert(
        It begin, It end,
        accessor<const raw_tree<T>> pos = accessor<const raw_tree<T>>());

    // Erase the element pointed to by 'pos'. No-op if 'pos' is 'end'.
    // Complexity:
    //   O(lg(N)).
    void erase(accessor<const raw_tree<T>> pos);

    // Removes all values from the tree.
    void clear();

    // Returns true iff tree has nodes.
    explicit operator bool() const;

    // Get the value at root.
    const T &operator*() const;

    // Return a pointer to the underlying tree root for performing operations.
    const raw_tree<T> *operator->() const;
    const raw_tree<T> &node() const;

    // Releases ownership of the underlying tree to the call site. This is the
    // only way to get a mutable reference to the underlying tree. This ensures
    // client code cannot meddle with the tree while it is under the care of
    // `predicated_tree`.
    raw_tree<T> release();

  private:
    /// Returns true iff `value` is supposed to lie in the subtree rooted at
    /// `pos`. This is always true if `pos` points to a root node. In other
    /// cases we check the ancestral values of `pos` to see the range of values
    /// which should lie in the subtree and then respond.
    bool in_subtree(accessor<const raw_tree<T>> pos, const T &value) const;

    template <side wing>
    std::optional<raw_tree<T>> clip(accessor<raw_tree<T>> node,
                                    const T &value) const;

    /// Returns true iff `higher` should be on the `wing` side of `lower`
    /// value. This is true if left(higher, lower) is true when `wing` is
    /// `side::left`. When `side::right` then lower must not be greater than
    /// higher (i.e. left(higher, lower) must not be true).
    template <side wing>
    bool horizontal(const T &higher, const T &lower) const;

    bool equal(const T &first, const T &second) const;

    ::std::optional<raw_tree<T>> tree_;

  public:
    const H tall;
    const L left;
};

/// @}

// Class template deduction guides to simplify initialization of trees with
// relevant predicate information.
template <typename H>
predicated_tree(H h)
    ->predicated_tree<
        typename predicate_value_type<decltype(&H::operator())>::value_type, H>;

template <typename H, typename L>
predicated_tree(H h, L l)
    ->predicated_tree<
        typename predicate_value_type<decltype(&H::operator())>::value_type, H,
        L>;

// A very tiny lightweight wrapper around a function reference.
template <class T, bool (*F)(const T &, const T &)>
struct wrapper {
    inline bool operator()(const T &first, const T &second) const {
        return F(first, second);
    }
};

/// Macro to generate the wrapper type from a function reference. This macro is
/// essential to allow deducing argument type information from the function
/// reference and supplying it to the `wrapper` class template.
#define wrap_t(FUNC) \
    wrapper<predicate_value_type<decltype(&FUNC)>::value_type, FUNC>

/// Macro to generate functor (object) from a function reference. Same as
/// `wrap_t` except this generates an object instead of just the type.
/// 
/// Typical usage:
///
///     bool taller_elements(const string &, const string &);
///     ...
///     predicated_tree p{wrap(taller_elements)};
/// 
/// will create `p` as a `predicated_tree` of `string` elements with `H` set to
/// `taller_elements`.
#define wrap(FUNC) \
    wrap_t(FUNC)()

}  // namespace detangled

#include "impl/predicated_tree.hh"

#endif  // PREDICATED_TREE_PREDICATED_TREE_H
