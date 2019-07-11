#include <algorithm>
#include <cstdlib>

template <typename T>
accessor<T>::accessor(raw_tree<T> &node, int16_t depth)
    : node_(&node), depth_(depth) {
    if (!(depth_ >= -1)) { ::std::abort(); }

    for (raw_tree<T> *node_ptr = &node; depth > 0;
         --depth, node_ptr = std::exchange(node_ptr, &node_ptr->parent())) {
        if (!node_ptr->has_parent()) { ::std::abort(); }
    }
}

template <typename T>
accessor<T>::operator bool() const {
    return depth_ != -1;
}

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
raw_tree<T> &accessor<T>::node() const {
    return *node_;
}

template <typename T>
raw_tree<T> *accessor<T>::operator->() const {
    return node_;
}

template <typename T>
template <traversal_order order, side wing>
void accessor<T>::next() {
    if constexpr (order == traversal_order::pre) preorder_increment<wing>();
    if constexpr (order == traversal_order::in) inorder_increment<wing>();
    if constexpr (order == traversal_order::post) postorder_increment<wing>();
}

template <typename T>
void accessor<T>::next(traversal_order order, side wing) {
    if (wing == side::left) {
        if (order == traversal_order::pre) preorder_increment<side::left>();
        if (order == traversal_order::in) inorder_increment<side::left>();
        if (order == traversal_order::post) postorder_increment<side::left>();
    } else {
        if (order == traversal_order::pre) preorder_increment<side::right>();
        if (order == traversal_order::in) inorder_increment<side::right>();
        if (order == traversal_order::post) postorder_increment<side::right>();
    }
}

template <typename T>
void accessor<T>::up() {
    if (depth_ == -1) { return; }
    if (depth_ == 0) {
        --depth_;
        return;
    }
    unsafe_up();
}

template <typename T>
void accessor<T>::root() {
    if (depth_ == -1) {
        depth_ = 0;
        return;
    }

    for (; depth_; unsafe_up()) {}
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
bool accessor<T>::down(side wing) {
    if (wing == side::left) {
        return down<side::left>();
    } else {
        return down<side::right>();
    }
}

template <typename T>
template <side wing>
void accessor<T>::descendant() {
    if (depth_ == -1) { ++depth_; }

    for (; node_->template has_child<wing>();
         ++depth_, node_ = node_ = &node_->template child<wing>()) {}
}

template <typename T>
void accessor<T>::descendant(side wing) {
    if (wing == side::left) {
        return descendant<side::left>();
    } else {
        return descendant<side::right>();
    }
}

template <typename T>
accessor<T> accessor<T>::common_ancestor(const accessor<T> &other) const {
    if (!*this || !other) { return *this; }

    auto [lower, higher] = std::minmax_element(
        *this, other, [](const accessor<T> &left, const accessor<T> &right) {
            return left.depth_ > right.depth_;
        });

    for (uint32_t i = lower.depth_ - higher.depth_; i; --i) {
        lower.unsafe_up();
    }

    for (; lower.depth_ && lower != higher;
         lower.unsafe_up(), higher.unsafe_up()) {}
    return lower;
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