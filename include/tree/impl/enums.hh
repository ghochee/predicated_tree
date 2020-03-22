namespace detangled {

constexpr traversal_order operator~(const traversal_order order) {
    if (order == traversal_order::pre) {
        return traversal_order::post;
    } else if (order == traversal_order::in) {
        return traversal_order::in;
    } else {
        return traversal_order::pre;
    }
}

constexpr side operator!(const side wing) {
    if (wing == side::left) {
        return side::right;
    } else {
        return side::left;
    }
}

}  // namespace detangled
