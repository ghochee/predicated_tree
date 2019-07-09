template <typename T>
class raw_tree<T>::iterator
    : public std::iterator<std::forward_iterator_tag, T> {
  public:
    iterator(const iterator &) = default;
    bool operator==(const iterator &other) const;
    bool operator!=(const iterator &other) const;
    reference operator*() const;
    iterator &operator++();
    iterator operator++(int);

    void preorder_increment();
    void inorder_increment();

  private:
    // Equivalent to ::end.
    iterator(const traversal_order &order = traversal_order::in)
        : order_(order) {}
    // Equivalent of ::begin on 'root'.
    explicit iterator(raw_tree &root,
                      const traversal_order &order = traversal_order::in);
    // Iterator pointing to 'node'.
    explicit iterator(raw_tree *node,
                      const traversal_order &order = traversal_order::in)
        : node_(node), order_(order) {}

    friend class raw_tree<T>;
    raw_tree *node_ = nullptr;

    // 'order_' is supposed to be const for the life of the object but the way
    // template code for iterators is written in most places, iterators are
    // copied around using assignment operator. This would specifically be a
    // problem for a const member variable as the operator= method cannot be
    // const.
    //
    // TODO(ghochee): Order should be in the type of the iterator. This is
    // because it is really a feature of the iterator type. The container
    // should default to providing in_order (for example) iterators on begin
    // and end. It cannot return arbitrarily 'ordered' iterators because the
    // method signature for begin would change. If we support automatic
    // conversion from one form to the other then the same ::end could be used
    // for all three types.
    traversal_order order_;
};

#include "raw_tree_iterator.hh"
