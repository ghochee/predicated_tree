#include <algorithm>
#include <cstdlib>

template <class C, bool is_const>
accessor<C, is_const>::accessor(node_type &node) : node_(&node) {}

template <class C, bool is_const>
accessor<C, is_const>::accessor(const accessor<C, false> &other)
    : node_(other.node_) {}

template <class C, bool is_const>
typename accessor<C, is_const>::value_type_t &accessor<C, is_const>::operator*()
    const {
    return **node_;
}

template <class C, bool is_const>
typename accessor<C, is_const>::node_type &accessor<C, is_const>::node() const {
    return *node_;
}

template <class C, bool is_const>
typename accessor<C, is_const>::node_type *accessor<C, is_const>::operator->()
    const {
    return node_;
}

template <class C, bool is_const>
accessor<C, is_const>::operator bool() const {
    return node_ != nullptr;
}

template <class C, bool is_const>
bool accessor<C, is_const>::operator==(
    const accessor<C, is_const> &other) const {
    return node_ == other.node_;
}

template <class C, bool is_const>
bool accessor<C, is_const>::operator!=(
    const accessor<C, is_const> &other) const {
    return !(other == *this);
}

template <class C, bool is_const>
bool accessor<C, is_const>::is_root() const {
    return !node_->has_parent();
}

template <class C, bool is_const>
uint32_t accessor<C, is_const>::depth() const {
    if (!*this) { return -1; }
    if (is_root()) { return 0; }

    uint32_t levels = 0;
    for (auto it = *this; !it.is_root(); ++levels, it.up()) {}
    return levels;
}

template <class C, bool is_const>
void accessor<C, is_const>::up() {
    node_ = &node_->parent();
}

template <class C, bool is_const>
void accessor<C, is_const>::root() {
    if (!*this) { return; }

    for (; !is_root(); up()) {}
}

template <class C, bool is_const>
template <side wing>
bool accessor<C, is_const>::down() {
    // FIXME(ghochee): Removing this if test seems to slow down on benchmarks.
    if (!this->accessor<C, is_const>::operator bool()) { return false; }

    return node_->template has_child<wing>() &&
           (node_ = &node_->template child<wing>(), true);
}

template <class C, bool is_const>
bool accessor<C, is_const>::down(side wing) {
    SWITCH_ON_SIDE(down);
}

template <class C, bool is_const>
accessor<C, is_const> accessor<C, is_const>::common_ancestor(
    const accessor<C, is_const> &other) const {
    if (!*this || !other) { return *this; }

    uint32_t depth = this->depth(), other_depth = other.depth();
    uint32_t height_difference = depth - other_depth;
    auto lower = *this, higher = other;
    if (other_depth > depth) {
        swap(lower, higher);
        height_difference = other_depth - depth;
    }

    for (; height_difference; --height_difference) { lower.up(); }
    for (; lower && lower != higher; lower.up(), higher.up()) {}
    return lower;
}

template <class C, bool is_const>
accessor<C, false> accessor<C, is_const>::non_const() const {
    if (!this->accessor<C, is_const>::operator bool()) {
        return accessor<C, false>();
    }
    return accessor<C, false>(const_cast<C &>(*node_));
}
