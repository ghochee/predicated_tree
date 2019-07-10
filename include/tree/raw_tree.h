#include <array>
#include <memory>
#include <type_traits>

enum class traversal_order { pre, in, post };
// side is misleading because we have two children left and right, but
// if / when we have more this would be useful.
enum class side { left, right };

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

    template <traversal_order, side = side::left>
    class iterator;

    raw_tree() = delete;
    explicit raw_tree(T &&value) : value_(std::move(value)) {}
    explicit raw_tree(const T &value) : value_(value) {}
    explicit raw_tree(raw_tree &&);

    raw_tree &operator=(raw_tree &&);

    T &operator*();

    bool has_parent() const;
    raw_tree &parent();

    template <side wing = side::left>
    bool has_child() const;

    template <side wing = side::left>
    void replace(raw_tree<T> &&child);

    template <side wing = side::left>
    raw_tree detach();

    template <side wing = side::left>
    raw_tree &child();

    template <traversal_order order = traversal_order::in,
              side wing = side::left>
    iterator<order, wing> begin();
    template <traversal_order order = traversal_order::in,
              side wing = side::left>
    iterator<order, wing> end();

    // O(n) call as it actually iterates through the tree to recover the
    // counts.
    size_t size() const;

  private:
    template <side wing = side::left>
    const std::unique_ptr<raw_tree> &child_ref() const;
    template <side wing = side::left>
    std::unique_ptr<raw_tree> &child_ref();

    T value_;
    raw_tree *parent_ = nullptr;
    std::array<std::unique_ptr<raw_tree>, 2 /* num sides */> children_ = {
        {nullptr, nullptr}};
};

#include "tree/raw_tree.hh"

#include "tree/raw_tree_iterator.h"
