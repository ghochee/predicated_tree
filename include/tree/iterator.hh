template <class C, traversal_order order, side wing>
iterator<C, order, wing>::iterator(node_type &node)
    : base_type(traverser<base_type, order, wing>::begin(node)) {}

template <class C, traversal_order order, side wing>
iterator<C, order, wing> &iterator<C, order, wing>::operator++() {
    traverser<decltype(*this), order, wing>::next(*this);
    return *this;
}

template <class C, traversal_order order, side wing>
iterator<C, order, wing> iterator<C, order, wing>::operator++(int) {
    auto return_value = *this;
    ++(*this);
    return return_value;
}

template <class C, traversal_order order, side wing>
iterator<C, order, wing> &iterator<C, order, wing>::operator--() {
    traverser<decltype(*this), ~order, !wing>::next(*this);
    return *this;
}

template <class C, traversal_order order, side wing>
iterator<C, order, wing> iterator<C, order, wing>::operator--(int) {
    auto return_value = *this;
    --(*this);
    return return_value;
}
