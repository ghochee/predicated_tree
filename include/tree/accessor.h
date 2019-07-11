#ifndef TREE_ACCESSOR_H
#define TREE_ACCESSOR_H

// An accessor object is used for referencing a node in a raw_tree<T> tree and
// for navigating through the various nodes in the tree. It is a single object
// that can be used to visit through all the nodes in the tree.
// Example usage:
//     raw_tree<T> t(...);
//     ...
//     accessor<T> a(t, 0);
//     a.descendant<side::left>();
//     a.descendant<side::right>();
//     for (; a; a.next<traversal_order::pre, side::left>()) {
//       cout << *a << endl;
//     }
template <typename T>
class accessor {
  public:
    // An accessor which is specifically pointing to 'node' and set at 'depth'.
    //
    // 'depth' of 0 indicates the 'root' node. It can also be given a special
    //     value of '-1' which indicates an accessor which is **linked** to the
    //     tree rooted at 'node' but is currently set to access an invalid
    //     element. This may be used similar to an 'end' accessor or 'npos'.
    //     See NOTEs on various methods for information on how 'movement' of
    //     'end' accessors is handled.
    // 'node' must be part of a raw_tree<T> structure where 'depth' can climbed
    //     upward through valid nodes. A reference to 'node' is maintained
    //     inside this object and hence it must remain valid until at least the
    //     accessor is used to dereference it's value. Tree modifications don't
    //     affect the accessor except in the following ways:
    //     - Removing / deleting 'node'.
    //     - Reshaping the tree which invalidates 'depth'.
    //     Behaviour after these is undefined.
    //
    // If invalid values are supplied then the process is std::aborted.
    //
    // TODO(ghochee): Maybe a raw accessor constructor without testing and a
    // static make_valid_accessor (make_or_abort?) which would do testing and
    // return accessor value which will be rvalue moved into the call-site
    // accessor object. That would ensure we don't incur penalty unless we have
    // to.
    explicit accessor(raw_tree<T> &node, int16_t depth);

    accessor(const accessor &) = default;
    accessor(accessor &&) = default;
    accessor<T> &operator=(const accessor<T> &) = default;
    accessor<T> &operator=(accessor<T> &&) = default;

    // Returns true if we are not at 'end'.
    explicit operator bool() const;
    bool operator==(const accessor &other) const;
    bool operator!=(const accessor &other) const;
    T &operator*() const;
    raw_tree<T> &node() const;
    raw_tree<T> *operator->() const;

    // Movement operations allow this accessor / visitor to move over the tree
    // elements.
    //
    // The functions when possible come in two forms:
    // template <...>
    // ... functionName();
    // ... functionName(...);
    // The former are used when the side and traversal order enums are known at
    // compile time. The latter should be used when these values are determined
    // at runtime.

    // Moves to the parent node if possible.
    // Complexity: O(1).
    // NOTE: Calling up on:
    //   'root' node moves to 'end' node.
    //   'end' node is a no-op.
    void up();
    // Sets accessor to 'root' node.
    // Complexity: Depends on the shape of the tree.
    //   O(log(N)) for perfectly balanced trees.
    //   O(N) for perfectly imbalanced trees.
    //   In between in other cases.
    void root();

    // Moves to the 'wing' child and returns true when possible.
    // Complexity: O(1).
    // NOTE: Calling down<*>() on:
    //   'end' node moves to 'root' node and always returns true.
    template <side wing>
    bool down();
    bool down(side wing);
    // Moves to the furthest descendant on the 'wing' side.
    // Complexity: Similar to 'root'.
    template <side wing>
    void descendant();
    void descendant(side wing);

    // Moves this accessor to the *next* element according to 'order' on 'wing'
    // side.
    // NOTE: Calling next on:
    //   'end' node moves to *first* element of the wing-sided order traversal.
    //   For example:
    //     <in, left> would move to the leftmost descendant of the tree.
    //     <pre, right> would move to the root element of the tree.
    //     ...
    //   Basically the *first* element of the appropriate iteration being
    //   attempted.
    // Complexity:
    //   O(1) amortized but specific instances of 'next' may be more expensive
    //   and cost would depend on the shape of the tree.
    template <traversal_order order, side wing>
    void next();
    void next(traversal_order order, side wing);

    // Returns an accessor pointing to the lowest common ancestor between this
    // and 'other'.
    // If either is 'end' accessor then 'end' is returned.
    // If the two accessors have different 'root's then the result is undefined.
    accessor<T> common_ancestor(const accessor<T> &other) const;

  private:
    // Unsafe movement to the parent node. This should be called by other
    // methods when they have context that guarantees that calling this would
    // not violate correctness.
    void unsafe_up();

    template <side wing>
    void preorder_increment();
    template <side wing>
    void inorder_increment();
    template <side wing>
    void postorder_increment();

    // The node that we are pointing to currently in the tree but if depth_ ==
    // -1 this indicates the root of the tree.
    raw_tree<T> *node_ = nullptr;

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

#include "accessor.hh"

#endif  // TREE_ACCESSOR_H
