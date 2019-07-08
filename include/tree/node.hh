// static method.
template <typename T>
template <typename Comparator>
std::tuple<std::unique_ptr<typename tree<T>::Node>,
           std::unique_ptr<typename tree<T>::Node>>
tree<T>::Node::myminmax(std::unique_ptr<typename tree<T>::Node> &&first,
                        std::unique_ptr<typename tree<T>::Node> &&second,
                        const Comparator &comparator) {
    if (comparator(first->value_, second->value_)) {
        return {std::move(first), std::move(second)};
    } else {
        return {std::move(second), std::move(first)};
    }
}

// static method.
template <typename T>
template <typename HeightComparator, typename LeftComparator>
std::unique_ptr<typename tree<T>::Node> tree<T>::Node::merge(
    std::unique_ptr<tree<T>::Node> &&first,
    std::unique_ptr<tree<T>::Node> &&second,
    const HeightComparator &heightComparator,
    const LeftComparator &leftComparator) {
    if (!first) { return std::move(second); }
    if (!second) { return std::move(first); }

    auto [taller, shorter] =
        myminmax(std::move(first), std::move(second), heightComparator);

    std::unique_ptr<tree<T>::Node> inside;
    if (leftComparator(taller->value_, shorter->value_)) {
        std::swap(inside, shorter->left_);
        taller->right_ = merge(std::move(taller->right_), std::move(shorter));
    } else {
        std::swap(inside, shorter->right_);
        taller->left_ = merge(std::move(taller->left_), std::move(shorter));
    }
    taller = merge(std::move(taller), std::move(inside));
    return std::move(taller);
}
