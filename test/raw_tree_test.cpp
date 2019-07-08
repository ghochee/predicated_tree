#include "tree/raw_tree.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

void build_tree(raw_tree<uint32_t> &root, uint32_t levels);

TEST_CASE("raw tree objects can be initialized", "[integer_tree, raw_tree]") {
    raw_tree<uint32_t> t(10);

    SECTION("test values") {
        CHECK(*t == 10);
        CHECK(!t.has_parent());
    }

    SECTION("attach") {
        t.attach_left(raw_tree<uint32_t>(20));
        t.attach_right(raw_tree<uint32_t>(30));
        CHECK(!t.has_parent());
        REQUIRE(t.has_left());   CHECK(*t.left()  == 20);  CHECK(t.left().has_parent());
        REQUIRE(t.has_right());  CHECK(*t.right() == 30);  CHECK(t.right().has_parent());
    }

    SECTION("detach") {
        t.attach_left(raw_tree<uint32_t>(20));
        t.attach_right(raw_tree<uint32_t>(30));

        REQUIRE(t.has_left());   t.detach_left();   CHECK(!t.has_left());
        REQUIRE(t.has_right());  t.detach_right();  CHECK(!t.has_right());
        CHECK(*t == 10);

        t.attach_left(raw_tree<uint32_t>(35));
        REQUIRE(t.has_left());
        CHECK(*t.left() == 35);
        auto b = t.detach_left();
        CHECK(*b == 35);  CHECK(!b.has_parent());
    }

    SECTION("big tree") {
        build_tree(t, 2);
        CHECK(t.size() == (1 << (2 + 1)) - 1);

        raw_tree<uint32_t> a(1024);
        build_tree(a, 10);
        CHECK(a.size() == (1 << (10 + 1)) - 1);
    }
}

void build_tree(raw_tree<uint32_t> &root, uint32_t levels) {
    if (levels == 0) { return; }

    root.attach_left(raw_tree<uint32_t>(*root / 2));
    build_tree(root.left(), levels - 1);

    root.attach_right(raw_tree<uint32_t>(*root * 3 / 2));
    build_tree(root.right(), levels - 1);
}

