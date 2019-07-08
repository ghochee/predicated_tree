template <typename T>
template <traversal_order order>
class tree<T>::iterator : public std::iterator<std::input_iterator_tag, T> {
  public:
    iterator() {}
    explicit iterator(Node *node) : node_(node) {}

    bool operator==(const iterator &other) const;
    bool operator!=(const iterator &other) const;
    reference operator*() const;
    iterator &operator++();

  private:
    Node *node_ = nullptr;
};

#include "iterator.hh"
