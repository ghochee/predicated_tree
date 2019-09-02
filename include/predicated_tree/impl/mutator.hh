#include <optional>

namespace detangled {

template <typename T, typename H, typename L>
mutator<T, H, L>::mutator(const H h, const L l) : tall(h), left(l) {}

template <typename T, typename H, typename L>
accessor<raw_tree<T>> mutator<T, H, L>::sift_down(raw_tree<T> &root) {
    for (auto pos = accessor<raw_tree<T>>(root);;) {
        std::optional<side> rotation_side;
        if (pos->template has_child<side::left>() &&
            tall(*pos->template child<side::left>(), *pos)) {
            *rotation_side = side::right;
        }

        if (pos->template has_child<side::right>() &&
            tall(*pos->template child<side::right>(),
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

template <typename T, typename H, typename L>
void mutator<T, H, L>::stable_make_heap(raw_tree<T> &tree) {
    for (auto it = tree.template begin<traversal_order::post, side::left>();
         it != tree.template end<traversal_order::post, side::left>(); ++it) {
        this->sift_down(it.node());
    }
}

template <typename T, typename H, typename L>
void mutator<T, H, L>::stable_sort(raw_tree<T> &tree) {
    for (auto it = tree.template begin<traversal_order::post, side::left>();
         it != tree.template end<traversal_order::post, side::left>(); ++it) {}
}

}  // namespace detangled
