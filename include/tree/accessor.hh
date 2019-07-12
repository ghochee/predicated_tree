#include <algorithm>
#include <cstdlib>

template <typename T, template <typename> typename C>
accessor<T, C>::accessor(C<T> &node)
    : node_(&node) {
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
    return node_ != nullptr;
}

template <typename T, template <typename> typename C>
bool accessor<T, C>::operator==(const accessor<T, C> &other) const {
    return node_ == other.node_;
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
    if (!*this) { return -1; }
    if (is_root()) { return 0; }

    uint32_t levels = 0;
    for (auto it = *this; !it.is_root(); ++levels, it.unsafe_up()) {}
    return levels;
}

template <typename T, template <typename> typename C>
void accessor<T, C>::up() {
    if (node_->has_parent()) {
        unsafe_up();
    } else {
        node_ = nullptr;
    }
}

template <typename T, template <typename> typename C>
void accessor<T, C>::unsafe_up() {
    node_ = &node_->parent();
}

template <typename T, template <typename> typename C>
void accessor<T, C>::root() {
    if (!*this) {
        return;
    }

    for (; !is_root(); unsafe_up()) {}
}

template <typename T, template <typename> typename C>
template <side wing>
bool accessor<T, C>::down() {
    if (!*this) {
        return false;
    }

    return node_->template has_child<wing>() &&
           (node_ = &node_->template child<wing>(), true);
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

    for (; height_difference; --height_difference) { lower.unsafe_up(); }
    for (; lower && lower != higher; lower.unsafe_up(), higher.unsafe_up()) {}
    return lower;
}
