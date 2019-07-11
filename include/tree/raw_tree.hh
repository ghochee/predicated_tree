constexpr side operator!(const side wing) {
    if (wing == side::left) {
        return side::right;
    } else {
        return side::left;
    }
}

constexpr traversal_order operator~(const traversal_order order) {
    if (order == traversal_order::pre) {
        return traversal_order::post;
    } else if (order == traversal_order::in) {
        return traversal_order::in;
    } else {
        return traversal_order::pre;
    }
}

// The following code for compaction when accessing array indices (children_).
// NOTE: https://stackoverflow.com/questions/8357240
//
// TODO(ghochee): Make this private to a class to avoid polluting global
// namespace.
template <typename E>
constexpr auto as_int(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

// TODO(ghochee): Make these private to a class to avoid polluting global
// namespace.
constexpr std::underlying_type_t<side> _left = as_int(side::left);
constexpr std::underlying_type_t<side> _right = as_int(side::right);

template <typename T>
raw_tree<T>::raw_tree(raw_tree &&other)
    : value_(std::move(other.value_)),
      parent_(other.parent_),
      children_(std::move(other.children_)) {
    if (children_[_left]) { children_[_left]->parent_ = this; }
    if (children_[_right]) { children_[_right]->parent_ = this; }
}

template <typename T>
raw_tree<T> &raw_tree<T>::operator=(raw_tree<T> &&other) {
    value_ = std::move(other.value_);
    parent_ = other.parent_;
    children_ = std::move(other.children_);
    if (children_[_left]) { children_[_left]->parent_ = this; }
    if (children_[_right]) { children_[_right]->parent_ = this; }

    return *this;
}

template <typename T>
T &raw_tree<T>::operator*() {
    return value_;
}

template <typename T>
bool raw_tree<T>::has_parent() const {
    return parent_;
}

template <typename T>
raw_tree<T> &raw_tree<T>::parent() {
    return *parent_;
}

template <typename T>
template <side wing>
bool raw_tree<T>::is_side() const {
    return has_parent() && parent_->children_[as_int(wing)].get() == this;
}

template <typename T>
bool raw_tree<T>::is_side(side wing) const {
    if (wing == side::left) {
        return is_side<side::left>();
    } else {
        return is_side<side::right>();
    }
}

template <typename T>
template <side wing>
bool raw_tree<T>::has_child() const {
    return (bool)children_[as_int(wing)];
}

template <typename T>
bool raw_tree<T>::has_child(side wing) const {
    if (wing == side::left) {
        return has_child<side::left>();
    } else {
        return has_child<side::right>();
    }
}

template <typename T>
template <side wing>
raw_tree<T> &raw_tree<T>::child() {
    return *children_[as_int(wing)];
}

template <typename T>
raw_tree<T> &raw_tree<T>::child(side wing) {
    if (wing == side::left) {
        return child<side::left>();
    } else {
        return child<side::right>();
    }
}

template <typename T>
template <side wing>
void raw_tree<T>::rotate() {
    // NOTE: Examples and variable names describe 'right' rotation.
    // NOTE: Variable names based on pre-rotation positions.
    std::swap(**children_[as_int(!wing)], value_);

    // Move left child to right position. Hold right.
    auto right = std::exchange(
        children_[as_int(wing)],
        std::move(children_[as_int(!wing)]));
    // Move right child to right of left child. Hold left's right child.
    auto left_right = std::exchange(
        children_[as_int(wing)]->children_[as_int(wing)],
        std::move(right));
    // Move left's right child to left's left position. Hold left's left child.
    auto left_left =
        std::exchange(children_[as_int(wing)]->children_[as_int(!wing)],
                      std::move(left_right));
    // Move left's left child to left position.
    children_[as_int(!wing)] = std::move(left_left);

    // Correct parent pointers for the following if these nodes exist:
    // Right child is now a right-right grandchild.
    // left-left grandchild is now a left child.
    if (children_[as_int(wing)]->children_[as_int(wing)]) {
        children_[as_int(wing)]->children_[as_int(wing)]->parent_ =
            children_[as_int(wing)].get();
    }
    if (children_[as_int(!wing)]) {
        children_[as_int(!wing)]->parent_ = this;
    }
}

template <typename T>
void raw_tree<T>::rotate(side wing) {
    if (wing == side::left) {
        return rotate<side::left>();
    } else {
        return rotate<side::right>();
    }
}

template <typename T>
template <side wing>
void raw_tree<T>::replace(raw_tree<T> &&child) {
    child.parent_ = this;
    this->children_[as_int(wing)] =
        std::make_unique<raw_tree<T>>(std::move(child));
}

template <typename T>
void raw_tree<T>::replace(side wing, raw_tree<T> &&child) {
    if (wing == side::left) {
        return replace<side::left>(std::move(child));
    } else {
        return replace<side::right>(std::move(child));
    }
}

template <typename T>
template <side wing>
raw_tree<T> raw_tree<T>::detach() {
    auto detached = std::move(children_[as_int(wing)]);
    detached->parent_ = nullptr;
    return raw_tree<T>(std::move(*detached));
}

template <typename T>
raw_tree<T> raw_tree<T>::detach(side wing) {
    if (wing == side::left) {
        return detach<side::left>();
    } else {
        return detach<side::right>();
    }
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing> raw_tree<T>::begin() {
    return raw_tree<T>::iterator<order, wing>(*this);
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing> raw_tree<T>::end() {
    return raw_tree<T>::iterator<order, wing>(*this, -1);
}

template <typename T>
size_t raw_tree<T>::size() const {
    return 1 + (has_child<side::left>() ? children_[_left]->size() : 0) +
           (has_child<side::right>() ? children_[_right]->size() : 0);
}
