template <typename T>
void tree<T>::insert(T &&value) {
    root_ = tree<T>::Node::merge(
        std::move(root_), std::make_unique<tree<T>::Node>(std::move(value)),
        isTall_, isLeft_);
}

template <typename T>
tree<T>::tree(std::function<bool(const T &, const T &)> isTall,
              std::function<bool(const T &, const T &)> isLeft)
    : isTall_(isTall), isLeft_(isLeft) {}

template <typename T>
template <traversal_order order>
typename tree<T>::template iterator<order> tree<T>::begin() const {
    return tree<T>::iterator<order>(root_.get());
}

template <typename T>
template <traversal_order order>
typename tree<T>::template iterator<order> tree<T>::end() const {
    return tree<T>::iterator<order>();
}
