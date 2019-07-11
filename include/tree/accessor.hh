template <typename T>
bool accessor<T>::operator==(const accessor<T> &other) const {
    return depth_ == other.depth_ && node_ == other.node_;
}

template <typename T>
bool accessor<T>::operator!=(const accessor<T> &other) const {
    return !(other == *this);
}

template <typename T>
T &accessor<T>::operator*() const {
    return **node_;
}

template <typename T>
template <traversal_order order, side wing>
void accessor<T>::next() {
    if constexpr (order == traversal_order::pre) preorder_increment<wing>();
    if constexpr (order == traversal_order::in) inorder_increment<wing>();
    if constexpr (order == traversal_order::post) postorder_increment<wing>();
}

template <typename T>
void accessor<T>::up() {
    if (depth_ == -1) { return; }
    if (depth_ == 0) { --depth_; return; }
    unsafe_up();
}

template <typename T>
template <side wing>
bool accessor<T>::down() {
    if (depth_ == -1) {
        ++depth_;
        return true;
    }

    return node_->template has_child<wing>() &&
           (++depth_, node_ = &node_->template child<wing>(), true);
}

template <typename T>
void accessor<T>::unsafe_up() {
    node_ = &node_->parent();
    --depth_;
}

template <typename T>
template <side wing>
void accessor<T>::preorder_increment() {
    if (down<wing>() || down<!wing>()) { return; }

    for (; depth_ && (node_->template is_side<!wing>() ||
                      !node_->parent().template has_child<!wing>());
         unsafe_up()) {}

    if (depth_ == 0) {
        up();
        return;
    }

    node_ = &node_->parent().template child<!wing>();
}

template <typename T>
template <side wing>
void accessor<T>::inorder_increment() {
    if (depth_ == -1 || node_->template has_child<!wing>()) {
        for (down<!wing>(); down<wing>();) {}
        return;
    }

    for (; depth_ && node_->template is_side<!wing>(); unsafe_up()) {}
    up();
}

template <typename T>
template <side wing>
void accessor<T>::postorder_increment() {
    if (depth_ == 0) { return up(); }

    if (depth_ > 0 && (node_->template is_side<!wing>() ||
                       !(node_->parent().template has_child<!wing>()))) {
        return unsafe_up();
    }

    for (up(), down<!wing>(); down<wing>() || down<!wing>();) {}
}
