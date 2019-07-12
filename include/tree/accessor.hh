#include <algorithm>
#include <cstdlib>

template <typename T, template <typename> typename C>
accessor<T, C>::accessor(C<T> &node, int16_t depth)
    : node_(&node), depth_(depth) {
    if (!(depth_ >= -1)) { ::std::abort(); }

    for (C<T> *node_ptr = &node; depth > 0;
         --depth, node_ptr = std::exchange(node_ptr, &node_ptr->parent())) {
        if (!node_ptr->has_parent()) { ::std::abort(); }
    }
}

template <typename T, template <typename> typename C>
T &accessor<T, C>::operator*() const {
    return **node_;
}

template <typename T, template <typename> typename C>
C<T> &accessor<T, C>::node() const {
    return *node_;
}

template <typename T, template <typename> typename C>
C<T> *accessor<T, C>::operator->() const {
    return node_;
}

template <typename T, template <typename> typename C>
accessor<T, C>::operator bool() const {
    return depth_ >= 0;
}

template <typename T, template <typename> typename C>
bool accessor<T, C>::operator==(const accessor<T, C> &other) const {
    return depth_ == other.depth_ && node_ == other.node_;
}

template <typename T, template <typename> typename C>
bool accessor<T, C>::operator!=(const accessor<T, C> &other) const {
    return !(other == *this);
}

template <typename T, template <typename> typename C>
bool accessor<T, C>::is_root() const {
    return !node_->has_parent();
}

template <typename T, template <typename> typename C>
uint32_t accessor<T, C>::depth() const {
    return depth_;
}

template <typename T, template <typename> typename C>
void accessor<T, C>::up() {
    node_ = &node_->parent();
    --depth_;
}

template <typename T, template <typename> typename C>
void accessor<T, C>::root() {
    if (!*this) {
        depth_ = 0;
        return;
    }

    for (; !is_root(); up()) {}
}

template <typename T, template <typename> typename C>
template <side wing>
bool accessor<T, C>::down() {
    if (!*this) {
        depth_ = 0;
        return true;
    }

    return node_->template has_child<wing>() &&
           (++depth_, node_ = &node_->template child<wing>(), true);
}

template <typename T, template <typename> typename C>
bool accessor<T, C>::down(side wing) {
    SWITCH_ON_SIDE(down);
}

template <typename T, template <typename> typename C>
accessor<T, C> accessor<T, C>::common_ancestor(
    const accessor<T, C> &other) const {
    if (!*this || !other) { return *this; }

    uint32_t depth = this->depth(), other_depth = other.depth();
    uint32_t height_difference = depth - other_depth;
    auto lower = *this, higher = other;
    if (other_depth > depth) {
        std::swap(lower, higher);
        height_difference = other_depth - depth;
    }

    for (; height_difference; --height_difference) { lower.up(); }
    for (; lower && lower != higher; lower.up(), higher.up()) {}
    return lower;
}

template <typename T, template <typename> typename C>
template <traversal_order order, side wing>
void accessor<T, C>::next() {
    if (!*this) { return handle_end<order, wing>(); }
    if constexpr (order == traversal_order::pre) preorder_increment<wing>();
    if constexpr (order == traversal_order::in) inorder_increment<wing>();
    if constexpr (order == traversal_order::post) postorder_increment<wing>();
}

template <typename T, template <typename> typename C>
void accessor<T, C>::next(traversal_order order, side wing) {
    if (!*this) { return handle_end(order, wing); }
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

template <typename T, template <typename> typename C>
template <traversal_order order, side wing>
void accessor<T, C>::handle_end() {
    if constexpr (order == traversal_order::pre) {
        root();
        return;
    }

    if constexpr (order == traversal_order::in) {
        for (; down<wing>();) {}
        return;
    }

    if constexpr (order == traversal_order::post) {
        for (; down<wing>() || down<!wing>();) {}
        return;
    }
}

template <typename T, template <typename> typename C>
void accessor<T, C>::handle_end(traversal_order order, side wing) {
    // clang-format off
    if (wing == side::left) {
        if (order == traversal_order::pre) handle_end<traversal_order::pre, side::left>();
        if (order == traversal_order::in) handle_end<traversal_order::pre, side::left>();
        if (order == traversal_order::post) handle_end<traversal_order::pre, side::left>();
    } else {
        if (order == traversal_order::pre) handle_end<traversal_order::pre, side::right>();
        if (order == traversal_order::in) handle_end<traversal_order::pre, side::right>();
        if (order == traversal_order::post) handle_end<traversal_order::pre, side::right>();
    }
    // clang-format on
}

template <typename T, template <typename> typename C>
template <side wing>
void accessor<T, C>::preorder_increment() {
    if (down<wing>() || down<!wing>()) { return; }

    for (; depth_ && (node_->template is_side<!wing>() ||
                      !node_->parent().template has_child<!wing>());
         up()) {}

    if (depth_ == 0) { return up(); }

    node_ = &node_->parent().template child<!wing>();
}

template <typename T, template <typename> typename C>
template <side wing>
void accessor<T, C>::inorder_increment() {
    if (node_->template has_child<!wing>()) {
        for (down<!wing>(); down<wing>();) {}
        return;
    }

    for (; depth_ && node_->template is_side<!wing>(); up()) {}
    up();
}

template <typename T, template <typename> typename C>
template <side wing>
void accessor<T, C>::postorder_increment() {
    if (depth_ == 0) { return up(); }

    if (depth_ > 0 && (node_->template is_side<!wing>() ||
                       !(node_->parent().template has_child<!wing>()))) {
        return up();
    }

    for (up(), down<!wing>(); down<wing>() || down<!wing>();) {}
}
