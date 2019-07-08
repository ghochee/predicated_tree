#include <cstddef>
#include <functional>
#include <memory>

enum class traversal_order { pre, in, post };

// TODO(ghochee): This can be a partial specialization of
// tree<T, TallComparator, LeftComparator> where
// TallComparator = LeftComparator = std::function<bool(const T &, const T &)>
template <typename T>
class tree {
  private:
    // Strict weak ordering is needed for the predicates. This is a totally
    // strictly weak ordering :).
    static bool indifferent(const T &, const T &) { return false; }

  public:
    // TODO(ghochee): This should be private? but testable?
    class Node;

    typedef T value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    class iterator;

    tree(std::function<bool(const T &, const T &)> isTall = indifferent,
         std::function<bool(const T &, const T &)> isLeft = indifferent);

    // Insert 'value' into the tree. 'isTall' and 'isLeft' predicates are used
    // to determine correct position to insert at.
    //
    // 'pos' is a hint for insertion location. 'isTall' and 'isLeft' is used to
    //     determine if incorrect in which it is ignored.
    void insert(T &&value, iterator pos);

    iterator begin() const;
    iterator end() const;

  private:
    static iterator end_();

    // Insert helper which inserts a new node containing 'value' under 'pos'
    // **and** updates 'pos' to the result of the Node::merge call.
    void insert(T &&value, std::unique_ptr<Node> &pos);

    const std::function<bool(const T &, const T &)> isTall_;
    const std::function<bool(const T &, const T &)> isLeft_;

    std::unique_ptr<Node> root_;
};

#include "iterator.h"
#include "node.h"

#include "tree.hh"
