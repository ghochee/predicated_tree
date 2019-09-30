#ifndef TREE_RAW_TREE_H
#define TREE_RAW_TREE_H

#include <array>
#include <memory>
#include <type_traits>
#include <utility>

namespace detangled {

/// traversal_order is an enum listing supported traversal orders.
///
/// For example we have `traversal_order::pre` (which is preorder traversal).
enum class traversal_order : uint8_t { pre, in, post };

/// Returns a `traversal_order` that complements `order`. Complementarity is
/// fully decided by `side` as well, but this function does the
/// `traversal_order` part of things.
///
/// `Complementary (order, wing) == (~order, !wing)`
///
/// - ~in == in
/// - ~pre == post
/// - ~post == pre
///
/// @see `operator!(const side wing)`
constexpr traversal_order operator~(const traversal_order order);

/// side enum lists the side (left or right) we refer to at code sites.
///
/// @note The values are specified here because they may be used in array
/// listing and indexing.
enum class side : uint8_t { left = 0, right = 1 };
constexpr side operator!(const side wing);

template <class ContainerType, traversal_order, side>
class iterator;

/// @addtogroup basic Basic Trees
///
/// A family of class templaes which can be used for making tree objects and
/// manipulating them. There are lot of related assets but the primary entities
/// in this group are as follows.
///
/// # Raw Tree
///
/// `detangled::raw_tree`s allow users to create and manipulate tree objects.
///
/// # Accessor
///
/// `detangled::accessor`s are *pointers* to nodes in a tree. These can be used
/// to navigate through tree elements. These should be thought of as equivalent
/// to `iterator`s from STL *especially* when used for specifying `pos` style
/// arguments. A related class family is `virtual_accessor`s which are pointers
/// to nodes limited to a subtree.
///
/// # Iterator
///
/// `detangled::iterator` classes are configured to perform *iteration*s on a
/// virtual tree of nodes in a specific order. This allows client code to
/// efficiently navigate through the container of nodes that they create.
/// Iterators are additionally configurable on the type of iterator (e.g.
/// `left` sided `preorder`) and also on the subtree on which to run the
/// iteration.
///
/// This group contains resources which can be used for creating and
/// manipulating basic tree objects.
/// @{

/// raw_tree is a *container* which behaves like a *tree*.
///
/// # Overview
///
/// ## Typical usage
///
/// ```cpp
///     raw_tree<std::string> messages("world ");
///     messages.replace<side::left>("Hello ");
///     messages.replace<side::right>("!");
/// ```
///
/// ## Recursive Container
///
/// It is a *container* because it allows storing elements of a fixed type and
/// allows client-code meaningful ways to access the elements. It is
/// foundationally unlike other containers in that it's internal storage is
/// recursive. This allows usage patterns which allow treating a *subtree* like
/// a tree.
///
/// ## Accessing values
///
/// Clients may create `detangled::iterator`s to *iterate* over the contained
/// values.  See `raw_tree::begin()` for more. Other than this
/// `raw_tree::parent()`, `raw_tree::child` etc. are methods which can be used
/// for accessing other nodes from a tree node.
///
/// ### Similarity with std containers
///
/// It is most closely related to `std::list` for the following reasons:
/// * *Nodes* are stored on the heap.
/// * The *head* / *root* node is stored on the stack like a standard container
///   head.
/// * It has *splice* like operations which allow stitching pieces from one
///   container object to another container object.
/// * `detangled::accessor`s pointing to nodes in a tree are valid across
///   `raw_tree::detach` and `raw_tree::replace` operations (but read
///   `detangled::accessor` documentation for exceptions).
///
/// # Relations
///
/// Other than the data stored at a *node* the other elements that a `raw_tree`
/// object contains are references to a *parent* and upto two children (*left*
/// and *right*).
///
/// ## Children
///
/// Each node has two children which it owns (a.k.a. deletes when going out of
/// scope). The first child is referred to as *left* and the second is *right*.
///
/// Methods which are used for performing operations on any one of the two
/// children are non-type-templated with the side enum
/// (`raw_tree::has_child<side::left>()` for example). Each child is
/// guaranteed to refer to `this`.
///
/// ## Parent
///
/// Parent relations are non-owning (i.e. we may die without affecting the
/// parent). We are guaranteed to be found at either the ``right`` or ``left``
/// child position of the parent node.
///
/// @note Root nodes are exceptions. See the `parent()` methods for details on
/// the result for these.
///
/// # Operations
///
/// Operations (methods) of the tree objects are categorizable and the
/// following notes help in remembering the classification and specific
/// gotchas.
///
/// ## Safety of methods
///
/// Almost all *referential* operations are unsafe and client code should be
/// satisfied before attempting to access or modify a relation. For example
/// `raw_tree::detach<side::right>()` when there no `side::right` child
/// present is undefined access.
///
/// The above is done for efficiency. Many algorithmic situations would result
/// in preconditions being true without any effective way to determine it
/// beforehand (meaning we wouldn't know them at compile time). Clients
/// can be efficient by bypassing safety checks in those situations.
///
/// ## Non-mutating and Mutating methods
///
/// *Non-mutating* operations allow client code to access data on the tree
/// including through a chain of relation nodes without modifying the tree in
/// any way. Values stored at nodes are also accessible and classified as
/// *non-mutating* operations.
///
/// Additionally non-mutating operations have two forms. non-const and ``...
/// const;`` ones. For example:
/// ```
///     const raw_tree &parent() const;
///     raw_tree &parent();
/// ```
/// They are functionally equivalent and present to enable client code to work
/// effectively with both `const` and non-const references.
///
/// *Mutating* operations allow client code to add, remove or reshape the tree.
/// `reshape`, `rotate`, `detach` are examples of these.
///
/// @verbatim
/// Generated documentation will list out a subset of the methods (one's for
/// which documentation is listed typically). The actual class has non-const
/// and non-templated versions of many of the methods.
/// @endverbatim
///
/// ## Side as template argument vs function argument
///
/// Methods which deal with child (and grandchild) nodes require specifying the
/// specific child. Two methods are provided for this category of methods, ones
/// in which this specification is given through:
///
/// - A *template* argument (`has_child<side::left>()`)
/// - A *function* argument (`has_child(side::left)`)
///
/// The template argument is specified when the wing being sought is available
/// at compile time. These can be inlined during compilation and optimizers can
/// optimize across these boundaries. The function-argument version uses the
/// template version.
///
/// @tparam T is the value type of the stored element.
template <typename T>
class raw_tree {
  public:
    typedef T value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    static_assert(::std::is_move_constructible<T>::value &&
                      ::std::is_move_assignable<T>::value,
                  "raw_tree must be templated with a MoveConstructible and "
                  "MoveAssignable type");

