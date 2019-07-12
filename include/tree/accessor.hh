#include <algorithm>
#include <cstdlib>

template <typename T, template <typename> typename C>
accessor<T, C>::accessor(C<T> &node, int16_t depth)
    : node_(&node), depth_(depth) {
    if (depth_ != -1 && depth_ != 0) { ::std::abort(); }
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
