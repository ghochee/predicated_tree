template <class T, class H, class L>
comparator<T, H, L>::comparator(H height_comparator, L left_comparator)
    : tall(height_comparator), left(left_comparator) {}

template <class T, class H, class L>
bool comparator<T, H, L>::equal_tall(const T &first, const T &second) const {
    return !tall(first, second) && !tall(second, first);
}

template <class T, class H, class L>
bool comparator<T, H, L>::equal_left(const T &first, const T &second) const {
    return !left(first, second) && !left(second, first);
}

template <class T, class H, class L>
bool comparator<T, H, L>::equal(const T &first, const T &second) const {
    return equal_left(first, second) && equal_tall(first, second);
}

template <class T, class H, class L>
template <side wing>
bool comparator<T, H, L>::vertical(const T &first, const T &second) const {
    if (tall(first, second)) { return true; }
    return wing == side::right && !tall(second, first) &&
           equal_left(first, second);
}

template <class T, class H, class L>
template <side wing>
bool comparator<T, H, L>::horizontal(const T &first, const T &second) const {
    if constexpr (wing == side::right) { return left(second, first); }

    return !left(second, first);
}
