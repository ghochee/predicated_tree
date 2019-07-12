#include <optional>

template <typename T, typename C>
mutator<T, C>::mutator(const C comparator) : comparator_(comparator) {}

template <typename T, typename C>
bool mutator<T, C>::in_subtree(const raw_tree<T> &node, const T &value) const {
    if (comparator_.tall(value, *node)) { return false; }

    accessor<const raw_tree<T>> pos(node);

    if (comparator_.template horizontal<side::right>(value, *pos)) {
        return !pos.template ancestor<side::right>() ||
               comparator_.template horizontal<side::left>(value, *pos);
    } else {
        return !pos.template ancestor<side::left>() ||
               comparator_.template horizontal<side::right>(value, *pos);
    }
}

template <typename T, typename C>
template <class ContainerType>
accessor<ContainerType> mutator<T, C>::find(ContainerType &tree,
                                            const T &value) const {
    if (accessor<ContainerType> pos = lower_bound(tree, value);
        pos && equal(*pos, value)) {
        return pos;
    }
    return accessor<ContainerType>();
}

template <typename T, typename C>
template <class ContainerType>
accessor<ContainerType> mutator<T, C>::upper_bound(ContainerType &tree,
                                                   const T &value) const {
    accessor<ContainerType> pos(tree);
    for (; comparator_.tall(*pos, value);) {
        if (!(comparator_.template horizontal<side::right>(value, *pos)
                  ? pos.template down<side::right>()
                  : pos.template down<side::left>())) {
            return pos;
        }
    }

    for (; !comparator_.tall(value, *pos);) {
        if (comparator_.left(value, *pos)) {
            if (!pos.template down<side::left>()) { return pos; }
        } else if (comparator_.left(*pos, value)) {
            if (!pos.template down<side::right>()) { return pos; }
        } else {
            break;
        }
    }

    if (!pos->has_parent()) { return accessor<ContainerType>(); }

    pos.up();
    return pos;
}

template <typename T, typename C>
template <class ContainerType>
accessor<ContainerType> mutator<T, C>::lower_bound(ContainerType &tree,
                                                   const T &value) const {
    auto pos = upper_bound(tree, value);
    if (pos) {
        if (comparator_.template horizontal<side::right>(value, *pos)) {
            if (!pos.template down<side::right>()) { return pos; }
        } else {
            if (!pos.template down<side::left>()) { return pos; }
        }
    } else {
        pos = accessor<ContainerType>(tree);
    }

    for (; equal(*pos, value);) {
        if (!pos.template down<side::left>()) { return pos; }
    }

    if (!pos->has_parent()) { return accessor<ContainerType>(); }

    pos.up();
    return pos;
}

