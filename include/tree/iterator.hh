template <typename T>
template <traversal_order order>
bool tree<T>::iterator<order>::operator==(
    const tree<T>::iterator<order> &other) const {
    return node_ == other.node_;
}

template <typename T>
template <traversal_order order>
bool tree<T>::iterator<order>::operator!=(
    const tree<T>::iterator<order> &other) const {
    return node_ != other.node_;
}

template <typename T>
template <traversal_order order>
T &tree<T>::iterator<order>::operator*() const {
    return **node_;
}

template <typename T>
template <traversal_order order>
typename tree<T>::template iterator<order>
    &tree<T>::iterator<order>::operator++() {
    if (node_ == nullptr) { return *this; }

    if (order != traversal_order::pre) { return *this; }
    if (node_->left_) {
        node_ = node_->left_.get();
        return *this;
    }

    while (node_ != nullptr) {
        if (node_->parent_ && node_->parent_->right_ &&
            node_->parent_->right_.get() != node_) {
            node_ = node_->parent_->right_.get();
        } else {
            node_ = node_->parent_;
        }
    }

    return *this;
}
