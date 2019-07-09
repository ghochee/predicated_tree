#include <memory>

enum class traversal_order { pre, in, post };

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

    template <traversal_order order>
    class iterator;

    raw_tree() = delete;
    explicit raw_tree(T &&value) : value_(std::move(value)) {}
    explicit raw_tree(const T &value) : value_(value) {}
    explicit raw_tree(raw_tree &&);

    raw_tree &operator=(raw_tree &&);

    T &operator*();

    bool has_parent() const;
    raw_tree &parent();

    bool has_left() const;
    raw_tree &left();
    void attach_left(raw_tree<T> &&left);
    raw_tree detach_left();

    bool has_right() const;
    raw_tree &right();
    void attach_right(raw_tree<T> &&right);
    raw_tree detach_right();

    template <traversal_order order=traversal_order::in>
    iterator<order> begin();
    template <traversal_order order=traversal_order::in>
    iterator<order> end();

    // O(n) call as it actually iterates through the tree to recover the
    // counts.
    size_t size() const;

  private:
    T value_;
    raw_tree *parent_ = nullptr;
    std::unique_ptr<raw_tree> left_ = nullptr, right_ = nullptr;
};

#include "tree/raw_tree.hh"

#include "tree/raw_tree_iterator.h"
