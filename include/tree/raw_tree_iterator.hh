template <typename T>
template <traversal_order order, side wing>
raw_tree<T>::template iterator<order, wing>::iterator(raw_tree<T> &node)
    : accessor<T>(node, -1) {
    ++(*this);
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing>
    &raw_tree<T>::template iterator<order, wing>::operator++() {
    this->template next<order, wing>();
    return *this;
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing>
    raw_tree<T>::template iterator<order, wing>::operator++(int) {
    auto return_value = *this;
    ++(*this);
    return return_value;
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing>
    &raw_tree<T>::template iterator<order, wing>::operator--() {
    this->template next<~order, !wing>();
    return *this;
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing>
    raw_tree<T>::template iterator<order, wing>::operator--(int) {
    auto return_value = *this;
    --(*this);
    return return_value;
}
