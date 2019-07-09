template <typename T>
template <traversal_order order, side wing>
class raw_tree<T>::iterator
    : public std::iterator<std::forward_iterator_tag, T> {
  public:
    iterator(const iterator &) = default;

    // Automatic conversions between the various iterator types. This is a
    // transliteration (the position doesn't change only the direction in which
    // it will head will change). This behaviour is similar to forward and
    // reverse iterators.
    template <traversal_order other_order, side other_wing>
    iterator(const iterator<other_order, other_wing> &other);

    bool operator==(const iterator &other) const;
    bool operator!=(const iterator &other) const;
    reference operator*() const;
    iterator &operator++();
    iterator operator++(int);

    void preorder_increment();
    void inorder_increment();

  private:
    // Equivalent to ::end.
    iterator() = default;
    // Equivalent of ::begin on 'root'.
    explicit iterator(raw_tree &root);
    // Iterator pointing to 'node'.
    explicit iterator(raw_tree *node) : node_(node) {}

    friend class raw_tree<T>;
    raw_tree *node_ = nullptr;
};

#include "raw_tree_iterator.hh"
