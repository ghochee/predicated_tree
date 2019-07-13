template <typename T, typename C>
predicated_tree<T, C>::predicated_tree(const C c) : mutator_(c) {}

template <typename T, typename C>
accessor<const raw_tree<T>> predicated_tree<T, C>::upper_bound(
    const T &value) const {
    return mutator_.upper_bound(tree_.value(), value);
}

template <typename T, typename C>
accessor<const raw_tree<T>> predicated_tree<T, C>::lower_bound(
    const T &value) const {
    return mutator_.lower_bound(tree_.value(), value);
}

template <typename T, typename C>
accessor<const raw_tree<T>> predicated_tree<T, C>::find(const T &value) const {
    return mutator_.find(tree_.value(), value);
}

template <typename T, typename C>
accessor<const raw_tree<T>> predicated_tree<T, C>::insert(
    T &&value, accessor<const raw_tree<T>> hint) {
    if (!tree_) {
        tree_.emplace(std::move(value));
        return accessor<const raw_tree<T>>(tree_.value());
    }

    return mutator_.insert(std::move(value), tree_.value(), hint);
}

template <typename T, typename C>
template <typename It>
accessor<const raw_tree<T>> predicated_tree<T, C>::insert(
    It begin, It end, accessor<const raw_tree<T>> hint) {
    if (begin == end) { return; }

    if (!tree_) { tree_.emplace(std::move(*begin++)); }
    return mutator_.insert(begin, end, tree_.value(), hint);
}

template <typename T, typename C>
void predicated_tree<T, C>::erase(accessor<const raw_tree<T>> pos) {
    // FIXME(ghochee): Faster processing of only-one test.
    if (tree_ && !tree_->template has_child<side::left>() &&
        !tree_->template has_child<side::right>()) {
        pos = accessor<const raw_tree<T>>();
        return tree_.reset();
    }
    return mutator_.erase(tree_.value(), pos);
}

template <typename T, typename C>
void predicated_tree<T, C>::clear() {
    tree_.reset();
}

template <typename T, typename C>
const T &predicated_tree<T, C>::operator*() const {
    return **tree_;
}

template <typename T, typename C>
const raw_tree<T> *predicated_tree<T, C>::operator->() const {
    return &tree_.value();
}

template <typename T, typename C>
const raw_tree<T> &predicated_tree<T, C>::node() const {
    return tree_.value();
}

template <typename T, typename C>
raw_tree<T> predicated_tree<T, C>::release() {
    auto detached = std::move(tree_.value());
    tree_.reset();
    return std::move(detached);
}