    // We cannot have a node without a value.
    // @note sphinx doesn't handle this correctly, so we don't allow it to
    // integrate into generated documentation.
    raw_tree() = delete;

    /// Construct a tree with a single value.
    /// TODO(ghochee): Class template for creating *empty* trees so that client
    /// side code like for loops can run more naturally.
    ///
    /// @param value is of a type and category which can be used for
    ///     constructing value_ correctly.
    template <typename U>
    explicit raw_tree(U &&value) : value_(std::forward<U>(value)) {}

    /// Move operations would *deep*-move a node. `other` may be a *root* node.
    ///
    /// **Complexity: O(1)**
    /// @param other refers to the node we are moving into this location.
    raw_tree(raw_tree &&other);
    raw_tree &operator=(raw_tree &&other);

    /// *Deep* swaps `this` with `other`. To do a shallow swap, call swap on
    /// the values (`swap(*first, *second)` instead of `swap(first, second)`).
    ///
    /// @verbatim
    /// Neither node can be an ancestor of the other. `other` need not be part
    /// of the same tree. Either may be a *root* node. Calling swap between
    /// ancestrally related nodes would result in undefined behaviour
    /// (eventually a segfault but could also result in a busy loop). This
    /// isn't just an implementation defect, *deep* swap between such nodes is
    /// nonsensical.
    /// @endverbatim
    ///
    /// **Complexity: O(1)**
    /// @param other refers to the other tree node we wish to swap with.
    void swap(raw_tree<T> &other);

    /// @returns the stored value.
    const T &operator*() const;
    T &operator*();

    /// @returns `true` iff we are not a root node.
    bool has_parent() const;

