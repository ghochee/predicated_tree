template <typename T>
template <traversal_order order, side wing>
raw_tree<T>::template iterator<order, wing>::iterator(raw_tree<T> &root)
    : node_(&root) {
    ++(*this);
}

template <typename T>
template <traversal_order order, side wing>
template <traversal_order other_order, side other_wing>
raw_tree<T>::template iterator<order, wing>::iterator(
    const iterator<other_order, other_wing> &other)
    : node_(other.node_), depth_(other.depth_) {}

template <typename T>
template <traversal_order order, side wing>
bool raw_tree<T>::template iterator<order, wing>::operator==(
    const raw_tree<T>::iterator<order, wing> &other) const {
    return depth_ == other.depth_ && node_ == other.node_;
}

template <typename T>
template <traversal_order order, side wing>
bool raw_tree<T>::template iterator<order, wing>::operator!=(
    const raw_tree<T>::iterator<order, wing> &other) const {
    return !(other == *this);
}

template <typename T>
template <traversal_order order, side wing>
T &raw_tree<T>::template iterator<order, wing>::operator*() const {
    return **node_;
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing>
    &raw_tree<T>::template iterator<order, wing>::operator++() {
    if constexpr (order == traversal_order::pre) { preorder_increment(); }
    if constexpr (order == traversal_order::in) { inorder_increment(); }
    if constexpr (order == traversal_order::post) { postorder_increment(); }
    return *this;
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing>
    raw_tree<T>::template iterator<order, wing>::operator++(int) {
    auto return_value = *this;
    ++(*this);
    return return_value;
}

template <typename T>
template <traversal_order order, side wing>
void raw_tree<T>::template iterator<order, wing>::preorder_increment() {
    if (depth_ == -1) {
        ++depth_;
        return;
    }

    if (node_->child_ref<wing>()) {
        ++depth_;
        node_ = node_->child_ref<wing>().get();
        return;
    }

    if (node_->child_ref<!wing>()) {
        ++depth_;
        node_ = node_->child_ref<!wing>().get();
        return;
    }

    raw_tree<T> *parent = node_->parent_;
    for (; parent && (!parent->child_ref<!wing>() ||
                      parent->child_ref<!wing>().get() == node_);
         --depth_, node_ = std::exchange(parent, parent->parent_)) {}

    if (parent == nullptr) {
        --depth_;
    } else {
        node_ = parent->child_ref<!wing>().get();
    }
}

template <typename T>
template <traversal_order order, side wing>
void raw_tree<T>::template iterator<order, wing>::inorder_increment() {
    if (depth_ == -1) {
        for (++depth_; node_->child_ref<wing>();
             ++depth_, node_ = node_->child_ref<wing>().get()) {}
        return;
    }

    if (node_->child_ref<!wing>()) {
        for (raw_tree<T> *child = node_->child_ref<!wing>().get(); child;
             ++depth_, node_ = std::exchange(child,
                                             child->child_ref<wing>().get())) {}
        return;
    }

    raw_tree<T> *parent = node_->parent_;
    for (; parent && parent->child_ref<!wing>().get() == node_;
         --depth_, node_ = std::exchange(parent, parent->parent_)) {}
    --depth_;
    if (parent != nullptr) { node_ = parent; }
}

template <typename T>
template <traversal_order order, side wing>
void raw_tree<T>::template iterator<order, wing>::postorder_increment() {
    if (depth_ == -1) {
        for (raw_tree<T> *child = std::exchange(node_, nullptr); child;
             ++depth_, node_ = std::exchange(
                           child, child->child_ref<wing>().get()
                                      ? child->child_ref<wing>().get()
                                      : child->child_ref<!wing>().get())) {}
        return;
    }

    raw_tree<T> *parent = node_->parent_;
    if (parent == nullptr) {
        --depth_;
        return;
    }

    if (parent->child_ref<!wing>().get() == node_) {
        --depth_;
        node_ = parent;
        return;
    }

    --depth_;
    node_ = node_->parent_;
    for (raw_tree<T> *child = node_->child_ref<!wing>().get(); child;
         ++depth_, node_ = std::exchange(
                       child, child->child_ref<wing>().get()
                                  ? child->child_ref<wing>().get()
                                  : child->child_ref<!wing>().get())) {}
}
