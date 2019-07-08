template <typename T>
class tree<T>::iterator : public std::iterator<std::input_iterator_tag, T> {
  public:
    bool operator==(const iterator &other) const;
    bool operator!=(const iterator &other) const;
    reference operator*() const;
    iterator &operator++();

    // Returns iterators to the three edges from this node. Any / all of them
    // may be ::end.
    iterator parent() const;
    iterator left() const;
    iterator right() const;

  private:
    iterator(const traversal_order &order = traversal_order::pre)
        : order_(order) {}
    explicit iterator(Node *node,
                      const traversal_order &order = traversal_order::pre)
        : node_(node), order_(order) {}

    friend class tree<T>;
    Node *node_ = nullptr;
    const traversal_order order_;
};

#include "iterator.hh"
