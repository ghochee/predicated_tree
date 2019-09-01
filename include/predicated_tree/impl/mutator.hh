#include <optional>

namespace detangled {

template <typename T, typename C>
mutator<T, C>::mutator(const C comparator) : comparator_(comparator) {}

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

}  // namespace detangled
