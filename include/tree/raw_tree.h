#ifndef TREE_RAW_TREE_H
#define TREE_RAW_TREE_H

#include <array>
#include <memory>
#include <type_traits>

enum class traversal_order : uint8_t { pre, in, post };
constexpr traversal_order operator~(const traversal_order order);

enum class side : uint8_t { left, right };
constexpr side operator!(const side wing);

// raw_tree is a class which is used for creating a 'raw' tree. It allows
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
    explicit raw_tree(raw_tree &&);

    raw_tree &operator=(raw_tree &&);

    T &operator*();

    bool has_parent() const;
    raw_tree &parent();

    template <side wing>
    bool is_side() const;

    template <side wing>
    bool has_child() const;

    template <side wing>
    void replace(raw_tree<T> &&child);

    template <side wing>
    raw_tree detach();

    template <side wing>
    raw_tree &child();

    template <traversal_order order, side wing>
    iterator<order, wing> begin();
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

#include "tree/raw_tree_iterator.h"

#endif  // TREE_RAW_TREE_H
