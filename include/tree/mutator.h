#ifndef TREE_MUTATOR_H
#define TREE_MUTATOR_H

#include <functional>
#include <optional>

// A mutator object is strongly linked to the properties of a predicated_tree.
// Mutator objects allow effecting the properties of the predicated_tree but
// also provide some other facilities which a predicated_tree doesn't use
// directly.
//
// Typical usage:
//
//     struct Taller;
//     struct Lefter;
//     ...
//     raw_tree<T> r(...);
//     ...
//     Taller t;
//     Lefter l;
//     mutator<T> m(r, t, l);
//     ...
//     m.insert(...);
//     m.insert(...);
//     m.insert(...);
//     m.insert(...);
//     ...
//     m.erase(accessor<raw_tree<T>>(r, 0));
//     ...
//
// The most effective usage of this class is as part of predicated_tree.
template <typename T, typename Comparator>
class mutator {
  public:
    // Copy references to H and L predicates. The references have to be valid
    // for the life of this object.
    mutator(const Comparator = Comparator());

    bool in_subtree(const raw_tree<T> &pos, const T &value) const;

    // BEGIN INTERFACE
    // See 'predicated_tree' interface for documentation.
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
    // END INTERFACE

    // Combines 'left' and 'right' to give a single tree with all the elements
    // in the two trees under predicates H and L. If a represents elements of
    // 'left' and b elements of 'right' l(b, a) is always false. Additionally
    // all transitive properties are maintained.
    //
    // The uncommon name of this method reflects the operation being done,
    // where 'left' and 'right' are correct under H and L and left has elements
    // "less than" right, then the operation is zipping up the two halves along
    // the middle.
    raw_tree<T> zip(raw_tree<T> &&left, raw_tree<T> &&right) const;

    // Clips out the 'extremal' part of the tree w.r.t value. Specifcally if
    // 'value' is less than node then returns a tree which comprises all
    // elements 'smaller' than 'value'. If 'value' is greater than that at node
    // then returns a tree comprising elements 'greater' than 'value'. This is
    // the reverse operation of 'zip' governed by a value.
    //
    // Returns std::optional because 'value' may be smaller (or greater) than
    // all the values in the tree in which case we have to return an empty
    // tree.
    std::optional<raw_tree<T>> clip(raw_tree<T> &node, const T &value) const;

    // 'first' and 'second' trees are set under H and L. This operation will
    // merge the two trees to make a single tree which also meets the H and L
    // criteria.
    raw_tree<T> merge(raw_tree<T> &&first, raw_tree<T> &&second);

    // Heaps 'tree' using 'H' predicate while keeping relative 'L' order
    // unchanged. For raw_tree<T> value 't', the following:
    //
    //     H h(...); L l(...);
    //     std::sort(t.inlbegin(), t.inlend(), l);
    //     mutator<t::value_type, ...> m(h, l);
    //     m.stable_heap(t);
    //
    // would result in 't' being correct in both predicates.
    void stable_make_heap(raw_tree<T> &tree);

    // Sorts 'tree' using 'L' predicate while keeping relative 'H' order
    // unchanged. For raw_tree<T> value 't', the following:
    //
    //     H h(...); L l(...);
    //     std::make_heap(t.prerbegin(), t.prerend(), l);
    //     mutator<t::value_type, ...> m(h, l);
    //     m.stable_sort(t);
    //
    // would result in 't' being correct in both predicates.
    // FIXME(ghochee): Implementation.
    void stable_sort(raw_tree<T> &tree);

  private:
    bool equal(const T &first, const T &second) const;

    template <side wing>
    raw_tree<T> zip(raw_tree<T> &&merged, raw_tree<T> &&incoming) const;
    template <side wing>
    std::optional<raw_tree<T>> clip(raw_tree<T> &node, const T &value) const;

    Comparator comparator_;
};

#include "tree/mutator.hh"

#endif  // TREE_MUTATOR_H
