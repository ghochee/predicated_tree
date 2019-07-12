#ifndef TREE_VIRTUAL_ACCESSOR_H
#define TREE_VIRTUAL_ACCESSOR_H

#include <cstdint>

// A virtual_accessor is an accessor. Major differences are detailed below.
//
// It's domain of operation may be limited to a logical subtree of a physical
// tree. All objects created from copying moving this object will retain this
// behaviour. All virtual_accessors created with the same logical subtree are
// semantically compatible. See constructor for notes.
//
// All operations are well-defined on 'end' accessor except the derefernce
// operations. Specifically movement operations on 'end' accessor are well
// defined. Even more specifically in 'accessor' an 'end'ed accessor can in
// no way be navigated back to the tree (except through reassignment which is
// really not navigating it). In the 'virtual_accessor' objects navigatiion
// using 'down' / 'root' etc. for navigating back to the tree is well
// defined.
//
// virtual_accessors are not 'stable' (see accessor NOTE:). Specifically
// modifying the underlying tree may make the iterator invalid. More
// specifically modifying the 'virtual tree' in such a way as to change the
// 'depth' of a node that any virtual_accessor is pointing to would invalidate
// this accessor. This also means that modifications *including* height changes
// done to any physical ancestor of the virtual root of the tree would *not*
// invalidate an accessor. A clean way to read it is 'any modification which
// changes the 'depth' of the pointed-to node *in* the virtual tree that an
// accessor is referring to would invalidate it.
template <class ContainerType>
class virtual_accessor : public accessor<ContainerType> {
  public:
    using node_type = ContainerType;
    using base_type = accessor<ContainerType>;

    // An accessor with 'node' set as the root of a virtual tree. All
    // operations on this object will restrict the accessor to this virtual
    // tree. See up(), root() for noticeable differences from how 'accessor'
    // objects behave.
    //
    // 'node' is used in 'accessor::accessor()' parent constructor call.
    // 'depth' of 0 indicates the 'root' node. It can also be given a special
    //     value of '-1' which indicates an accessor which is indicative of an
    //     'end' accessor. Any other value will cause a std::abort.
    virtual_accessor(base_type &pos);
    virtual_accessor(node_type &node, int16_t depth);
    virtual_accessor(const virtual_accessor &) = default;
    virtual_accessor(virtual_accessor &&) = default;
    virtual_accessor &operator=(const virtual_accessor &) = default;
    virtual_accessor &operator=(virtual_accessor &&) = default;

    operator bool() const;
    // Returns true iff node being pointed to is the same *and* the virtual
    // tree that we are part of is the same.
    bool operator==(const virtual_accessor &other) const;
    bool operator!=(const virtual_accessor &other) const;
    bool is_root() const;
    // Depth with respect to the 'virtual' root that we operating under. 0 at
    // virtual root. See class documentation for more information. Invalid at
    // 'end'.
    // Complexity: O(1);
    uint32_t depth() const;

    void up();
    // Same as parent with the difference that calling on 'end' objects will
    // move it to the virtual root of the subtree this is operating under. 
    void root();

    // Same as parent with the difference that calling down<*> on 'end' objects
    // is identical to calling 'root' and always returns true in that case.
    template <side wing>
    bool down();
    bool down(side wing);

  private:
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
    //   clients can set it as part of the contained value and the updates etc.
    //   are expected to update them it would be convoluted to bake it
    //   together.
    int16_t depth_ = -1;
};

#include "virtual_accessor.hh"

template <class T>
using raw_virtual_accessor = virtual_accessor<raw_tree<T>>;

#endif  // TREE_VIRTUAL_ACCESSOR_H
