template <typename T>
bool tree<T>::iterator::operator==(
    const tree<T>::iterator &other) const {
    return node_ == other.node_;
}

template <typename T>
bool tree<T>::iterator::operator!=(
    const tree<T>::iterator &other) const {
    return node_ != other.node_;
}

template <typename T>
T &tree<T>::iterator::operator*() const {
    return **node_;
}

template <typename T>
typename tree<T>::iterator
    &tree<T>::iterator::operator++() {
    if (node_ == nullptr) { return *this; }

    if (order_ != traversal_order::pre) { return *this; }
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

template <typename T>
typename tree<T>::iterator
tree<T>::iterator::parent() const {
    if (!node_) { return iterator(nullptr); }
    return iterator(node_->parent_);
}

template <typename T>
typename tree<T>::iterator
tree<T>::iterator::left() const {
    if (!node_) { return iterator(nullptr); }
    return iterator(node_->left_.get());
}

template <typename T>
typename tree<T>::iterator
tree<T>::iterator::right() const {
    if (!node_) { return iterator(nullptr); }
    return iterator(node_->right_.get());
}
