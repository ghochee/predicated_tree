template <typename T>
raw_tree<T>::raw_tree(raw_tree &&other)
    : value_(std::move(other.value_)),
      parent_(other.parent_),
      children_{std::move(other.children_[static_cast<int>(side::left)]),
                std::move(other.children_[static_cast<int>(side::right)])} {
    if (children_[static_cast<int>(side::left)]) {
        children_[static_cast<int>(side::left)]->parent_ = this;
    }
    if (children_[static_cast<int>(side::right)]) {
        children_[static_cast<int>(side::right)]->parent_ = this;
    }
}

template <typename T>
raw_tree<T> &raw_tree<T>::operator=(raw_tree<T> &&other) {
    value_ = std::move(other.value_);
    children_[static_cast<int>(side::left)] =
        std::move(other.children_[static_cast<int>(side::left)]);
    children_[static_cast<int>(side::right)] =
        std::move(other.children_[static_cast<int>(side::right)]);
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
bool raw_tree<T>::has_child() const {
    return (bool)children_[static_cast<int>(wing)];
}

template <typename T>
template <side wing>
void raw_tree<T>::replace(raw_tree<T> &&child) {
    this->children_[static_cast<int>(wing)] =
        std::make_unique<raw_tree<T>>(std::move(child));
    this->children_[static_cast<int>(wing)]->parent_ = this;
}

template <typename T>
template <side wing>
raw_tree<T> raw_tree<T>::detach() {
    auto detached = std::move(children_[static_cast<int>(wing)]);
    detached->parent_ = nullptr;
    return raw_tree<T>(std::move(*detached));
}

template <typename T>
template <side wing>
raw_tree<T> &raw_tree<T>::child() {
    return *children_[static_cast<int>(wing)];
}

template <typename T>
template <traversal_order order>
typename raw_tree<T>::template iterator<order> raw_tree<T>::begin() {
    return raw_tree<T>::iterator<order>(*this);
}

template <typename T>
template <traversal_order order>
typename raw_tree<T>::template iterator<order> raw_tree<T>::end() {
    return raw_tree<T>::iterator<order>();
}

template <typename T>
size_t raw_tree<T>::size() const {
    return 1 +
           (has_child<side::left>()
                ? children_[static_cast<int>(side::left)]->size()
                : 0) +
           (has_child<side::right>()
                ? children_[static_cast<int>(side::right)]->size()
                : 0);
}

template <typename T>
template <side wing>
const std::unique_ptr<raw_tree<T>> &raw_tree<T>::child_ref() const {
    if constexpr (wing == side::left) {
        return children_[static_cast<int>(side::left)];
    }

    if constexpr (wing == side::right) {
        return children_[static_cast<int>(side::right)];
    }
}

// TODO(ghochee): The following needs to be DRY with the above method but the
// standard method is to do a const_cast which I am uncomfortable with. Also
// doing that on the unique_ptr ref is causing a segfault.
template <typename T>
template <side wing>
std::unique_ptr<raw_tree<T>> &raw_tree<T>::child_ref() {
    if constexpr (wing == side::left) {
        return children_[static_cast<int>(side::left)];
    }

    if constexpr (wing == side::right) {
        return children_[static_cast<int>(side::right)];
    }
}
