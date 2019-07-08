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
typename tree<T>::iterator tree<T>::erase(typename tree<T>::iterator left,
                                          typename tree<T>::iterator right) {
    for (auto pos = left, next = std::next(left);
         pos != end() && pos != right;
         pos = next, next = std::next(next)) {
        auto parent = pos.parent();
        if (parent == end()) {
            erase(root_);
            continue;
        }

        if (parent.left() == pos) {
            erase(parent.node_->left_);
        } else {
            erase(parent.node_->right_);
        }
    }
    return right;
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

template <typename T>
void tree<T>::erase(std::unique_ptr<Node> &pos) {
    if (pos->left_) { pos->left_->parent_ = pos->parent_; }
    if (pos->right_) { pos->right_->parent_ = pos->parent_; }

    pos = tree<T>::Node::merge(std::move(pos->left_), std::move(pos->right_),
                               isTall_, isLeft_);
}

// static member
template <typename T>
typename tree<T>::iterator tree<T>::end_ = tree<T>::iterator();
