#ifndef TREE_VIRTUAL_ACCESSOR_H
#define TREE_VIRTUAL_ACCESSOR_H

#include <cstdint>

#include "enums.h"

namespace detangled {

/// @addtogroup basic
/// @{

/// A virtual_accessor is an accessor on a *virtual* tree.
///
/// # Overview
///
/// It's domain of operation may be limited to a subtree of a *physical* tree.
/// All objects created from copying moving this object will retain this
/// behaviour. All virtual_accessors created with the same logical subtree are
/// semantically compatible. See constructor for notes.
///
/// Minor modifications are done on methods defined on the parent
/// `detangled::accessor` class. These are not done through virtual methods for
/// efficiency but also because clients are reasonably expected to not need to
/// erase type information when dealing with `detangled::accessor` and
/// `virtual_accessor` class family objects.
///
/// # Properties
///
/// ## Operations on end accessors
///
/// All operations are well-defined on *end* accessor except the derefernce
/// operations. Specifically movement operations on *end* accessor are well
/// defined. Even more specifically in `detangled::accessor` an *end*ed
/// accessor can in no way be navigated back to the tree (except through
/// reassignment which is really not navigating it). In the `virtual_accessor`
/// objects navigatiion using `down` / `root` etc. for navigating back to the
/// tree is well defined.
///
/// ## Validity in the face of Mutating methods
///
/// virtual_accessors are not *stable* (see accessor *Validity in the face of
/// ...*). Specifically modifying the underlying tree may make the iterator
/// invalid. More specifically modifying the 'virtual tree' in such a way as to
/// change the 'depth' of a node that any virtual_accessor is pointing to would
/// invalidate this accessor. This also means that modifications *including*
/// height changes done to any physical ancestor of the virtual root of the
/// tree would *not* invalidate an accessor. A clean way to read it is 'any
/// modification which changes the 'depth' of the pointed-to node *in* the
/// virtual tree that an accessor is referring to would invalidate it.
///
/// @tparam ContainerType is the same as that for `detangled::accessor`.
template <class ContainerType>
class virtual_accessor : public accessor<ContainerType> {
  public:
    using base_type = accessor<ContainerType>;
    using container_type = typename base_type::container_type;
    using node_type = typename base_type::node_type;
    using value_type_t = typename base_type::value_type_t;

    /// @param pos is the position of the accessor type which is treated as
    ///     *root*.
    virtual_accessor(base_type &pos);

    /// An accessor with `node` set as the root of a virtual tree. All
    /// operations on this object will restrict the accessor to this virtual
    /// tree. See up(), root() for noticeable differences from how
    /// `detangled::accessor` objects behave.
    ///
    /// @param node is used in `detangled::accessor::accessor()` parent
    ///     constructor call.
    /// @param depth of 0 indicates the *root* node. It can also be given a
    ///     special value of -1 which indicates an accessor which is indicative
    ///     of an *end* accessor. Any other value will cause a std::abort.
    virtual_accessor(node_type &node, int16_t depth);

    virtual_accessor(const virtual_accessor &) = default;
    virtual_accessor(virtual_accessor &&) = default;
    virtual_accessor &operator=(const virtual_accessor &) = default;
    virtual_accessor &operator=(virtual_accessor &&) = default;

    /// @returns true iff we are not at *end*.
    operator bool() const;

    /// @returns true iff node being pointed to is the same *and* the virtual
    /// tree that we are part of is the same.
    bool operator==(const virtual_accessor &other) const;
    bool operator!=(const virtual_accessor &other) const;
    bool is_root() const;

    /// Depth with respect to the *virtual* root that we operating under. 0 at
    /// virtual root. See class documentation for more information. Invalid at
    /// *end*.
    ///
    /// **Complexity: O(1)**
    uint32_t depth() const;

    void up();

    /// Same as parent with the difference that calling on *end* objects will
    /// move it to the virtual root of the subtree this is operating under.
    void root();

    /// Same as parent with the difference that calling down<*> on *end* objects
    /// is identical to calling *root* and always returns true in that case.
    template <side wing>
    bool down();
    bool down(side wing);

  private:
    /// Depth indicates the depth of the node in our tree. Root node has a depth
    /// of 0 and all subsequent values are 1 more. The only invalid value is
    /// ::std::numeric_limits<uint64_t>::max(). This value is set for `end`
    /// iterators. When a `virtual_accessor` is set to `end` the `node_` is set
    /// to the *virtual root*. This is different behaviour from raw `accessor`
    /// where `node_` would be set to nullptr.
    ///
    /// Having this has a few benefits:
    /// - Without a second field we will have to set node_ to a tree-neutral
    ///   value to indicate end of iterator. This means we would store a
    ///   pointer to a value which is completely disconnected from the tree and
    ///   hence can never lead us back to the tree. This is normally fine but
    ///   we would do well with this because we can reverse iterate if we have
    ///   access to the original tree element.
    /// - This allows us to have iterators which range only over a subtree.
    ///   This seems like a pretty useful feature for writing more complex
    ///   algorithms.
    /// - Depth is a useful feature when iterating through a tree. Though
    ///   clients can set it as part of the contained value and the updates
    ///   etc. are expected to update them it would be convoluted to bake it
    ///   together.
    ///
    /// NOTE: -1 is technically out-of-range but is well defined in the C++
    /// standard for unsigned integer operations for equality, incrementing and
    /// decrementing.
    uint64_t depth_ = -1;
};

/// @}

}  // namespace detangled

#include "impl/virtual_accessor.hh"

#endif  // TREE_VIRTUAL_ACCESSOR_H
