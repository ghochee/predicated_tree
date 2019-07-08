template <typename T>
tree<T>::tree(std::function<bool(const T &, const T &)> isTall,
              std::function<bool(const T &, const T &)> isLeft)
    : isTall_(isTall), isLeft_(isLeft) {}

template <typename T>
typename tree<T>::iterator tree<T>::begin() const {
    return tree<T>::iterator(root_.get());
}

template <typename T>
typename tree<T>::iterator tree<T>::end() const {
    return tree<T>::end_;
}

template <typename T>
typename tree<T>::iterator tree<T>::insert(typename tree<T>::iterator pos,
                                           T &&value) {
    if (pos == this->end() || isTall_(value, *pos)) {
        return insert(std::move(value), root_);
    }

    auto parent = pos.parent();
    if (parent == this->end() ||
        isLeft_(*parent, value) != isLeft_(*parent, *pos)) {
        return insert(std::move(value), root_);
    }

    if (isLeft_(*pos, value)) {
        return insert(std::move(value), pos.node_->right_);
    } else {
        return insert(std::move(value), pos.node_->left_);
    }
}

template <typename T>
typename tree<T>::iterator tree<T>::insert(T &&value) {
    return tree::insert(tree<T>::end_, std::move(value));
}

template <typename T>
typename tree<T>::iterator tree<T>::insert(typename tree<T>::iterator pos,
                                           const T &value) {
    return insert(pos, T(value));
}

template <typename T>
typename tree<T>::iterator tree<T>::insert(const T &value) {
    return tree::insert(tree<T>::end_, std::move(value));
}

template <typename T>
typename tree<T>::iterator tree<T>::insert(T &&value,
                                           std::unique_ptr<Node> &pos) {
    auto node = std::make_unique<tree<T>::Node>(std::move(value));
    if (!pos) {
        pos = std::move(node);
        return tree<T>::iterator(pos.get());
    }
    node->parent_ = pos->parent_;
    pos =
        tree<T>::Node::merge(std::move(pos), std::move(node), isTall_, isLeft_);
    return tree<T>::iterator(pos.get());
}

// static member
template <typename T>
typename tree<T>::iterator tree<T>::end_ = tree<T>::iterator();
