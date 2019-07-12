#ifndef TREE_ACCESSOR_H
#define TREE_ACCESSOR_H

#include <cstdint>

// An accessor object is used for referencing a node in a raw_tree<T> tree and
// for navigating through the various nodes in the tree. It is a single object
// that can be used to visit through all the nodes in the tree.
//
// Example usage:
//     raw_tree<uint32_t> t(...);
//     ...
//     accessor<raw_tree<uint32_t>> a(t);
//     a.descendant<side::left>();
//     a.descendant<side::right>();
//
// Clients should see this object as a more fundamental object than 'iterator's
// are because these objects don't know 'next' and can navigate freely through
// the container. Freely doesn't imply cheaply, complexity costs are indicated
// on the various methods.
//
// Other than nodes in a tree, accessor objects may be put in a state we call
// 'end' (because typically this is useful when done as an iterator). In this
// state the accessor refers to an invalid object. This accessor may be seen
// like a virtual node which is the parent of the 'root' node of this tre with
// the difference that it is impossible to navigate down after reaching there.
//
// Once in 'end' state doing any operations on the accessor or dereferenced
// tree nodes is undefined unless specified otherwise as being end_safe.
//
// NOTE: Accessor objects are stable. They remain valid if the tree that the
// node they point to is modified in any way including detaching enveloping
// subtrees. So long as the node pointed to is not deleted (detach with dropped
// reference or replace) any accessors pointing to it are valid. Their
// behaviour may be seen analogous to that of std::list<..>::iterators which
// remain valid across inward and outward splice operations done on the lists.
//
// NOTE: Accessors would lose some semantic relevance across 'swap' operations.
// In swaps, the memory allocation of two nodes remains the same and the
// contents are switched. This would cause the first memory location to
// 'become' the tree rooted at the second location. Accessors are tied to
// memory locations and hence will not port across swaps.
template <class ContainerType, bool is_const=false>
class accessor {
  public:
    using node_type =
        typename std::conditional<is_const,
                                  const ContainerType,
                                  ContainerType>::type;
    using value_type_t =
        typename std::conditional<is_const,
                                  const typename node_type::value_type,
                                  typename node_type::value_type>::type;

    // An accessor which is specifically pointing to 'node'.
    //
    // 'node' must be part of a raw_tree<T> structure. A reference to 'node' is
    //     maintained inside this object and hence it must remain valid until
    //     at least the accessor is used to dereference it's value. Tree
    //     modifications don't affect the accessor. See note above.
    explicit accessor(node_type &node);
    accessor() = default;
    accessor(accessor &&) = default;
    accessor &operator=(const accessor &) = default;
    accessor &operator=(accessor &&) = default;

    // We specifically define the following constructor instead of using the
    // following signature:
    //
    // accessor(const accessor &) = default;
    // 
    // but the functionality is equivalent. When 'is_const' is false, this is a
    // copy constructor, when is_const is true this is a 'conversion'
    // constructor which converts from the non-const accessor to the const
    // accessor.
    //
	// Detailed notes at
	// http://www.drdobbs.com/the-standard-librarian-defining-iterato/184401331
    accessor(const accessor<ContainerType, false> &);

    // Dereferences.
    value_type_t &operator*() const;
    node_type &node() const;
    node_type *operator->() const;

    // Equality and position queries. These are end_safe operations.
    // Returns false iff iterator is in 'end' state.
    operator bool() const;
    bool operator==(const accessor &other) const;
    bool operator!=(const accessor &other) const;
    // Returns true if the node we point to is a 'root' node.
    bool is_root() const;
    // Returns the 'depth' of the node. Invalid result is returned when we are
    // at end.
    // Complexity: O(lg(n))
    uint32_t depth() const;

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
    //   'end' node is a no-op. This is end_safe.
    void up();
    // Sets accessor to 'root' node.
    // Complexity: Depends on the shape of the tree.
    //   O(log(N)) for perfectly balanced trees.
    //   O(N) for perfectly imbalanced trees.
    //   In between in other cases.
    // NOTE: This is end_safe. Calling on 'end' is no-op.
    void root();

    // Moves to the 'wing' child and returns true when possible.
    // Complexity: O(1).
    // NOTE: This is end_safe. Calling down<*>() on 'end' accessor is a no-op.
    template <side wing>
    bool down();
    bool down(side wing);

    // Returns an accessor pointing to the lowest common ancestor between this
    // and 'other'.
    // If either is 'end' accessor then 'end' is returned.
    // If the two accessors have different 'root's then the result is undefined.
    accessor common_ancestor(const accessor &other) const;

  private:
    void unsafe_up();

    // The node that we are pointing to currently in the tree or nullptr at
    // 'end'.
    node_type *node_ = nullptr;
};

#include "accessor.hh"

// raw_accessors are a family (T-parameterize) of accessors which work over
// 'raw_tree' container types.
template <class T>
using raw_accessor = accessor<raw_tree<T>, false>;

template <class T>
using const_raw_accessor = accessor<raw_tree<T>, true>;

#endif  // TREE_ACCESSOR_H
