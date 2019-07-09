#include "tree/raw_tree.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using int_tree = raw_tree<uint32_t>;

void build_bst(int_tree &root, uint32_t levels, uint32_t min_value = 0);

TEST_CASE("raw tree objects can be initialized", "[integer_tree, raw_tree]") {
    int_tree t(10);

    SECTION("test values") {
        CHECK(*t == 10);
        CHECK(!t.has_parent());
    }

    SECTION("attach") {
        t.attach_left(int_tree(20));
        t.attach_right(int_tree(30));
        CHECK(!t.has_parent());
        REQUIRE(t.has_left());
        CHECK((*t.left()  == 20 && t.left().has_parent()));
        REQUIRE(t.has_right());
        CHECK((*t.right() == 30 && t.right().has_parent()));
    }

    SECTION("detach") {
        t.attach_left(int_tree(20));
        t.attach_right(int_tree(30));

        REQUIRE(t.has_left());   t.detach_left();   CHECK(!t.has_left());
        REQUIRE(t.has_right());  t.detach_right();  CHECK(!t.has_right());
        CHECK(*t == 10);

        t.attach_left(int_tree(35));
        REQUIRE(t.has_left());
        CHECK(*t.left() == 35);
        auto b = t.detach_left();
        CHECK(*b == 35);  CHECK(!b.has_parent());
    }

    SECTION("iterator") {
        t.attach_left(int_tree(20));
        t.attach_right(int_tree(30));

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
        t.attach_left(int_tree(20));
        t.attach_right(int_tree(30));
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
        CHECK(std::is_sorted(t.begin(), t.end()));

        *t = 256;
        build_bst(t, 7);
        CHECK(t.size() == (1 << (7 + 1)) - 1);
        CHECK(std::is_sorted(t.begin(), t.end()));

        *t = 1024;
        build_bst(t, 10);
        CHECK(t.size() == (1 << (10 + 1)) - 1);
        CHECK(std::is_sorted(t.begin(), t.end()));
    }
}

void build_bst(int_tree &root, uint32_t levels, uint32_t min_value) {
    if (levels == 0) { return; }

    root.attach_left(int_tree(*root - (*root - min_value) / 2));
    build_bst(root.left(), levels - 1, min_value);

    root.attach_right(int_tree(*root + (*root - min_value) / 2));
    build_bst(root.right(), levels - 1, *root + 1);
}
