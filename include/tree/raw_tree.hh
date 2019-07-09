template <typename T>
raw_tree<T>::raw_tree(raw_tree &&other)
    : value_(std::move(other.value_)),
      parent_(other.parent_),
      left_(std::move(other.left_)),
      right_(std::move(other.right_)) {
    if (left_) { left_->parent_ = this; }
    if (right_) { right_->parent_ = this; }
}

template <typename T>
raw_tree<T> &raw_tree<T>::operator=(raw_tree<T> &&other) {
    value_ = std::move(other.value_);
    left_ = std::move(other.left_);
    right_ = std::move(other.right_);
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
bool raw_tree<T>::has_left() const {
    return (bool)left_;
}

template <typename T>
raw_tree<T> &raw_tree<T>::left() {
    return *left_;
}

template <typename T>
void raw_tree<T>::attach_left(raw_tree<T> &&left) {
    left_ = std::make_unique<raw_tree<T>>(std::move(left));
    left_->parent_ = this;
}

template <typename T>
raw_tree<T> raw_tree<T>::detach_left() {
    auto detached = std::move(left_);
    detached->parent_ = nullptr;
    return raw_tree<T>(std::move(*detached));
}

template <typename T>
bool raw_tree<T>::has_right() const {
    return (bool)right_;
}

template <typename T>
raw_tree<T> &raw_tree<T>::right() {
    return *right_;
}

template <typename T>
void raw_tree<T>::attach_right(raw_tree<T> &&right) {
    right_ = std::make_unique<raw_tree<T>>(std::move(right));
    right_->parent_ = this;
}

template <typename T>
raw_tree<T> raw_tree<T>::detach_right() {
    auto detached = std::move(right_);
    detached->parent_ = nullptr;
    return raw_tree<T>(std::move(*detached));
}

template <typename T>
typename raw_tree<T>::iterator raw_tree<T>::begin() {
    return raw_tree<T>::iterator(*this);
}

template <typename T>
typename raw_tree<T>::iterator raw_tree<T>::end() {
    return raw_tree<T>::end_;
}

template <typename T>
size_t raw_tree<T>::size() const {
    return 1 + (has_left() ? left_->size() : 0) +
           (has_right() ? right_->size() : 0);
}

// static member
template <typename T>
typename raw_tree<T>::iterator raw_tree<T>::end_ = raw_tree<T>::iterator();
