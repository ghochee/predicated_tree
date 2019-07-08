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
    static bool indifferent(const T &, const T &) { return false; }

  public:
    // TODO(ghochee): This should be private? but testable?
    class Node;

    typedef T value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    template <traversal_order order>
    class iterator;

    tree(std::function<bool(const T &, const T &)> isTall = indifferent,
         std::function<bool(const T &, const T &)> isLeft = indifferent);

    void insert(T &&value);

    template <traversal_order order = traversal_order::pre>
    iterator<order> begin() const;
    template <traversal_order order = traversal_order::pre>
    iterator<order> end() const;

  private:
    const std::function<bool(const T &, const T &)> isTall_;
    const std::function<bool(const T &, const T &)> isLeft_;

    std::unique_ptr<Node> root_;
};

#include "iterator.h"
#include "node.h"

#include "tree.hh"