    /// @returns a reference to the parent node, undefined if we are *root*
    /// node.
    const raw_tree &parent() const;
    raw_tree &parent();

    /// @returns `true` iff we are *wing*-sided child of our parent. Always
    /// `false` for *root* node.
    template <side wing>
    bool is_side() const;

    /// @returns the side that this node is on, undefined if we are *root*.
    side get_side() const;

    /// @returns true iff *wing*-sided child is present.
    template <side wing>
    bool has_child() const;
    bool has_child(side wing) const;

    /// @tparam wing is the side that we want.
    /// @returns a reference to our *wing*-side child.
    template <side wing>
    const raw_tree &child() const;
    const raw_tree &child(side wing) const;
    template <side wing>
    raw_tree &child();
    raw_tree &child(side wing);

    /// **Complexity: O(N)** since we don't maintain counts so we have to
    /// iterate.
    /// @returns the size of the tree.
    size_t size() const;

    /// Reshapes `this` node and the relative positions of the children (and
    /// grand-children) while maintaining the `traversal_order::in` sequence.
    /// The specification of what reshaping is to be done is specified through
    /// the parameters `child` and `grand_child`. Specified with the
    /// parameters, the function call may be treated as a *command*.
    ///
    /// **Complexity: O(1)**
    ///
    /// @tparam child is the `side` that the child of post-operation `this`
    ///     should be.
    /// @tparam grand_child is the `side` that the *grand-child* of
    ///     post-operation `this` should be.
    ///
    /// After the reshaping command the value at `this` will change (because we
    /// reshape without moving `this` node). Any reshaping command which would
    /// make an absent node the root will cause undefined behaviour (likely
    /// segfault). For example R, R when the left child is absent is an error.
    ///
    /// Examples:
    ///
    /// @verbatim
    /// Before:
    ///                      1 === this node
    ///                     /             \
    ///                    /               \
    ///   child<side::left> === 0         child<side::right> === 2
    ///           /          \               /           \
    ///          /            \             /             \
    ///      left-left     left-right   right-left    right-right
    ///
    /// command: L, R
    ///
    ///                     right(2)
    ///                    /        \
    ///                   /          \
    ///                left(0)     right-right
    ///               /       \
    ///              /         \
    ///          left-left    this(1)
    ///                              \
    ///                               \
    ///                            right-left
    ///
    /// command: R, R
    ///
    ///                      left(0)
    ///                     /       \
    ///                    /         \
    ///                left-left   this(1)
    ///                           /       \
    ///                          /         \
    ///                    left-right    right(2)
    ///                                 /        \
    ///                                /          \
    ///                          right-left    right-right
    ///
    /// @endverbatim
    template <side child, side grand_child>
    void reshape();

    /// Performs *wing*-side rotation as described in common literature (e.g.
    /// left-rotation).
    /// @note `rotate<wing>()` is identical to `reshape<wing, wing>()`.
    /// @tparam wing is the side to which we want rotation.
    template <side wing>
    void rotate();
    void rotate(side wing);

    /// Switch the left and right children.
    void flip();

    /// Replace *wing*-sided child of `this`.
    ///
    /// @param child is the node we wish to bring in.
    template <side wing>
    void replace(raw_tree<T> &&child);
    void replace(side wing, raw_tree<T> &&child);

    /// Create *wing*-sided child of `this`.
    ///
    /// @tparam Args are the types of the arguments for emplacement
    ///     construction.
    /// @param args are the constructor args for construction of
    ///     `raw_tree::value_type`, discarding any existing values.
    template <side wing, typename... Args>
    void emplace(Args &&... args);
    template <typename... Args>
    void emplace(side wing, Args &&... args);

    /// Equivalent emplacing a node and moving existing node to a *grand_child*
    /// location.
    ///
    /// @tparam wing is the side where we wish to emplace.
    /// @tparam gc_wing is the side where we wish to move the existing node.
    /// @param args are the constructor arguments for construction of
    ///     `raw_tree::value_type`.
    template <side wing, side gc_wing, typename... Args>
    void splice(Args &&... args);

    /// Detaches a child node making it a root node. Undefined is not present.
    ///
    /// @tparam wing is the child `side` we wish to detach.
    template <side wing>
    raw_tree detach();
    raw_tree detach(side wing);

