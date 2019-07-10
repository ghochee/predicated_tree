#include "tree/raw_tree.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

using int_tree = raw_tree<uint32_t>;

void build_bst(int_tree &root, uint32_t levels, uint32_t min_value = 0);
void test_tree_invariants(int_tree &root);
void test_bst_invariants(int_tree &root);

TEST_CASE("raw tree objects can be initialized", "[integer_tree, raw_tree]") {
    int_tree t(10);

    SECTION("test values") {
        CHECK(*t == 10);
        CHECK(!t.has_parent());
    }

    SECTION("attach") {
        t.replace<side::left>(int_tree(20));
        t.replace<side::right>(int_tree(30));
        CHECK(!t.has_parent());
        REQUIRE(t.has_child<side::left>());
        CHECK((*t.child<side::left>() == 20 &&
               t.child<side::left>().has_parent()));
        REQUIRE(t.has_child<side::right>());
        CHECK((*t.child<side::right>() == 30 &&
               t.child<side::right>().has_parent()));
    }

    SECTION("detach") {
        t.replace<side::left>(int_tree(20));
        t.replace<side::right>(int_tree(30));

        REQUIRE(t.has_child<side::left>()); t.detach<side::left>();
        CHECK(!t.has_child<side::left>());

        REQUIRE(t.has_child<side::right>()); t.detach<side::right>();
        CHECK(!t.has_child<side::right>());

        CHECK(*t == 10);

        t.replace<side::left>(int_tree(35));
        REQUIRE(t.has_child<side::left>());
        CHECK(*t.child<side::left>() == 35);

        auto b = t.detach<side::left>();
        CHECK(*b == 35);  CHECK(!b.has_parent());
    }

    SECTION("iterator") {
        t.replace<side::left>(int_tree(20));
        t.replace<side::right>(int_tree(30));

        SECTION("inorder") {
            auto pos = t.begin();
            CHECK(*pos++ == 20);
            CHECK(*pos++ == 10);
            CHECK(*pos++ == 30);
            CHECK(pos == t.end());
        }

        SECTION("preorder") {
            CHECK(std::is_sorted(
                t.begin<traversal_order::pre>(),
                int_tree::iterator<traversal_order::pre>(t.end())));
        }
    }

    SECTION("preorder iterator") {
        t.replace<side::left>(int_tree(20));
        t.replace<side::right>(int_tree(30));
        auto pos = t.begin();
        CHECK(*pos++ == 20);
        CHECK(*pos++ == 10);
        CHECK(*pos++ == 30);
        CHECK(pos == t.end());
    }

    SECTION("big tree iterators") {
        *t = 16;
        build_bst(t, 3);
        CHECK(t.size() == (1 << (3 + 1)) - 1);
        test_tree_invariants(t);
        test_bst_invariants(t);

        *t = 256;
        build_bst(t, 7);
        CHECK(t.size() == (1 << (7 + 1)) - 1);
        test_tree_invariants(t);
        test_bst_invariants(t);

        *t = 1024;
        build_bst(t, 10);
        CHECK(t.size() == (1 << (10 + 1)) - 1);
        test_tree_invariants(t);
        test_bst_invariants(t);
    }
}

void build_bst(int_tree &root, uint32_t levels, uint32_t min_value) {
    if (levels == 0) { return; }

    root.replace<side::left>(int_tree(*root - (*root - min_value) / 2));
    build_bst(root.child<side::left>(), levels - 1, min_value);

    root.replace<side::right>(int_tree(*root + (*root - min_value) / 2));
    build_bst(root.child<side::right>(), levels - 1, *root + 1);
}

void test_tree_invariants(int_tree &root) {
    REQUIRE(!root.has_parent());
    std::vector<int_tree::value_type> elements;

    std::copy(root.prelbegin(), root.prelend(),
              std::inserter(elements, elements.end()));
    CHECK(std::equal(elements.rbegin(), elements.rend(),
                     root.postrbegin(), root.postrend()));

    elements.clear();
    std::copy(root.postlbegin(), root.postlend(),
              std::inserter(elements, elements.end()));
    CHECK(std::equal(elements.rbegin(), elements.rend(),
                     root.prerbegin(), root.prerend()));

    elements.clear();
    std::copy(root.inlbegin(), root.inlend(),
              std::inserter(elements, elements.end()));
    CHECK(std::equal(elements.rbegin(), elements.rend(),
                     root.inrbegin(), root.inrend()));

    elements.clear();
    auto r = root.detach<side::right>();
    std::copy(root.prelbegin(), root.prelend(),
              std::inserter(elements, elements.end()));
    std::copy(r.prelbegin(), r.prelend(),
              std::inserter(elements, elements.end()));
    root.replace<side::right>(std::move(r));
    CHECK(std::equal(elements.rbegin(), elements.rend(),
                     root.postrbegin(), root.postrend()));
}

void test_bst_invariants(int_tree &root) {
    REQUIRE(!root.has_parent());
    CHECK(std::is_sorted(root.inlbegin(), root.inlend()));
    CHECK(std::is_sorted(root.inrbegin(), root.inrend(), std::greater()));

    auto l = root.detach<side::left>();
    CHECK(std::is_sorted(root.inlbegin(), root.inlend()));
    CHECK(std::is_sorted(root.inrbegin(), root.inrend(), std::greater()));
    CHECK(std::is_sorted(l.inlbegin(), l.inlend()));
    CHECK(std::is_sorted(l.inrbegin(), l.inrend(), std::greater()));

    auto lr = l.detach<side::right>();
    CHECK(std::is_sorted(l.inlbegin(), l.inlend()));
    CHECK(std::is_sorted(l.inrbegin(), l.inrend(), std::greater()));
    CHECK(std::is_sorted(lr.inlbegin(), lr.inlend()));
    CHECK(std::is_sorted(lr.inrbegin(), lr.inrend(), std::greater()));

    root.replace<side::left>(std::move(lr));
    CHECK(std::is_sorted(root.inlbegin(), root.inlend()));
    CHECK(std::is_sorted(root.inrbegin(), root.inrend(), std::greater()));
    CHECK(std::is_sorted(l.inlbegin(), l.inlend()));
    CHECK(std::is_sorted(l.inrbegin(), l.inrend(), std::greater()));

    root.detach<side::right>();
    CHECK(std::is_sorted(root.inlbegin(), root.inlend()));
    CHECK(std::is_sorted(root.inrbegin(), root.inrend(), std::greater()));
    CHECK(std::is_sorted(l.inlbegin(), l.inlend()));
    CHECK(std::is_sorted(l.inrbegin(), l.inrend(), std::greater()));
}
