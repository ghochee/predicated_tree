#ifndef TREE_RAW_TREE_ITERATOR_H
#define TREE_RAW_TREE_ITERATOR_H

// TODO(ghochee): Make a separate class raw_tree<T>::accessor<...> which allows
// accessing the various spot-wise relative elements of a tree including child
// etc. It returns an accessor. This Iterator can convert to / from and derives
// from / template parameters the accessor. Iterator doesn't add any data
// members of it's own and derives from the std::iterator hierarchy.
template <typename T>
template <traversal_order order, side wing>
class raw_tree<T>::iterator
    : public std::iterator<std::forward_iterator_tag, T> {
  public:
    iterator(const iterator &) = default;

    // Automatic conversions between the various iterator types. This is a
    // transliteration (the position doesn't change only the direction in which
    // it will head will change). This behaviour is similar to forward and
    // reverse iterators.
    template <traversal_order other_order, side other_wing>
    iterator(const iterator<other_order, other_wing> &other);

    bool operator==(const iterator &other) const;
    bool operator!=(const iterator &other) const;
    reference operator*() const;
    iterator &operator++();
    iterator operator++(int);

    void preorder_increment();
    void inorder_increment();
    void postorder_increment();

  private:
    // Equivalent to ::end.
    iterator() = default;
    // Equivalent of ::begin on 'root'.
    explicit iterator(raw_tree &root);
    // An iterator which is specifically pointing to 'node' and set at 'depth'.
    // This may be used when the call site already knows enough to actually
    // construct a pointer to the right object with the right depth.
    //
    // TODO(ghochee): This should be public to allow clients of the overall
    // library to use externally available information when creating the
    // iterator. We already allow public conversion between one type of
    // iterator to another.
    explicit iterator(raw_tree *node, int16_t depth)
        : node_(node), depth_(depth) {}

    friend class raw_tree<T>;

    // The node that we are pointing to currently in the tree but if depth_ ==
    // -1 this indicates the root of the tree.
    raw_tree *node_ = nullptr;

    // Depth indicates the depth of the node in our tree. Root node has a depth
    // of 0 and all subsequent values are 1 more. The only valid negative value
    // is -1 which indicates that the node is at the end. In such a situation
    // the node_ is set to the root.
    //
    // Having this has a few benefits:
    // - Without a second field we will have to set node_ to a tree-neutral
    //   value to indicate end of iterator. This means we would store a pointer
    //   to a value which is completely disconnected from the tree and hence
    //   can never lead us back to the tree. This is normally fine but we would
    //   do well with this because we can reverse iterate if we have access to
    //   the original tree element.
    // - This allows us to have iterators which range only over a subtree. This
    //   seems like a pretty useful feature for writing more complex algorithms.
    // - Depth is a useful feature when iterating through a tree. Though
    //   clients can set it as part of their T value and the updates etc. are
    //   expected to update them it would be convoluted to bake it together.
    // TODO(ghochee): Should this be part of every iterator or should we have a
    // different class for having this feature as well, or is this a feature
    // for a wrapper to T that we provide to all clients and the generic
    // iterator would use it when T's type traits make it possible.
    int16_t depth_ = -1;
};

#include "raw_tree_iterator.hh"

#endif  // TREE_RAW_TREE_ITERATOR_H
