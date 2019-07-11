#include <iostream>

#include "tree/raw_tree.h"

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
    t.replace<side::left>(int_tree(20));
    t.replace<side::right>(int_tree(30));
    test_tree_invariants(t);

    CHECK(*t == 10);

    SECTION("children") {
        REQUIRE(t.has_child<side::left>());
        REQUIRE(t.has_child<side::right>());
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

    SECTION("order") {
        SECTION("in") {
            CHECK(range_eq(t.inlbegin(), t.inlend(), {20, 10, 30}));
        }
        SECTION("pre") {
            CHECK(range_eq(t.prelbegin(), t.prelend(), {10, 20, 30}));
        }
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
