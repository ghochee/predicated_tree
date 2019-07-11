constexpr side operator!(const side wing) {
    if (wing == side::left) {
        return side::right;
    } else {
        return side::left;
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
    if (children_[_left]) {
        children_[_left]->parent_ = this;
    }
    if (children_[_right]) {
        children_[_right]->parent_ = this;
    }
}

template <typename T>
raw_tree<T> &raw_tree<T>::operator=(raw_tree<T> &&other) {
    value_ = std::move(other.value_);
    children_[_left] =
        std::move(other.children_[_left]);
    children_[_right] =
        std::move(other.children_[_right]);
    parent_ = other.parent_;

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
template <side wing>
bool raw_tree<T>::has_child() const {
    return (bool)children_[as_int(wing)];
}

template <typename T>
template <side wing>
void raw_tree<T>::replace(raw_tree<T> &&child) {
    child.parent_ = this;
    this->children_[as_int(wing)] =
        std::make_unique<raw_tree<T>>(std::move(child));
}

template <typename T>
template <side wing>
raw_tree<T> raw_tree<T>::detach() {
    auto detached = std::move(children_[as_int(wing)]);
    detached->parent_ = nullptr;
    return raw_tree<T>(std::move(*detached));
}

template <typename T>
template <side wing>
raw_tree<T> &raw_tree<T>::child() {
    return *children_[as_int(wing)];
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing> raw_tree<T>::begin() {
    return raw_tree<T>::iterator<order, wing>(*this);
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing> raw_tree<T>::end() {
    return raw_tree<T>::iterator<order, wing>(this, -1);
}

template <typename T>
size_t raw_tree<T>::size() const {
    return 1 + (has_child<side::left>() ? children_[_left]->size() : 0) +
           (has_child<side::right>() ? children_[_right]->size() : 0);
}
