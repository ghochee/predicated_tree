namespace detangled {

template <class C>
virtual_accessor<C>::virtual_accessor(base_type &pos)
    : base_type(pos), depth_(pos->depth()) {}

template <class C>
virtual_accessor<C>::virtual_accessor(node_type &node, int16_t depth)
    : base_type(node), depth_(depth) {
    if (depth_ != (uint64_t)-1 && depth_ != 0) { ::std::abort(); }
}

template <class C>
virtual_accessor<C>::operator bool() const {
    return depth_ != (uint64_t)-1;
}

template <class C>
bool virtual_accessor<C>::operator==(const virtual_accessor<C> &other) const {
    return static_cast<const base_type *>(this)->operator==(
               static_cast<const base_type>(other)) &&
           depth_ == other.depth_;
}

template <class C>
bool virtual_accessor<C>::operator!=(const virtual_accessor<C> &other) const {
    return !(other == *this);
}

template <class C>
bool virtual_accessor<C>::is_root() const {
    return depth_ == 0;
}

template <class C>
uint32_t virtual_accessor<C>::depth() const {
    return depth_;
}

template <class C>
void virtual_accessor<C>::up() {
    if (depth_ == (uint64_t)-1) { return; }

    if (depth_) { static_cast<base_type *>(this)->up(); }
    --depth_;
}

template <class C>
void virtual_accessor<C>::root() {
    if (depth_ == (uint64_t)-1) {
        depth_ = 0;
        return;
    }

    for (; !is_root(); up()) {}
}

template <class C>
template <side wing>
bool virtual_accessor<C>::down() {
    if (depth_ == (uint64_t)-1) {
        depth_ = 0;
        return true;
    }

    if (static_cast<base_type *>(this)->template down<wing>()) {
        ++depth_;
        return true;
    }

    return false;
}

template <class C>
bool virtual_accessor<C>::down(side wing) {
    if (static_cast<base_type *>(this)->down(wing)) {
        ++depth_;
        return true;
    }
    return false;
}

}  // namespace detangled
