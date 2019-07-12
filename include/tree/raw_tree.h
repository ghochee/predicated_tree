#ifndef TREE_RAW_TREE_H
#define TREE_RAW_TREE_H

#include <array>
#include <memory>

enum class traversal_order : uint8_t { pre, in, post };
constexpr traversal_order operator~(const traversal_order order);

enum class side : uint8_t { left, right };
constexpr side operator!(const side wing);

// There are a large number of wrapper functions of the following type
//     returnType functionName(side wing, args...);
//
// which have analogous
//     template <side wing>
//     returnType function(args...);
//
// templated functions. The definition of such wrapper functions is as seen for
// the SWITCH_ON_SIDE macro. Basically it involves looking at the wing argument
// and choosing to call a specific template version (side == left or right).
// This macro does this and makes is more reliable to define the wrapper
// functions with much lower probability of errors creeping in.
//
// TODO(ghochee): Try to template instantiate the entire definition or MACRO
// define the whole function instead of just the body.
#define SWITCH_ON_SIDE(function_name, ...)              \
    if (wing == side::left) {                           \
        return function_name<side::left>(__VA_ARGS__);  \
    } else {                                            \
        return function_name<side::right>(__VA_ARGS__); \
    }

// raw_tree is a container which is used for creating a 'raw' tree. It allows
// client users to manipulate the tree in tree like operations.
//
// T is the value type of the element stored in the tree. This is expected to
// be MoveConstructible and MoveAssignable. raw_tree<T> itself is
// MoveConstructible and MoveAssignable.
template <typename T>
class raw_tree {
  public:
    typedef T value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    template <traversal_order, side>
    class iterator;

    raw_tree() = delete;
    explicit raw_tree(T &&value) : value_(std::move(value)) {}
    explicit raw_tree(const T &value) : value_(value) {}
    raw_tree(raw_tree &&);

    raw_tree &operator=(raw_tree &&);

    T &operator*();

    bool has_parent() const;
    raw_tree &parent();

    template <side wing>
    bool is_side() const;
    bool is_side(side wing) const;

    template <side wing>
    bool has_child() const;
    bool has_child(side wing) const;

    template <side wing>
    raw_tree &child();
    raw_tree &child(side wing);

    // Reshape.

    // Reshape this node and the relative positions of the children while
    // maintaining the in-order traversal sequence. The specification of what
    // reshaping is to be done is specified through the template parameters
    // child and grandchild. Specified with the parameters, the function call
    // may be treated as a "command".
    //
    // Examples:
    //
    // Before:
    //                      1 === this node
    //                     /             \
    //                    /               \
    //   child<side::left> === 0         child<side::right> === 2
    //           /          \               /           \
    //          /            \             /             \
    //      left-left     left-right   right-left    right-right
    //
    // command: L, R
    //
    //                     right(2)
    //                    /        \
    //                   /          \
    //                left(0)     right-right
    //               /       \
    //              /         \
    //          left-left    this(1)
    //                              \
    //                               \
    //                            right-left
    //
    // command: R, R
    //
    //                      left(0)
    //                     /       \
    //                    /         \
    //                left-left   this(1)
    //                           /       \
    //                          /         \
    //                    left-right    right(2)
    //                                 /        \
    //                                /          \
    //                          right-left    right-right
    //
    // After the reshaping command the value at this will change (because we
    // reshape without moving this node but move around it's values). Any
    // reshaping command which would make an absent node the root will cause
    // undefined behaviour (likely segfault). For example R, R when the left
    // child is absent is an error.
    template <side child, side grand_child>
    void reshape();

    // NOTE: right == clockwise      == reshape<R, R>
    //       left  == anti-clockwise == reshape<L, L>
    template <side wing>
    void rotate();
    void rotate(side wing);

    // Add / remove subtrees.
    template <side wing>
    void replace(raw_tree<T> &&child);
    void replace(side wing, raw_tree<T> &&child);

    template <side wing>
    raw_tree detach();
    raw_tree detach(side wing);

    // Construct iterators for the tree.
    template <traversal_order order, side wing>
    iterator<order, wing> begin();

    // Returns an iterator which compares equal with an iterator which starts
    // from begin and has navigated through all the nodes (depending on the
    // specific order and wing).
    //
    // Specifically
    //    std::advance(tree.begin<traversal_order::post, side::right>(),
    //                 tree.size()) == \
    //    tree.end<traversal_order::post, side::right>()
    // is always true. Note that the traversal_order, wing pair has to be the
    // same for the begin and end iterators (even though they are convertible
    // from one to another). Comparing iterators with different orders / wings
    // is not guaranteed to be correct.
    //
    // See accessor<T>::accessor(...) for information on 'end' for iterators.
    template <traversal_order order, side wing>
    iterator<order, wing> end();

#define RAW_TREE_MAKE_ALIAS(prefix, order, wing)                  \
    inline auto prefix##begin()->decltype(                        \
        begin<traversal_order::order, side::wing>()) {            \
        return this->begin<traversal_order::order, side::wing>(); \
    }                                                             \
    inline auto prefix##end()->decltype(                          \
        end<traversal_order::order, side::wing>()) {              \
        return this->end<traversal_order::order, side::wing>();   \
    }

    // Compact method names for various iterator types. The generic
    // construction of the name is in the form of
    //
    // <order><wing_short>{begin|end}
    // e.g.
    // prelbegin (order = pre, wing = left  (l), begin)
    // inrend    (order = in,  wing = right (r), end)
    RAW_TREE_MAKE_ALIAS(inl,   in,   left);
    RAW_TREE_MAKE_ALIAS(inr,   in,   right);
    RAW_TREE_MAKE_ALIAS(prel,  pre,  left);
    RAW_TREE_MAKE_ALIAS(prer,  pre,  right);
    RAW_TREE_MAKE_ALIAS(postl, post, left);
    RAW_TREE_MAKE_ALIAS(postr, post, right);

    // O(n) call as it actually iterates through the tree to recover the
    // counts.
    size_t size() const;

  private:
    T value_;
    raw_tree *parent_ = nullptr;
    std::array<std::unique_ptr<raw_tree>, 2 /* num sides */> children_ = {
        {nullptr, nullptr}};
};

#include "tree/raw_tree.hh"

#endif  // TREE_RAW_TREE_H
