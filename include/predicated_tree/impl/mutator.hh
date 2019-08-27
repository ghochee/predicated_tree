#include <optional>

namespace detangled {

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
        pos && comparator_.equal(*pos, value)) {
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

    if (pos == accessor<ContainerType>(tree)) {
        // If we are at 'root' of search tree (not necessarily physical tree)
        // then we return 'end' accessor.
        return accessor<ContainerType>();
    }

    pos.up();
    return pos;
}

template <typename T, typename C>
template <class ContainerType>
accessor<ContainerType> mutator<T, C>::lower_bound(ContainerType &tree,
                                                   const T &value) const {
    auto pos = upper_bound(tree, value);
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
accessor<raw_tree<T>> mutator<T, C>::insert(T &&value, raw_tree<T> &node,
                                            accessor<const raw_tree<T>> hint) {
    // NOTE: We can cast away the const from 'hint' because we have been given
    // a reference to 'node'; 'hint' points to a node in the tree accessible
    // from 'node'.
    accessor<raw_tree<T>> &pos =
        *reinterpret_cast<accessor<raw_tree<T>> *>(&hint);

    if (!pos) { pos = accessor<raw_tree<T>>(node); }
    for (; pos->has_parent() && comparator_.tall(value, *pos); pos.up()) {}

    // FIXME(ghochee): Insert called with non-root node might mess up the tree
    // but maybe we are OK with that. This test can be deferred to if the node
    // is getting inserted at a *periphery* i.e. at root or root-equal node. Or
    // if the result of clipping is an empty tree.
    if (!in_subtree(pos.node(), value)) { pos = accessor<raw_tree<T>>(node); }

    if (auto vert_pos = upper_bound(pos.node(), value); vert_pos) {
        pos = vert_pos;
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
        } else {
            pos->template replace<side::left>(std::move(node));
        }
    }

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

    accessor<raw_tree<T>> sifted_position = this->sift_out(pos.node());
    bool has_left = sifted_position->template has_child<side::left>(),
         has_right = sifted_position->template has_child<side::right>();
    if (!has_left && !has_right) {
        if (sifted_position->template is_side<side::left>()) {
            sifted_position->parent().template detach<side::left>();
        } else {
            sifted_position->parent().template detach<side::right>();
        }
        return;
    }

    auto child_side = has_left ? side::left : side::right;
    auto &child = sifted_position->child(child_side);
    *sifted_position = std::move(*child);
    if (child.has_child(!child_side)) {
        sifted_position->replace(!child_side, child.detach(!child_side));
    }
    if (child.has_child(child_side)) {
        sifted_position->replace(child_side, child.detach(child_side));
    } else {
        sifted_position->detach(child_side);
    }
}

template <typename T, typename C>
accessor<raw_tree<T>> mutator<T, C>::sift_down(raw_tree<T> &root) {
    for (auto pos = accessor<raw_tree<T>>(root);;) {
        std::optional<side> rotation_side;
        if (pos->template has_child<side::left>() &&
            comparator_.tall(*pos->template child<side::left>(), *pos)) {
            *rotation_side = side::right;
        }

        if (pos->template has_child<side::right>() &&
            comparator_.tall(*pos->template child<side::right>(),
                             rotation_side.has_value()
                                 ? *pos->template child<side::left>()
                                 : *pos)) {
            *rotation_side = side::left;
        }

        if (!rotation_side.has_value()) { return pos; }

        pos->rotate(*rotation_side);
        pos.down(*rotation_side);
    }
}

template <typename T, typename C>
accessor<raw_tree<T>> mutator<T, C>::sift_out(raw_tree<T> &root) {
    for (auto pos = accessor<raw_tree<T>>(root);;) {
        if (!(pos->template has_child<side::left>() &&
              pos->template has_child<side::right>())) {
            return pos;
        }

        if (comparator_.tall(*pos->template child<side::left>(),
                             *pos->template child<side::right>())) {
            pos->template rotate<side::right>();
            pos.template down<side::right>();
        } else {
            pos->template rotate<side::left>();
            pos.template down<side::left>();
        }
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
        this->sift_down(it.node());
    }
}

template <typename T, typename C>
void mutator<T, C>::stable_sort(raw_tree<T> &tree) {
    for (auto it = tree.template begin<traversal_order::post, side::left>();
         it != tree.template end<traversal_order::post, side::left>(); ++it) {}
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

}  // namespace detangled
