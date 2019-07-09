template <typename T>
raw_tree<T>::iterator::iterator(
        raw_tree<T> &root,
        const traversal_order &order)
    : node_(nullptr),
      order_(order) {
    if (order != traversal_order::in) {
        return;
    }
    for (auto first = &root; true; first = first->left_.get()) {
        if (!first->left_) {
            node_ = first;
            break;
        }
    }
}

template <typename T>
bool raw_tree<T>::iterator::operator==(
    const raw_tree<T>::iterator &other) const {
    return node_ == other.node_;
}

template <typename T>
bool raw_tree<T>::iterator::operator!=(
    const raw_tree<T>::iterator &other) const {
    return node_ != other.node_;
}

template <typename T>
T &raw_tree<T>::iterator::operator*() const {
    return **node_;
}

template <typename T>
typename raw_tree<T>::iterator &raw_tree<T>::iterator::operator++() {
    if (node_ == nullptr) { return *this; }

    if (order_ == traversal_order::pre) {
        return preorder_increment();
    } else if (order_ == traversal_order::in) {
        return inorder_increment();
    } else {
        return *this;
    }
    return *this;
}

template <typename T>
typename raw_tree<T>::iterator &raw_tree<T>::iterator::preorder_increment() {
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
typename raw_tree<T>::iterator &raw_tree<T>::iterator::inorder_increment() {
    if (node_->right_) {
        for (node_ = node_->right_.get(); node_->left_;
             node_ = node_->left_.get()) {}
        return *this;
    }

    for (auto parent = node_->parent_; parent && parent->right_.get() == node_;
         node_ = parent, parent = node_->parent_) {}
    node_ = node_->parent_;
    return *this;
}
