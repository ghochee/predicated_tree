#include <algorithm>
#include <cstdlib>

template <class C>
accessor<C>::accessor(C &node) : node_(&node) {}

template <class C>
typename accessor<C>::value_type_t &accessor<C>::operator*() const {
    return **node_;
}

template <class C>
C &accessor<C>::node() const {
    return *node_;
}

template <class C>
C *accessor<C>::operator->() const {
    return node_;
}

template <class C>
accessor<C>::operator bool() const {
    return node_ != nullptr;
}

template <class C>
bool accessor<C>::operator==(const accessor<C> &other) const {
    return node_ == other.node_;
}

template <class C>
bool accessor<C>::operator!=(const accessor<C> &other) const {
    return !(other == *this);
}

template <class C>
bool accessor<C>::is_root() const {
    return !node_->has_parent();
}

template <class C>
uint32_t accessor<C>::depth() const {
    if (!*this) { return -1; }
    if (is_root()) { return 0; }

    uint32_t levels = 0;
    for (auto it = *this; !it.is_root(); ++levels, it.unsafe_up()) {}
    return levels;
}

template <class C>
void accessor<C>::up() {
    if (node_->has_parent()) {
        unsafe_up();
    } else {
        node_ = nullptr;
    }
}

template <class C>
void accessor<C>::unsafe_up() {
    node_ = &node_->parent();
}

template <class C>
void accessor<C>::root() {
    if (!*this) { return; }

    for (; !is_root(); unsafe_up()) {}
}

template <class C>
template <side wing>
bool accessor<C>::down() {
    // FIXME(ghochee): Removing this if test seems to slow down on benchmarks.
    if (!this->accessor<C>::operator bool()) { return false; }

    return node_->template has_child<wing>() &&
           (node_ = &node_->template child<wing>(), true);
}

template <class C>
bool accessor<C>::down(side wing) {
    SWITCH_ON_SIDE(down);
}

template <class C>
accessor<C> accessor<C>::common_ancestor(const accessor<C> &other) const {
    if (!*this || !other) { return *this; }

    uint32_t depth = this->depth(), other_depth = other.depth();
    uint32_t height_difference = depth - other_depth;
    auto lower = *this, higher = other;
    if (other_depth > depth) {
        swap(lower, higher);
        height_difference = other_depth - depth;
    }

    for (; height_difference; --height_difference) { lower.unsafe_up(); }
    for (; lower && lower != higher; lower.unsafe_up(), higher.unsafe_up()) {}
    return lower;
}