    /// Returns an iterator for the tree rooted at `this`.
    ///
    /// For example `begin<traversal_order::pre, side::left>()` would return an
    /// `iterator` which would follow *left*-sided *inorder* traversal over the
    /// *subtree* rooted at `this`. Due to the nature of this container it is
    /// possible to have the following construct:
    /// ```cpp
    ///     for (auto it = root_node.begin<traversal_order::post,
    ///            side::left>(); it != root_node.end<...>; ++it) {
    ///         if (meets_some_condition(*it)) {
    ///             for (auto jt = it->begin<traversal_order::in, side::left>();
    ///                  jt != it->end<...>(); ++jt) {
    ///                 ...
    ///             }
    ///         }
    ///     }
    /// ```
    /// which effectively iterates through a subtree at `*it` inorder if
    /// `meets_some_condition` is satisfied. This kind of construction allows
    /// for complex algorithmic expressiveness cleanly.
    ///
    /// @tparam order is the `traversal_order` that the returned `iterator`
    ///     should follow.
    /// @tparam wing is the `side` from which the `order` must be followed.
    /// @returns an `iterator` for the tree rooted at `this`.
    template <traversal_order order, side wing>
    iterator<raw_tree<T>, order, wing> begin();
    template <traversal_order order, side wing>
    iterator<const raw_tree<T>, order, wing> begin() const;

    /// Returns an iterator which compares equal with an iterator which starts
    /// from begin and has navigated through all the nodes (depending on the
    /// specific order and wing).
    ///
    /// Specifically
    /// ```cpp
    ///    std::advance(tree.begin<traversal_order::post, side::right>(),
    ///                 tree.size()) == \
    ///        tree.end<traversal_order::post, side::right>()
    /// ```
    /// is always true. Note that the traversal_order, wing pair has to be the
    /// same for the begin and end iterators (even though they are convertible
    /// from one to another). Comparing iterators with different orders / wings
    /// is not guaranteed to be correct.
    ///
    /// See `detangled::accessor::accessor()` for information on *end* for
    /// iterators.
    template <traversal_order order, side wing>
    iterator<raw_tree<T>, order, wing> end();
    template <traversal_order order, side wing>
    iterator<const raw_tree<T>, order, wing> end() const;

    /// Compact method names for various iterator types. The generic
    /// construction of the name is in the form of
    /// `order + wing_short + ("begin" | "end")`.
    ///
    /// @param prefix is the prefix we are making an alias with e.g. *postl*
    /// @param order is the `traversal_order` to map with e.g.
    ///     `traversal_order::post`
    /// @param wing is the `side` we are using in the alias e.g. `side::left`
    ///
    /// e.g.
    /// @verbatim
    /// prelbegin (order = pre, wing = left  (l), begin)
    /// inrend    (order = in,  wing = right (r), end)
    /// @endverbatim
#define RAW_TREE_MAKE_ALIAS(prefix, order, wing)                  \
    inline auto prefix##begin() {                                 \
        return this->begin<traversal_order::order, side::wing>(); \
    }                                                             \
    inline auto prefix##begin() const {                           \
        return this->begin<traversal_order::order, side::wing>(); \
    }                                                             \
    inline auto prefix##end() {                                   \
        return this->end<traversal_order::order, side::wing>();   \
    }                                                             \
    inline auto prefix##end() const {                             \
        return this->end<traversal_order::order, side::wing>();   \
    }

    // clang-format off
    RAW_TREE_MAKE_ALIAS(inl,   in,   left);
    RAW_TREE_MAKE_ALIAS(inr,   in,   right);
    RAW_TREE_MAKE_ALIAS(prel,  pre,  left);
    RAW_TREE_MAKE_ALIAS(prer,  pre,  right);
    RAW_TREE_MAKE_ALIAS(postl, post, left);
    RAW_TREE_MAKE_ALIAS(postr, post, right);
    // clang-format on

  private:
    T value_;
    raw_tree *parent_ = this;
    std::array<std::unique_ptr<raw_tree>, 2 /* num sides */> children_ = {
        {nullptr, nullptr}};
};

/// @}

template <typename T>
void swap(raw_tree<T> &left, raw_tree<T> &right);

}  // namespace detangled

#include "impl/raw_tree.hh"

#endif  // TREE_RAW_TREE_H
