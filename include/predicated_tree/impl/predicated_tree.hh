namespace detangled {

template <typename T, typename H, typename L>
predicated_tree<T, H, L>::predicated_tree(const H t, const L l)
    : tall(t), left(l) {}

template <typename T, typename H, typename L>
predicated_tree<T, H, L>::predicated_tree(raw_tree<T> &&tree, const H t,
                                          const L l)
    : tree_(std::move(tree)), tall(t), left(l) {}

template <typename T, typename H, typename L>
template <typename HIn, typename LIn>
predicated_tree<T, H, L>::predicated_tree(predicated_tree<T, HIn, LIn> &&ptree,
                                          const H t, const L l)
    : tree_(ptree.release()), tall(t), left(l) {
    // TODO(ghochee): If `H` (or `L`) is indifferent, we need to handle (i.e.
    // squash) equal nodes so that we can use the `lower_bound` and
    // `upper_bound` methods correctly.

    // When the incoming predicates are not equal we must sort *and* it has to
    // be done first because the partial heap-order can't be maintained while
    // the sort order is being corrected.
    if constexpr (!::std::is_same<L, LIn>::value &&
                  !::std::is_same<L, indifferent<T>>::value) {
        sort();
    }

    if constexpr (!::std::is_same<H, HIn>::value &&
                  !::std::is_same<H, indifferent<T>>::value) {
        stable_heap();
    }
}

template <typename T, typename H, typename L>
accessor<const raw_tree<T>> predicated_tree<T, H, L>::upper_bound(
    const T &value) const {
    accessor<const raw_tree<T>> pos(tree_.value());
    for (; tall(*pos, value);) {
        if (!(left(*pos, value) ? pos.template down<side::right>()
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
    for (; !tall(value, *pos);) {
        if (left(value, *pos)) {
            if (!pos.template down<side::left>()) { return pos; }
            continue;
        }

        if (left(*pos, value)) {
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

template <typename T, typename H, typename L>
accessor<const raw_tree<T>> predicated_tree<T, H, L>::lower_bound(
    const T &value) const {
    auto pos = upper_bound(value);
    if (!pos) { return pos; }
    if (!equal(*pos, value)) { return pos; }

    // Find the lowest node which is equal in value.
    if (!pos.template down<side::left>()) { return pos; }
    for (; equal(*pos, value);) {
        if (!pos.template down<side::left>()) { return pos; }
    }

    pos.up();
    return pos;
}

template <typename T, typename H, typename L>
accessor<const raw_tree<T>> predicated_tree<T, H, L>::find(
    const T &value) const {
    if (accessor<const raw_tree<T>> pos = lower_bound(value);
        pos && equal(*pos, value)) {
        return pos;
    }
    return accessor<raw_tree<T>>();
}

template <typename T, typename H, typename L>
template <typename U>
accessor<const raw_tree<T>> predicated_tree<T, H, L>::insert(
    U &&value, accessor<const raw_tree<T>> hint) {
    return insert(raw_tree<T>(std::forward<U>(value)), hint);
}

template <typename T, typename H, typename L>
template <typename It>
accessor<const raw_tree<T>> predicated_tree<T, H, L>::insert(
    It begin, It end, accessor<const raw_tree<T>> hint) {
    auto pos = hint;
    for (; begin != end; ++begin) { pos = this->insert(*begin, hint); }
    return pos;
}

template <typename T, typename H, typename L>
void predicated_tree<T, H, L>::erase(accessor<const raw_tree<T>> const_pos) {
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
        if (tall(*pos->template child<side::left>(),
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

    pos->parent().detach(pos->get_side());
}

template <typename T, typename H, typename L>
void predicated_tree<T, H, L>::clear() {
    tree_.reset();
}

template <typename T, typename H, typename L>
template <side wing, typename>
void predicated_tree<T, H, L>::rotate(accessor<const raw_tree<T>> pos) {
    (*reinterpret_cast<accessor<raw_tree<T>> *>(&pos))->template rotate<wing>();
}

template <typename T, typename H, typename L>
template <typename>
void predicated_tree<T, H, L>::rotate(accessor<const raw_tree<T>> pos,
                                      side wing) {
    (*reinterpret_cast<accessor<raw_tree<T>> *>(&pos))->rotate(wing);
}

template <typename T, typename H, typename L>
predicated_tree<T, H, L>::operator bool() const {
    return tree_.has_value();
}

template <typename T, typename H, typename L>
const T &predicated_tree<T, H, L>::operator*() const {
    return **tree_;
}

template <typename T, typename H, typename L>
const raw_tree<T> *predicated_tree<T, H, L>::operator->() const {
    return &tree_.value();
}

template <typename T, typename H, typename L>
const raw_tree<T> &predicated_tree<T, H, L>::node() const {
    return tree_.value();
}

template <typename T, typename H, typename L>
raw_tree<T> predicated_tree<T, H, L>::release() {
    auto detached = std::move(tree_.value());
    tree_.reset();
    return std::move(detached);
}

template <typename T, typename H, typename L>
void predicated_tree<T, H, L>::sort() {
    // TODO(ghochee): A better sort algorithm would have the following
    // properties:
    // - If `tree_` is already sorted should be O(N)
    // - *Minimum* distortion of current heap-order to ensure that we only
    //   incur re-heap costs where essential.
    if (!tree_) { return; }
    auto tree = release();

    for (auto it = tree.postlbegin(), jt = ::std::next(tree.postlbegin());
         jt != tree.postlend(); it = jt, ++jt) {
        insert(it->parent().detach(it->get_side()), accessor<raw_tree<T>>());
    }
    insert(::std::move(tree), accessor<raw_tree<T>>());
}

template <typename T, typename H, typename L>
void predicated_tree<T, H, L>::stable_heap() {
    // Do a postorder traversal and re-heap the top element of the subtree.
    // Reheap involves using rotate which doesn't affect the inorder sequence
    // (preserves left-ness).
    for (auto it = tree_->template begin<traversal_order::post, side::left>();
         it != tree_->template end<traversal_order::post, side::left>(); ++it) {
        for (auto pos = it;;) {
            // Determine a side of rotation (left or right), dependending on
            // presence and height-relation with children and then rotate.
            std::optional<side> rotation_side;
            if (pos->template has_child<side::left>() &&
                tall(*pos->template child<side::left>(), *pos)) {
                rotation_side.emplace(side::right);
            }

            if (pos->template has_child<side::right>() &&
                tall(*pos->template child<side::right>(),
                     rotation_side.has_value()
                         ? *pos->template child<side::left>()
                         : *pos)) {
                rotation_side.emplace(side::left);
            }

            if (!rotation_side.has_value()) { break; }

            pos->rotate(*rotation_side);
            pos.down(*rotation_side);
        }
    }
}

template <typename T, typename H, typename L>
accessor<const raw_tree<T>> predicated_tree<T, H, L>::insert(
    raw_tree<T> &&node, accessor<const raw_tree<T>> hint) {
    if (!tree_) {
        tree_ = ::std::move(node);
        return accessor<const raw_tree<T>>(tree_.value());
    }

    // NOTE: We can cast away the const from 'hint' because we have been given
    // a reference to 'node'; 'hint' points to a node in the tree accessible
    // from 'node'. This argument correctly applies to all places where
    // reinterpret_cast is being used in this codebase.
    accessor<raw_tree<T>> &pos =
        *reinterpret_cast<accessor<raw_tree<T>> *>(&hint);

    if (!pos) { pos = accessor<raw_tree<T>>(tree_.value()); }
    for (; pos->has_parent() && tall(*node, *pos); pos.up()) {}

    // FIXME(ghochee): Insert called with non-root node might mess up the tree
    // but maybe we are OK with that. This test can be deferred to if the node
    // is getting inserted at a *periphery* i.e. at root or root-equal node. Or
    // if the result of clipping is an empty tree.
    if (!in_subtree(pos, *node)) { pos = accessor<raw_tree<T>>(tree_.value()); }

    if (auto vert_pos = upper_bound(*node); vert_pos) {
        pos = *reinterpret_cast<accessor<raw_tree<T>> *>(&vert_pos);
        if (left(*pos, *node)) {
            if (!pos->template has_child<side::right>()) {
                pos->template replace<side::right>(::std::move(node));
                pos.template down<side::right>();
                return pos;
            }

            if (left(*node, *pos->template child<side::right>())) {
                node.template replace<side::right>(
                    pos->template detach<side::right>());
            } else {
                node.template replace<side::left>(
                    pos->template detach<side::right>());
            }
            pos->template replace<side::right>(::std::move(node));
            pos.template down<side::right>();
        } else {
            if (!pos->template has_child<side::left>()) {
                pos->template replace<side::left>(::std::move(node));
                pos.template down<side::left>();
                return pos;
            }

            if (left(*node, *pos->template child<side::left>())) {
                node.template replace<side::right>(
                    pos->template detach<side::left>());
            } else {
                node.template replace<side::left>(
                    pos->template detach<side::left>());
            }
            pos->template replace<side::left>(::std::move(node));
            pos.template down<side::left>();
        }
    } else {
        swap(node, pos.node());
        if (left(*pos, *node)) {
            pos->template replace<side::right>(::std::move(node));
        } else {
            pos->template replace<side::left>(::std::move(node));
        }
    }

    // If the value we are inserting is equal to an existing value then we
    // don't need to run `clip` to readjust nodes. This is because the
    // left-right arrangement has already been settled for the existing `equal`
    // node.
    if (pos->template has_child<side::left>()) {
        auto existing = pos;
        existing.template down<side::left>();
        if (equal(*pos, *existing)) {
            if (existing->template has_child<side::right>()) {
                pos->template replace<side::right>(
                    existing->template detach<side::right>());
            }

            if (existing->template has_child<side::left>() &&
                equal(*pos, *existing->template child<side::left>())) {
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

template <typename T, typename H, typename L>
bool predicated_tree<T, H, L>::in_subtree(accessor<const raw_tree<T>> pos,
                                          const T &value) const {
    if (tall(value, *pos)) { return false; }

    // Either an *outer*-bound ancestor does not exist or it bounds `value`
    // correctly. For example, right sided ancestor of a node is a value which
    // is inorder greater than the entire subtree. If value is not greater than
    // this node then it is in the correct subtree. The left right variation
    // because right is gt and left is le.
    if (left(*pos, value)) {
        return !pos.template ancestor<side::right>() || !left(*pos, value);
    } else {
        return !pos.template ancestor<side::left>() || left(*pos, value);
    }
}

template <typename T, typename H, typename L>
template <side wing>
std::optional<raw_tree<T>> predicated_tree<T, H, L>::clip(
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

template <typename T, typename H, typename L>
template <side wing>
bool predicated_tree<T, H, L>::horizontal(const T &higher,
                                          const T &lower) const {
    if constexpr (wing == side::left) { return left(higher, lower); }

    return !left(higher, lower);
}

template <typename T, typename H, typename L>
bool predicated_tree<T, H, L>::equal(const T &first, const T &second) const {
    return !(left(first, second) || left(second, first) ||
             tall(first, second) || tall(second, first));
}

}  // namespace detangled
