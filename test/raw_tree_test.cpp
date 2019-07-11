#include <iostream>

#include "tree.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

#include "raw_tree_test.h"

using namespace std;

using int_tree = raw_tree<uint32_t>;

TEST_CASE("pointed single node", "[integer_tree, raw_tree]") {
    int_tree t(10);

    SECTION("accessors") {
        SECTION("values") {
            CHECK(*t == 10);
            CHECK(!t.has_parent());
        }

        SECTION("children") {
            CHECK(!t.has_child<side::left>());
            CHECK(!t.has_child<side::right>());
        }

        SECTION("iterators") {
            CHECK(range_eq(t.inlbegin(), t.inlend(), {10}));
        }
    }

    SECTION("modify") {
        *t = 20;
        CHECK(*t == 20);
    }
}

TEST_CASE("pointed multiple nodes", "[integer_tree, raw_tree]") {
    int_tree t(10);

    int_tree l(20);
    l.replace<side::left>(int_tree(5));
    l.replace<side::right>(int_tree(50));

    int_tree r(30);
    r.replace<side::left>(int_tree(35));
    r.replace<side::right>(int_tree(20));

    t.replace<side::left>(std::move(l));
    t.replace<side::right>(std::move(r));

    test_tree_invariants(t);
    std::initializer_list<int_tree::value_type> inl_values = {5,  20, 50, 10,
                                                              35, 30, 20};
    std::initializer_list<int_tree::value_type> prel_values = {10, 20, 5, 50,
                                                               30, 35, 20};
    CHECK(*t == 10);

    SECTION("children") {
        REQUIRE(t.has_child<side::left>());
        REQUIRE(t.has_child<side::right>());
    }

    SECTION("order") {
        SECTION("in") {
            CHECK(range_eq(t.inlbegin(), t.inlend(), inl_values));
        }
        SECTION("pre") {
            CHECK(range_eq(t.prelbegin(), t.prelend(), prel_values));
        }
    }

    SECTION("rotate") {
        t.rotate<side::right>();
        test_tree_invariants(t);
        CHECK(range_eq(t.inlbegin(), t.inlend(), inl_values));

        t.rotate<side::left>();
        CHECK(range_eq(t.inlbegin(), t.inlend(), inl_values));
        CHECK(range_eq(t.prelbegin(), t.prelend(), prel_values));

        t.rotate<side::left>();
        CHECK(range_eq(t.inlbegin(), t.inlend(), inl_values));
    }

    SECTION("detach") {
        REQUIRE(t.has_child<side::left>()); t.detach<side::left>();
        CHECK(!t.has_child<side::left>());

        REQUIRE(t.has_child<side::right>()); t.detach<side::right>();
        CHECK(!t.has_child<side::right>());

        t.replace<side::left>(int_tree(35));
        REQUIRE(t.has_child<side::left>());
        CHECK(*t.child<side::left>() == 35);

        auto b = t.detach<side::left>();
        CHECK(*b == 35);  CHECK(!b.has_parent());
    }
}

TEST_CASE("coarse string", "[string_tree, raw_tree]") {
    raw_tree<string> t("middle");
    CHECK(*t == "middle");

    t.replace<side::left>(raw_tree<string>("left"));
    t.child<side::left>().replace<side::left>(raw_tree<string>("far left"));
    t.child<side::left>().replace<side::right>(raw_tree<string>("left right"));

    t.replace<side::right>(raw_tree<string>("right"));
    t.child<side::right>().replace<side::left>(raw_tree<string>("more right"));
    t.child<side::right>().replace<side::right>(raw_tree<string>("rightmost"));

    CHECK(t.size() == 7);
    test_bst_invariants(t);
}

TEST_CASE("bst", "[integer_tree, raw_tree]") {
    int_tree t(10);
    build_bst(t, 3);
    CHECK(t.size() == (1 << (3 + 1)) - 1);
    test_bst_invariants(t);

    *t = 256;
    build_bst(t, 7);
    CHECK(t.size() == (1 << (7 + 1)) - 1);
    test_bst_invariants(t);

    *t = 1024;
    build_bst(t, 10);
    CHECK(t.size() == (1 << (10 + 1)) - 1);
    test_bst_invariants(t);
}
