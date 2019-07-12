template <typename T, template <typename> typename C>
virtual_accessor<T, C>::virtual_accessor(C<T> &node, int16_t depth)
    : base_type(node),
      depth_(depth) {
    if (depth_ != -1 && depth_ != 0) { ::std::abort(); }
}

template <typename T, template <typename> typename C>
virtual_accessor<T, C>::operator bool() const {
    return depth_ >= 0;
}

template <typename T, template <typename> typename C>
bool virtual_accessor<T, C>::operator==(
    const virtual_accessor<T, C> &other) const {
    return static_cast<const base_type *>(this)->operator==(
               static_cast<const base_type>(other)) &&
           depth_ == other.depth_;
}

template <typename T, template <typename> typename C>
bool virtual_accessor<T, C>::operator!=(
    const virtual_accessor<T, C> &other) const {
    return !(other == *this);
}

template <typename T, template <typename> typename C>
bool virtual_accessor<T, C>::is_root() const {
    return depth_ == 0;
}

template <typename T, template <typename> typename C>
uint32_t virtual_accessor<T, C>::depth() const {
    return depth_;
}

template <typename T, template <typename> typename C>
void virtual_accessor<T, C>::up() {
    if (depth_ == -1) {
        return;
    }

    if (depth_) {
        static_cast<base_type *>(this)->up();
    }
    --depth_;
}

template <typename T, template <typename> typename C>
void virtual_accessor<T, C>::root() {
    if (depth_ == -1) {
        depth_ = 0;
        return;
    }

    for (; !is_root(); up()) {}
}

template <typename T, template <typename> typename C>
template <side wing>
bool virtual_accessor<T, C>::down() {
    if (depth_ == -1) {
        depth_ = 0;
        return true;
    }

    if (static_cast<base_type *>(this)->template down<wing>()) {
        ++depth_;
        return true;
    }

    return false;
}

template <typename T, template <typename> typename C>
bool virtual_accessor<T, C>::down(side wing) {
    if (static_cast<base_type *>(this)->template down(wing)) {
        ++depth_;
        return true;
    }
    return false;
}
