#include "tree/raw_tree.h"

template <typename T>
void build_bst(raw_tree<T> &root, uint32_t levels, uint32_t min_value) {
    if (levels == 0) { return; }

    root.template replace<side::left>(
        raw_tree<T>(*root - (*root - min_value) / 2));
    build_bst(root.template child<side::left>(), levels - 1, min_value);

    root.template replace<side::right>(
        raw_tree<T>(*root + (*root - min_value) / 2));
    build_bst(root.template child<side::right>(), levels - 1, *root + 1);
}

template <typename F, typename L, typename I>
bool range_eq(F f, L l, const std::initializer_list<I> &i) {
    return std::equal(f, l, i.begin(), i.end());
}

#define test_tree_invariants(root)                                 \
    {                                                              \
        REQUIRE(!root.has_parent());                               \
        CHECK(std::equal(std::reverse_iterator(root.prelend()),    \
                         std::reverse_iterator(root.prelbegin()),  \
                         root.postrbegin(), root.postrend()));     \
        CHECK(std::equal(std::reverse_iterator(root.inlend()),     \
                         std::reverse_iterator(root.inlbegin()),   \
                         root.inrbegin(), root.inrend()));         \
        CHECK(std::equal(std::reverse_iterator(root.postlend()),   \
                         std::reverse_iterator(root.postlbegin()), \
                         root.prerbegin(), root.prerend()));       \
                                                                   \
        std::vector<typename decltype(root)::value_type> elements; \
        auto r = root.template detach<side::right>();              \
        std::copy(root.prelbegin(), root.prelend(),                \
                  std::inserter(elements, elements.end()));        \
        std::copy(r.prelbegin(), r.prelend(),                      \
                  std::inserter(elements, elements.end()));        \
        root.replace<side::right>(std::move(r));                   \
        CHECK(std::equal(elements.rbegin(), elements.rend(),       \
                         root.postrbegin(), root.postrend()));     \
    }

#define test_bst_invariants(root)                                              \
    {                                                                          \
        test_tree_invariants(root);                                            \
        REQUIRE(!root.has_parent());                                           \
        CHECK(std::is_sorted(root.inlbegin(), root.inlend()));                 \
        CHECK(std::is_sorted(root.inrbegin(), root.inrend(), std::greater())); \
                                                                               \
        auto l = root.template detach<side::left>();                           \
        CHECK(std::is_sorted(root.inlbegin(), root.inlend()));                 \
        CHECK(std::is_sorted(root.inrbegin(), root.inrend(), std::greater())); \
        CHECK(std::is_sorted(l.inlbegin(), l.inlend()));                       \
        CHECK(std::is_sorted(l.inrbegin(), l.inrend(), std::greater()));       \
                                                                               \
        auto lr = l.template detach<side::right>();                            \
        CHECK(std::is_sorted(l.inlbegin(), l.inlend()));                       \
        CHECK(std::is_sorted(l.inrbegin(), l.inrend(), std::greater()));       \
        CHECK(std::is_sorted(lr.inlbegin(), lr.inlend()));                     \
        CHECK(std::is_sorted(lr.inrbegin(), lr.inrend(), std::greater()));     \
                                                                               \
        root.template replace<side::left>(std::move(lr));                      \
        CHECK(std::is_sorted(root.inlbegin(), root.inlend()));                 \
        CHECK(std::is_sorted(root.inrbegin(), root.inrend(), std::greater())); \
        CHECK(std::is_sorted(l.inlbegin(), l.inlend()));                       \
        CHECK(std::is_sorted(l.inrbegin(), l.inrend(), std::greater()));       \
                                                                               \
        root.template detach<side::right>();                                   \
        CHECK(std::is_sorted(root.inlbegin(), root.inlend()));                 \
        CHECK(std::is_sorted(root.inrbegin(), root.inrend(), std::greater())); \
        CHECK(std::is_sorted(l.inlbegin(), l.inlend()));                       \
        CHECK(std::is_sorted(l.inrbegin(), l.inrend(), std::greater()));       \
    }
