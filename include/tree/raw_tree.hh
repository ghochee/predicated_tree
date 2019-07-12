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
const T &raw_tree<T>::operator*() const {
    return value_;
}

template <typename T>
T &raw_tree<T>::operator*() {
    return const_cast<T &>(
        const_cast<const raw_tree<T> *>(this)->raw_tree<T>::operator*());
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
const raw_tree<T> &raw_tree<T>::parent() const {
    return *parent_;
}

template <typename T>
template <side wing>
bool raw_tree<T>::is_side() const {
    return has_parent() && parent_->children_[as_int(wing)].get() == this;
}

template <typename T>
bool raw_tree<T>::is_side(side wing) const {
    SWITCH_ON_SIDE(is_side);
}

template <typename T>
template <side wing>
bool raw_tree<T>::has_child() const {
    return (bool)children_[as_int(wing)];
}

template <typename T>
bool raw_tree<T>::has_child(side wing) const {
    SWITCH_ON_SIDE(has_child);
}

template <typename T>
template <side wing>
const raw_tree<T> &raw_tree<T>::child() const {
    return *children_[as_int(wing)];
}

template <typename T>
const raw_tree<T> &raw_tree<T>::child(side wing) const {
    SWITCH_ON_SIDE(child);
}

template <typename T>
template <side wing>
raw_tree<T> &raw_tree<T>::child() {
    return const_cast<raw_tree<T> &>(
        const_cast<const raw_tree<T> *>(this)->raw_tree<T>::child<wing>());
}

template <typename T>
raw_tree<T> &raw_tree<T>::child(side wing) {
    return const_cast<raw_tree<T> &>(
        const_cast<const raw_tree<T> *>(this)->raw_tree<T>::child(wing));
}

template <typename T>
template <side C, side GC>
void raw_tree<T>::reshape() {
    // NOTE: Variable names based on pre-rotation positions.
    std::swap(**children_[as_int(!C)], value_);

    std::unique_ptr<raw_tree<T>> child_node =
        std::exchange(children_[as_int(!C)],
                      std::move(children_[as_int(!C)]->children_[as_int(!C)]));
    if (children_[as_int(!C)]) { children_[as_int(!C)]->parent_ = this; }

    child_node->children_[as_int(!C)] =
        std::move(child_node->children_[as_int(C)]);

    if constexpr (GC == C) {
        if (children_[as_int(C)]) {
            children_[as_int(C)]->parent_ = child_node.get();
            child_node->children_[as_int(C)] = std::move(children_[as_int(C)]);
        }
        children_[as_int(C)] = std::move(child_node);
    } else {
        if (children_[as_int(C)]->children_[as_int(!C)]) {
            children_[as_int(C)]->children_[as_int(!C)]->parent_ =
                child_node.get();
            child_node->children_[as_int(C)] =
                std::move(children_[as_int(C)]->children_[as_int(!C)]);
        }
        child_node->parent_ = children_[as_int(C)].get();
        children_[as_int(C)]->children_[as_int(!C)] = std::move(child_node);
    }
}

template <typename T>
template <side wing>
void raw_tree<T>::rotate() {
    return reshape<wing, wing>();
}

template <typename T>
void raw_tree<T>::rotate(side wing) {
    SWITCH_ON_SIDE(rotate);
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
    SWITCH_ON_SIDE(replace, std::move(child));
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
    SWITCH_ON_SIDE(detach);
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
