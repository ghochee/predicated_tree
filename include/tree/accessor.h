#ifndef TREE_ACCESSOR_H
#define TREE_ACCESSOR_H

#include <cstdint>
#include <type_traits>

namespace detangled {

/// accessors are used for creating *pointers* to nodes in a `raw_tree`.
///
/// # Overview
///
/// Accessor objects are used for referencing a node in a tree and for
/// navigating through the various nodes in the tree. It is a single object
/// that can be used to visit all the nodes in the tree.
///
/// Clients should see these classes as more fundamental than
/// `detangled::iterator`s are because objects of these classes don't know
/// *next* and can navigate freely through the container. Freely doesn't imply
/// cheaply; complexity costs are indicated on the various methods.
///
/// # End accessor
///
/// Other than nodes in a tree, accessor objects may be put in a state we call
/// *end* (because typically this state is useful when done as an iterator). In
/// this state the accessor refers to an invalid object. This accessor may be
/// seen like a virtual node which is the parent of the *root* node of this
/// tree with the difference that it is impossible to navigate down after
/// reaching there.
///
/// # Properties
///
/// ## Validity in the face of Mutating methods
///
/// Accessor objects are generally stable. They remain valid if the tree that
/// the node they point to is modified in any way including detaching
/// enveloping subtrees. So long as the node pointed to is not deleted (detach
/// with dropped reference or replace) any accessors pointing to it are valid.
///
/// Their behaviour may be seen analogous to that of `std::list::iterator`s
/// which remain valid across inward and outward splice operations done on the
/// lists.
///
/// ## Operations on end accessors
///
/// Once in *end* state doing any operations on the accessor or dereferenced
/// tree nodes is undefined unless specified otherwise as being end_safe.
///
/// ## Swap operations
///
/// Accessors would lose some semantic relevance across `swap` operations. In
/// swaps, the memory allocation of two nodes remains the same and the contents
/// are switched. This would cause the first memory location to *become* the
/// tree rooted at the second location. Accessors are tied to memory locations
/// and hence will not port across swaps.
///
/// @tparam Container is the tree container type that this accessor works over.
/// Methods like `parent()`, `child<side::left>()` etc. are expected to be
/// defined over this type.
template <class Container>
class accessor {
  public:
    static constexpr bool is_const = ::std::is_const<Container>::value;
    using container_type = typename ::std::remove_cv<Container>::type;
    using node_type = typename std::conditional<is_const, const container_type,
                                                container_type>::type;
    using value_type_t =
        typename std::conditional<is_const,
                                  const typename container_type::value_type,
                                  typename container_type::value_type>::type;

    // The following single argument constructor is not explicit so that
    // interfaces which accept `accessor`s can be called from client sites
    // using accessor objects or trees directly and an automatic construction
    // happens.
    accessor(node_type &node);
    accessor() = default;
    accessor(accessor &&) = default;
    accessor &operator=(const accessor &) = default;
    accessor &operator=(accessor &&) = default;

    accessor(const accessor<container_type> &pos);
    accessor(const accessor<const container_type> &post);

    // Dereferences.
    value_type_t &operator*() const;
    node_type &node() const;
    node_type *operator->() const;

    // @returns false iff iterator is in *end* state.
    operator bool() const;
    /// Equality and position queries. These are end_safe operations.
    bool operator==(const accessor &other) const;
    bool operator!=(const accessor &other) const;

    // @returns true iff the node we point to is a *root* node.
    bool is_root() const;

    /// **Complexity: O(lg(n))**
    /// @returns the 'depth' of the node. Invalid result is returned when we are
    /// at end.
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
    // NOTE: Calling up on 'root' is a no-op.
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

    // Returns the first ancestor which is on 'wing' side. For example if we
    // ask for 'left' ancestor then we keep going up until we reach an ancestor
    // who is a 'right' sided child. We then move to *it's parent* and return
    // true. If we never find such an ancestor, we move to root and return
    // false.
    template <side wing>
    bool ancestor();
    bool ancestor(side wing);

    // Returns an accessor pointing to the lowest common ancestor between this
    // and 'other'.
    // If either is 'end' accessor then 'end' is returned.
    // If the two accessors have different 'root's then the result is undefined.
    accessor common_ancestor(const accessor &other) const;

    // The following non-const constructor exists specifically to make life
    // easy for client code which has to write const and non-const versions of
    // functions. Calling this method to return a non-const accessor pointing
    // to the same element should be considered the semantic equivalent of
    // calling const_cast on an object inside a function which takes the
    // object's const reference.
    accessor<container_type> non_const() const;

  private:
    // The node that we are pointing to currently in the tree or nullptr at
    // 'end'.
    node_type *node_ = nullptr;
};

}  // namespace detangled

#include "impl/accessor.hh"

#endif  // TREE_ACCESSOR_H
