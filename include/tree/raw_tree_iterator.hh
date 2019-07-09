template <typename T>
raw_tree<T>::iterator::iterator(
        raw_tree<T> &root,
        const traversal_order &order)
    : node_(nullptr),
      order_(order) {
    if (order == traversal_order::pre) {
        node_ = &root;
    } else if (order == traversal_order::in) {
        for (node_ = &root; node_->left_.get(); node_ = node_->left_.get()) {}
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
    if (order_ == traversal_order::pre) {
        preorder_increment();
    } else if (order_ == traversal_order::in) {
        inorder_increment();
    }
    return *this;
}

template <typename T>
typename raw_tree<T>::iterator raw_tree<T>::iterator::operator++(int) {
    auto return_value = *this;
    ++(*this);
    return return_value;
}

template <typename T>
void raw_tree<T>::iterator::preorder_increment() {
    if (node_ == nullptr) {
        // FIXME(ghochee): Set to root.
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
    if (node_) {
        node_ = node_->right_.get();
    }
}

template <typename T>
void raw_tree<T>::iterator::inorder_increment() {
    if (node_ == nullptr) {
        // FIXME(ghochee): Set root and choose leftmost descendant.
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
