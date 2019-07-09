template <typename T>
template <traversal_order order>
raw_tree<T>::template iterator<order>::iterator(raw_tree<T> &root) {
    if constexpr (order == traversal_order::pre) {
        node_ = &root;
    }
    
    if constexpr (order == traversal_order::in) {
        for (node_ = &root; node_->left_.get(); node_ = node_->left_.get()) {}
    }
}

template <typename T>
template <traversal_order order>
template <traversal_order other_order>
raw_tree<T>::template iterator<order>::iterator(
    const iterator<other_order> &other)
    : node_(other.node_) {}

template <typename T>
template <traversal_order order>
bool raw_tree<T>::template iterator<order>::operator==(
    const raw_tree<T>::iterator<order> &other) const {
    return node_ == other.node_;
}

template <typename T>
template <traversal_order order>
bool raw_tree<T>::template iterator<order>::operator!=(
    const raw_tree<T>::iterator<order> &other) const {
    return node_ != other.node_;
}

template <typename T>
template <traversal_order order>
T &raw_tree<T>::template iterator<order>::operator*() const {
    return **node_;
}

template <typename T>
template <traversal_order order>
typename raw_tree<T>::template iterator<order>
    &raw_tree<T>::template iterator<order>::operator++() {
    if constexpr (order == traversal_order::pre) {
        preorder_increment();
    }
    
    if constexpr (order == traversal_order::in) {
        inorder_increment();
    }
    return *this;
}

template <typename T>
template <traversal_order order>
typename raw_tree<T>::template iterator<order>
    raw_tree<T>::template iterator<order>::operator++(int) {
    auto return_value = *this;
    ++(*this);
    return return_value;
}

template <typename T>
template <traversal_order order>
void raw_tree<T>::template iterator<order>::preorder_increment() {
    if (node_ == nullptr) {
        // TODO(ghochee): Set to root.
        return;
    }

    if (node_->left_) {
        node_ = node_->left_.get();
        return;
    }

    if (node_->right_) {
        node_ = node_->right_.get();
        return;
    }

    for (auto parent = node_->parent_;
         parent && (!parent->right_ || parent->right_.get() == node_);
         node_ = parent, parent = node_->parent_) {}
    node_ = node_->parent_;
    if (node_) { node_ = node_->right_.get(); }
}

template <typename T>
template <traversal_order order>
void raw_tree<T>::template iterator<order>::inorder_increment() {
    if (node_ == nullptr) {
        // TODO(ghochee): Set root and choose leftmost descendant.
        return;
    }

    if (node_ and !node_->right_) {
        for (auto parent = node_->parent_;
             parent && parent->right_ && parent->right_.get() == node_;
             node_ = parent, parent = node_->parent_) {}
        node_ = node_->parent_;
        return;
    }

    for (node_ = node_->right_.get(); node_->left_;
         node_ = node_->left_.get()) {}
}