template <typename T, typename C>
accessor<raw_tree<T>> mutator<T, C>::insert(T &&value, raw_tree<T> &node,
                                            accessor<const raw_tree<T>> hint) {
    // NOTE: We can cast away the const from 'hint' because we have been given
    // a reference to 'node'; 'hint' points to a node in the tree accessible
    // from 'node'.
    accessor<raw_tree<T>> &pos =
        *reinterpret_cast<accessor<raw_tree<T>> *>(&hint);

    if (!pos) { pos = accessor<raw_tree<T>>(node); }
    for (; pos->has_parent() && comparator_.tall(value, *pos); pos.up()) {}
    if (!in_subtree(pos.node(), value)) { pos = accessor<raw_tree<T>>(node); }

    if (auto vert_pos = lower_bound(pos.node(), value); vert_pos) {
        pos = vert_pos;
        // FIXME(ghochee): Private member function template for DRY-ing this.
        if (comparator_.template horizontal<side::right>(value, *pos)) {
            if (!pos->template has_child<side::right>()) {
                pos->template emplace<side::right>(std::move(value));
                pos.template down<side::right>();
                return pos;
            }

            if (comparator_.template horizontal<side::right>(
                    *pos->template child<side::right>(), value)) {
                pos->template splice<side::right, side::right>(std::move(value));
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

            // When equal_left, we don't need to do clipping or other
            // comparison. The child will move further left as this node gets
            // inserted.
            if (comparator_.equal_left(value, *pos)) {
                pos->template splice<side::left, side::left>(std::move(value));
                pos.template down<side::left>();
                return pos;
            }

            if (comparator_.template horizontal<side::right>(
                    *pos->template child<side::left>(), value)) {
                pos->template splice<side::left, side::right>(std::move(value));
            } else {
                pos->template splice<side::left, side::left>(std::move(value));
            }
            pos.template down<side::left>();
        }
    } else {
        raw_tree<T> node(std::move(value));
        swap(node, pos.node());
        if (comparator_.template horizontal<side::right>(*node, *pos)) {
            pos->template replace<side::right>(std::move(node));
            if (!pos->template child<side::right>()
                     .template has_child<side::left>()) {
                return pos;
            }
        } else {
            pos->template replace<side::left>(std::move(node));
            if (!pos->template child<side::left>()
                     .template has_child<side::right>()) {
                return pos;
            }
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
accessor<raw_tree<T>> mutator<T, C>::insert(It begin, It end, raw_tree<T> &node,
                                            accessor<const raw_tree<T>> pos) {
    for (; begin != end; ++begin) { this->insert(*begin, node, pos); }
    return pos;
}

template <typename T, typename C>
void mutator<T, C>::erase(raw_tree<T> &,
                          accessor<const raw_tree<T>> const_pos) {
    if (!const_pos) { return; }

    // NOTE: We can cast away the const from 'const_pos' because we have been
    // given a reference to the tree that it is part of; 'const_pos' points to
    // a node in the tree accessible from 'node'.
    accessor<raw_tree<T>> &pos =
        *reinterpret_cast<accessor<raw_tree<T>> *>(&const_pos);

    bool left = pos->template has_child<side::left>();
    bool right = pos->template has_child<side::right>();

    std::optional<raw_tree<T>> merged;
    if (!left && right) {
        merged.emplace(pos->template detach<side::right>());
    } else if (left && !right) {
        merged.emplace(pos->template detach<side::left>());
    } else if (left && right) {
        merged.emplace(zip(pos->template detach<side::left>(),
                           pos->template detach<side::right>()));
    }

    if (pos->has_parent()) {
        auto parent = pos;
        parent.up();
        side wing =
            pos->template is_side<side::left>() ? side::left : side::right;
        if (merged.has_value()) {
            parent->replace(wing, std::move(merged.value()));
        } else {
            parent->detach(wing);
        }
    } else {
        // NOTE: Undefined if we are removing the last node in the tree.
        pos.node() = std::move(merged.value());
    }
}

/*
template <typename T, typename C>
raw_tree<T> mutator<T, C>::merge(raw_tree<T> &&first, raw_tree<T> &&second) {
}
*/

template <typename T, typename C>
void mutator<T, C>::stable_make_heap(raw_tree<T> &tree) {
    for (auto it = tree.template begin<traversal_order::post, side::left>();
         it != tree.template end<traversal_order::post, side::left>(); ++it) {
        bool leftTall =
            it->template has_child<side::left>() &&
            comparator_.tall(*it->template child<side::left>(), *it);
        bool rightTall =
            it->template has_child<side::right>() &&
            comparator_.tall(*it->template child<side::right>(), *it);

        if (!leftTall && !rightTall) { continue; }

        if (!leftTall && rightTall) {
            it->template reshape<side::left, side::left>();
            continue;
        }

        if (leftTall && !rightTall) {
            it->template reshape<side::right, side::right>();
            continue;
        }

        if (comparator_.tall(*it->template child<side::left>(),
                             *it->template child<side::right>())) {
            it->template reshape<side::right, side::left>();
        } else {
            it->template reshape<side::left, side::right>();
        }
    }
}

template <typename T, typename C>
void mutator<T, C>::stable_sort(raw_tree<T> &tree) {
    for (auto it = tree.template begin<traversal_order::post, side::left>();
         it != tree.template end<traversal_order::post, side::left>(); ++it) {}
}

template <typename T, typename C>
raw_tree<T> mutator<T, C>::zip(raw_tree<T> &&left, raw_tree<T> &&right) const {
    if (comparator_.tall(*left, *right)) {
        return zip<side::right>(std::move(left), std::move(right));
    } else {
        return zip<side::left>(std::move(right), std::move(left));
    }
}

template <typename T, typename C>
template <side wing>
raw_tree<T> mutator<T, C>::zip(raw_tree<T> &&main,
                               raw_tree<T> &&incoming) const {
    accessor<raw_tree<T>> zipper(main);

    while (true) {
        for (; !comparator_.template vertical<wing>(*incoming, *zipper);
             zipper.template down<wing>()) {
            if (!zipper->template has_child<wing>()) {
                zipper->template replace<wing>(std::move(incoming));
                return raw_tree<T>(std::move(main));
            }
        }

        incoming.swap(zipper.node());

        for (; !comparator_.template vertical<!wing>(*incoming, *zipper);
             zipper.template down<!wing>()) {
            if (!zipper->template has_child<!wing>()) {
                zipper->template replace<!wing>(std::move(incoming));
                return raw_tree<T>(std::move(main));
            }
        }

        incoming.swap(zipper.node());
    }
}

template <typename T, typename C>
bool mutator<T, C>::equal(const T &first, const T &second) const {
    return comparator_.equal_left(first, second) &&
           comparator_.equal_tall(first, second);
}

template <typename T, typename C>
std::optional<raw_tree<T>> mutator<T, C>::clip(raw_tree<T> &node,
                                               const T &value) const {
    if (comparator_.template horizontal<side::right>(value, *node)) {
        return clip<side::right>(node, value);
    }
    return clip<side::left>(node, value);
}

template <typename T, typename C>
template <side wing>
std::optional<raw_tree<T>> mutator<T, C>::clip(raw_tree<T> &node,
                                               const T &value) const {
    accessor<raw_tree<T>> main(node);

    if (!main.template down<wing>()) { return std::nullopt; }
    for (; !comparator_.template horizontal<!wing>(value, *main);) {
        if (!main.template down<wing>()) { return std::nullopt; }
    }
    main.up();

    raw_tree<T> clipped(main->template detach<wing>());
    accessor<raw_tree<T>> seam(clipped);

    while (true) {
        for (seam.template down<!wing>();
             !comparator_.template horizontal<wing>(value, *seam);) {
            if (!seam.template down<!wing>()) { return clipped; }
        }
        seam.up();
        main->template replace<wing>(seam->template detach<!wing>());

        for (main.template down<wing>();
             !comparator_.template horizontal<!wing>(value, *main);) {
            if (!main.template down<wing>()) { return clipped; }
        }
        main.up();
        seam->template replace<!wing>(main->template detach<wing>());
    }
}
