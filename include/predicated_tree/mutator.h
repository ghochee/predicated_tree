#ifndef TREE_MUTATOR_H
#define TREE_MUTATOR_H

#include <functional>
#include <optional>

namespace detangled {

template <typename T, typename Comparator>
class mutator {
  public:
    mutator(const Comparator = Comparator());

    /// Takes the element at `root` and heaps it down until it reaches it's
    /// appropriate position. Doesn't affect inorder sequencing of elements in
    /// the tree.
    accessor<raw_tree<T>> sift_down(raw_tree<T> &root);

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
