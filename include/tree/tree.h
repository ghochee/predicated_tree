#include <cstddef>
#include <functional>

// TODO(ghochee): This can be a partial specialization of
// tree<T, TallComparator, LeftComparator> where
// TallComparator = LeftComparator = std::function<bool(const T &, const T &)>
template <typename T>
class tree {
  private:
    static bool indifferent(const T &, const T &) { return false; }
    enum class IteratorOrder { pre, in, post };

  public:
    class Node;
    typedef T value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    template <IteratorOrder order>
    class iterator;

    tree(std::function<bool(const T &, const T &)> isTall = indifferent,
         std::function<bool(const T &, const T &)> isLeft = indifferent);

    template <IteratorOrder order = IteratorOrder::pre>
    iterator<order> begin() const;
    template <IteratorOrder order = IteratorOrder::pre>
    iterator<order> end() const;

  private:
    const std::function<bool(const T &, const T &)> isTall_;
    const std::function<bool(const T &, const T &)> isLeft_;
};

#include "node.h"
#include "tree.hh"
