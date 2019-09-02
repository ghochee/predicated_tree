namespace detangled {

template <typename T, typename HIn, typename LIn, typename HOut, typename LOut>
predicated_tree<T, HOut, LOut> gardener<T, HIn, LIn, HOut, LOut>::craft(
    predicated_tree<T, HIn, LIn> &ptree, const HOut h, const LOut l) {
    auto pout = make_predicated_tree(h, l);
    // FIXME(ghochee): Inserting elements from ptree into the result before
    // returning should that *consume* the intree, i.e. move the values and
    // leave the tree empty.
    pout.insert(ptree.inlbegin(), ptree.inlend());
}

template <typename T, typename HIn, typename L, typename HOut>
class gardener<T, HIn, L, HOut, L> {
  public:
    static predicated_tree<T, HOut, L> craft(predicated_tree<T, HIn, L> &ptree,
                                             const HOut = HOut(),
                                             const L = L());
};

template <typename T, typename HIn, typename L, typename HOut>
predicated_tree<T, HOut, L> gardener<T, HIn, L, HOut, L>::craft(
    predicated_tree<T, HIn, L> &ptree, const HOut h, const L l) {
    raw_tree<T> tree = ptree.release();
    // Do a postorder traversal and re-heap the top element of the subtree.
    // Reheap involves using rotate which doesn't affect the inorder sequence
    // (preserves left-ness).
    for (auto it = tree.template begin<traversal_order::post, side::left>();
         it != tree.template end<traversal_order::post, side::left>(); ++it) {
        for (auto pos = it;;) {
            // Determine a side of rotation (left or right), dependending on
            // presence and h-relation with children and then rotate.
            std::optional<side> rotation_side;
            if (pos->template has_child<side::left>() &&
                h(*pos->template child<side::left>(), *pos)) {
                rotation_side.emplace(side::right);
            }

            if (pos->template has_child<side::right>() &&
                h(*pos->template child<side::right>(),
                  rotation_side.has_value() ? *pos->template child<side::left>()
                                            : *pos)) {
                rotation_side.emplace(side::left);
            }

            if (!rotation_side.has_value()) { break; }

            pos->rotate(*rotation_side);
            pos.down(*rotation_side);
        }
    }

    return predicated_tree<T, HOut, L>(std::move(tree), h, l);
}

}  // namespace detangled
