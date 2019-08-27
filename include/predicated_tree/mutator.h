#ifndef TREE_MUTATOR_H
#define TREE_MUTATOR_H

#include <functional>
#include <optional>

namespace detangled {

/// A mutator class object is strongly linked to the properties of a
/// `detangled::predicated_tree`.
///
/// Mutator objects allow *effecting* the properties of the
/// `detangled::predicated_tree` but also provide some other facilities which a
/// `detangled::predicated_tree` doesn't use directly.
///
/// Typical usage:
/// ```
///     struct Taller;
///     struct Lefter;
///     ...
///     raw_tree<T> r(...);
///     ...
///     mutator<T> m(comparator<Taller, Lefter>());
///     ...
///     m.insert(...);
///     m.insert(...);
///     m.insert(...);
///     m.insert(...);
///     ...
///     m.erase(accessor<raw_tree<T>>(r, 0));
///     ...
/// ```
///
/// The most effective usage of this class is as part of
/// `detangled::predicated_tree`.
template <typename T, typename Comparator>
class mutator {
  public:
    mutator(const Comparator = Comparator());

    bool in_subtree(const raw_tree<T> &pos, const T &value) const;

    template <class ContainerType>
    accessor<ContainerType> upper_bound(ContainerType &tree,
                                        const T &value) const;
    template <class ContainerType>
    accessor<ContainerType> lower_bound(ContainerType &tree,
                                        const T &value) const;
    template <class ContainerType>
    accessor<ContainerType> find(ContainerType &tree, const T &value) const;
    accessor<raw_tree<T>> insert(
        T &&value, raw_tree<T> &node,
        accessor<const raw_tree<T>> pos = accessor<const raw_tree<T>>());
    template <typename It>
    accessor<raw_tree<T>> insert(
        It begin, It end, raw_tree<T> &node,
        accessor<const raw_tree<T>> pos = accessor<const raw_tree<T>>());
    void erase(raw_tree<T> &node, accessor<const raw_tree<T>> pos);

    /// Takes the element at `root` and heaps it down until it reaches it's
    /// appropriate position. Doesn't affect inorder sequencing of elements in
    /// the tree.
    accessor<raw_tree<T>> sift_down(raw_tree<T> &root);

    /// Same as `sift_down` but this sifts down all the way until this node can
    /// become a leaf. This would be equivalent to sifting down as if the value
    /// at `root` is the lowest possible value in the `H` predicate.
    accessor<raw_tree<T>> sift_out(raw_tree<T> &root);

    /// Clips out the *extremal* part of the tree w.r.t value. Specifcally if
    /// `value` is less than node then returns a tree which comprises all
    /// elements *smaller* than `value`. If `value` is greater than that at node
    /// then returns a tree comprising elements *greater* than `value`.
    ///
    /// Returns `std::optional` because `value` may be smaller (or greater)
    /// than all the values in the tree in which case we have to return an
    /// empty tree.
    std::optional<raw_tree<T>> clip(raw_tree<T> &node, const T &value) const;

    /// `first` and `second` trees are set under H and L. This operation will
    /// merge the two trees to make a single tree which also meets the H and L
    /// criteria.
    /// FIXME(ghochee): Implementation pending.
    raw_tree<T> merge(raw_tree<T> &&first, raw_tree<T> &&second);

    /// Heaps `tree` using `H` predicate while keeping relative `L` order
    /// unchanged. For `detangled::raw_tree<T>` value `t`, the following:
    /// ```
    ///     H h(...); L l(...);
    ///     std::sort(t.inlbegin(), t.inlend(), l);
    ///     mutator<t::value_type, ...> m(h, l);
    ///     m.stable_heap(t);
    /// ```
    /// would result in `t` being correct in both predicates.
    void stable_make_heap(raw_tree<T> &tree);

    /// Sorts 'tree' using 'L' predicate while keeping relative 'H' order
    /// unchanged. For raw_tree<T> value 't', the following:
    ///
    ///     H h(...); L l(...);
    ///     std::make_heap(t.prerbegin(), t.prerend(), l);
    ///     mutator<t::value_type, ...> m(h, l);
    ///     m.stable_sort(t);
    ///
    /// would result in 't' being correct in both predicates.
    /// FIXME(ghochee): Implementation pending.
    void stable_sort(raw_tree<T> &tree);

  private:
    template <side wing>
    raw_tree<T> zip(raw_tree<T> &&merged, raw_tree<T> &&incoming) const;
    template <side wing>
    std::optional<raw_tree<T>> clip(raw_tree<T> &node, const T &value) const;

    const Comparator comparator_;
};

}  // namespace detangled

#include "impl/mutator.hh"

#endif  // TREE_MUTATOR_H
