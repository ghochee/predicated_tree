namespace detangled {

template <typename T, typename C>
predicated_tree<T, C>::predicated_tree(const C c) : comparator_(c) {}

template <typename T, typename C>
predicated_tree<T, C>::predicated_tree(raw_tree<T> &&tree, const C c)
    : tree_(std::move(tree)), comparator_(c) {}

template <typename T, typename C>
accessor<const raw_tree<T>> predicated_tree<T, C>::upper_bound(
    const T &value) const {
    accessor<const raw_tree<T>> pos(tree_.value());
    for (; comparator_.tall(*pos, value);) {
        if (!(comparator_.left(*pos, value)
                  ? pos.template down<side::right>()
                  : pos.template down<side::left>())) {
            return pos;
        }
    }

    // After navigating down through all *taller* nodes, we continue downward
    // through all nodes which are either left or right of 'value' i.e. we
    // continue downward until we go through nodes which are *not-equal* in
    // *left-ness* property. If we return without doing this then there is a
    // possibility of returning without finding an *equal* node which actually
    // exists in the tree at a lower level.
    for (; !comparator_.tall(value, *pos);) {
        if (comparator_.left(value, *pos)) {
            if (!pos.template down<side::left>()) { return pos; }
            continue;
        }

        if (comparator_.left(*pos, value)) {
            if (!pos.template down<side::right>()) { return pos; }
            continue;
        }

        // The values are left-equal if we reach here.
        break;
    }

    if (pos == accessor<const raw_tree<T>>(tree_.value())) {
        // If we are at 'root' of search tree (not necessarily physical tree)
        // then we return 'end' accessor.
        return accessor<raw_tree<T>>();
    }

    pos.up();
    return pos;
}

template <typename T, typename C>
accessor<const raw_tree<T>> predicated_tree<T, C>::lower_bound(
    const T &value) const {
    auto pos = upper_bound(value);
    if (!pos) { return pos; }
    if (!comparator_.equal(*pos, value)) { return pos; }

    // Find the lowest node which is equal in value.
    if (!pos.template down<side::left>()) { return pos; }
    for (; comparator_.equal(*pos, value);) {
        if (!pos.template down<side::left>()) { return pos; }
    }

    pos.up();
    return pos;
}

template <typename T, typename C>
accessor<const raw_tree<T>> predicated_tree<T, C>::find(const T &value) const {
    if (accessor<const raw_tree<T>> pos = lower_bound(value);
        pos && comparator_.equal(*pos, value)) {
        return pos;
    }
    return accessor<raw_tree<T>>();
}

template <typename T, typename C>
accessor<const raw_tree<T>> predicated_tree<T, C>::insert(
    T &&value, accessor<const raw_tree<T>> hint) {
    if (!tree_) {
        tree_.emplace(std::move(value));
        return accessor<const raw_tree<T>>(tree_.value());
    }

    // NOTE: We can cast away the const from 'hint' because we have been given
    // a reference to 'node'; 'hint' points to a node in the tree accessible
    // from 'node'. This argument correctly applies to all places where
    // reinterpret_cast is being used in this codebase.
    accessor<raw_tree<T>> &pos =
        *reinterpret_cast<accessor<raw_tree<T>> *>(&hint);

    if (!pos) { pos = accessor<raw_tree<T>>(tree_.value()); }
    for (; pos->has_parent() && comparator_.tall(value, *pos); pos.up()) {}

    // FIXME(ghochee): Insert called with non-root node might mess up the tree
    // but maybe we are OK with that. This test can be deferred to if the node
    // is getting inserted at a *periphery* i.e. at root or root-equal node. Or
    // if the result of clipping is an empty tree.
    if (!in_subtree(pos, value)) { pos = accessor<raw_tree<T>>(tree_.value()); }

    if (auto vert_pos = upper_bound(value); vert_pos) {
        pos = *reinterpret_cast<accessor<raw_tree<T>> *>(&vert_pos);
        if (comparator_.left(*pos, value)) {
            if (!pos->template has_child<side::right>()) {
                pos->template emplace<side::right>(std::move(value));
                pos.template down<side::right>();
                return pos;
            }

            if (comparator_.left(
                    value, *pos->template child<side::right>())) {
                pos->template splice<side::right, side::right>(
                    std::move(value));
            } else {
                pos->template splice<side::right, side::left>(std::move(value));
            }
            pos.template down<side::right>();
        } else {
            if (!pos->template has_child<side::left>()) {
                pos->template emplace<side::left>(std::move(value));
                pos.template down<side::left>();
                return pos;
            }

            if (comparator_.left(
                    value, *pos->template child<side::left>())) {
                pos->template splice<side::left, side::right>(std::move(value));
            } else {
                pos->template splice<side::left, side::left>(std::move(value));
            }
            pos.template down<side::left>();
        }
    } else {
        raw_tree<T> node(std::move(value));
        swap(node, pos.node());
        if (comparator_.left(*pos, *node)) {
            pos->template replace<side::right>(std::move(node));
        } else {
            pos->template replace<side::left>(std::move(node));
        }
    }

    // If the value we are inserting is equal to an existing value then we
    // don't need to run `clip` to readjust nodes. This is because the
    // left-right arrangement has already been settled for the existing `equal`
    // node.
    if (pos->template has_child<side::left>()) {
        auto existing = pos;
        existing.template down<side::left>();
        if (comparator_.equal(*pos, *existing)) {
            if (existing->template has_child<side::right>()) {
                pos->template replace<side::right>(
                    existing->template detach<side::right>());
            }

            if (existing->template has_child<side::left>() &&
                comparator_.equal(*pos,
                                  *existing->template child<side::left>())) {
                auto left_child = pos->template detach<side::left>();
                auto left_gchild = left_child.template detach<side::left>();
                if (left_gchild.template has_child<side::right>()) {
                    left_child.template replace<side::right>(
                        left_gchild.template detach<side::right>());
                }
                left_gchild.template replace<side::right>(
                    std::move(left_child));
                pos->template replace<side::left>(std::move(left_gchild));
            }

            return pos;
        }
    }

    if (pos->template has_child<side::right>()) {
        if (auto clipped =
                clip<side::left>(pos->template child<side::right>(), *pos);
            clipped) {
            pos->template replace<side::left>(std::move(*clipped));
        }
    } else {
        if (auto clipped =
                clip<side::right>(pos->template child<side::left>(), *pos);
            clipped) {
            pos->template replace<side::right>(std::move(*clipped));
        }
    }

    return pos;
}

template <typename T, typename C>
template <typename It>
accessor<const raw_tree<T>> predicated_tree<T, C>::insert(
    It begin, It end, accessor<const raw_tree<T>> hint) {
    auto pos = hint;
    for (; begin != end; ++begin) { pos = this->insert(*begin, hint); }
    return pos;
}

template <typename T, typename C>
void predicated_tree<T, C>::erase(accessor<const raw_tree<T>> const_pos) {
    // TODO(ghochee): Faster processing of only-one test.
    if (tree_ && !tree_->template has_child<side::left>() &&
        !tree_->template has_child<side::right>()) {
        return tree_.reset();
    }

    if (!const_pos) { return; }

    // NOTE: We can cast away the const from 'const_pos' because we have been
    // given a reference to the tree that it is part of; 'const_pos' points to
    // a node in the tree accessible from 'node'.
    accessor<raw_tree<T>> &pos =
        *reinterpret_cast<accessor<raw_tree<T>> *>(&const_pos);

    // We can keep going down until we reach a point where we have atmost one
    // child. At this point we can continue going down but since this node is
    // going to be removed the effect of this node `sift_down`ing through a
    // bunch of nodes which have already been heaped correctly is a no-op. The
    // call site should just move the child node up to it's grandparent and
    // squash this node.
    for (; pos->template has_child<side::left>() &&
           pos->template has_child<side::right>();) {
        if (comparator_.tall(*pos->template child<side::left>(),
                             *pos->template child<side::right>())) {
            pos->template rotate<side::right>();
            pos.template down<side::right>();
        } else {
            pos->template rotate<side::left>();
            pos.template down<side::left>();
        }
    }

    if (pos->template has_child<side::left>()) {
        pos.node() = pos->template detach<side::left>();
        return;
    }
    if (pos->template has_child<side::right>()) {
        pos.node() = pos->template detach<side::right>();
        return;
    }

    if (pos->template is_side<side::left>()) {
        pos->parent().template detach<side::left>();
    } else {
        pos->parent().template detach<side::right>();
    }
}

template <typename T, typename C>
void predicated_tree<T, C>::clear() {
    tree_.reset();
}

template <typename T, typename C>
predicated_tree<T, C>::operator bool() const {
    return tree_.has_value();
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

template <typename T, typename C>
bool predicated_tree<T, C>::in_subtree(accessor<const raw_tree<T>> pos,
                                       const T &value) const {
    if (comparator_.tall(value, *pos)) { return false; }

    // Either an *outer*-bound ancestor does not exist or it bounds `value`
    // correctly. For example, right sided ancestor of a node is a value which
    // is inorder greater than the entire subtree. If value is not greater than
    // this node then it is in the correct subtree. The left right variation
    // because right is gt and left is le.
    if (comparator_.left(*pos, value)) {
        return !pos.template ancestor<side::right>() ||
               !comparator_.left(*pos, value);
    } else {
        return !pos.template ancestor<side::left>() ||
               comparator_.left(*pos, value);
    }
}

template <typename T, typename C>
template <side wing>
std::optional<raw_tree<T>> predicated_tree<T, C>::clip(
    accessor<raw_tree<T>> main, const T &value) const {
    for (; !this->template horizontal<!wing>(value, *main);) {
        if (!main.template down<wing>()) { return std::nullopt; }
    }
    main.up();

    raw_tree<T> clipped(main->template detach<wing>());
    accessor<raw_tree<T>> seam(clipped);

    while (true) {
        for (seam.template down<!wing>();
             !this->template horizontal<wing>(value, *seam);) {
            if (!seam.template down<!wing>()) { return clipped; }
        }
        seam.up();
        main->template replace<wing>(seam->template detach<!wing>());

        for (main.template down<wing>();
             !this->template horizontal<!wing>(value, *main);) {
            if (!main.template down<wing>()) { return clipped; }
        }
        main.up();
        seam->template replace<!wing>(main->template detach<wing>());
    }
}

template <class T, class C>
template <side wing>
bool predicated_tree<T, C>::horizontal(const T &higher, const T &lower) const {
    if constexpr (wing == side::left) {
        return comparator_.left(higher, lower);
    }

    return !comparator_.left(higher, lower);
}

}  // namespace detangled
