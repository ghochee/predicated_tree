#include <iostream>

#include "tree.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

#include "raw_tree.h"

using namespace detangled;
using namespace std;

using int_tree = raw_tree<uint32_t>;

TEST_CASE("height0", "[integer_tree, raw_tree]") {
    int_tree t(10);

    SECTION("accessors") {
        SECTION("value") {
            CHECK(*t == 10);
            *t = 20;
            CHECK(*t == 20);
        }

        SECTION("children") {
            CHECK(!t.has_parent());
            CHECK(!t.has_child<side::left>());
            CHECK(!t.has_child<side::right>());
        }

        SECTION("iterator") {
            CHECK(range_eq(t.inlbegin(), t.inlend(), {10}));
        }

        SECTION("size") {
            CHECK(t.size() == 1);
        }
    }

    SECTION("modifiers") {
        SECTION("replace") {
            SECTION("right") {
                t.replace<side::right>(int_tree(20));
                REQUIRE(t.has_child<side::right>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {10, 20}));
            }

            SECTION("left") {
                t.replace<side::left>(int_tree(5));
                REQUIRE(t.has_child<side::left>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10}));
            }

            SECTION("both") {
                t.replace<side::left>(int_tree(5));
                t.replace<side::right>(int_tree(20));
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 20}));
            }
        }

        SECTION("emplace") {
            SECTION("right") {
                t.emplace<side::right>(20);
                REQUIRE(t.has_child<side::right>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {10, 20}));
            }

            SECTION("left") {
                t.emplace<side::left>(5);
                REQUIRE(t.has_child<side::left>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10}));
            }

            SECTION("both") {
                t.emplace<side::left>(5);
                t.emplace<side::right>(20);
                REQUIRE(t.has_child<side::left>());
                REQUIRE(t.has_child<side::right>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 20}));
            }
        }
    }
}

TEST_CASE("height1", "[integer_tree, raw_tree]") {
    int_tree t(10);
    t.emplace<side::left>(5);
    t.emplace<side::right>(20);

    SECTION("accessors") {
        SECTION("value") {
            CHECK(*t == 10);
            CHECK(*(t.child<side::left>()) == 5);
            *(t.child<side::right>()) = 21;
            CHECK(*(t.child<side::right>()) == 21);
        }

        SECTION("children") {
            CHECK(!t.has_parent());
            CHECK(t.has_child<side::left>());
            CHECK(t.has_child<side::right>());
            CHECK(!t.child<side::left>().has_child<side::left>());
        }

        SECTION("iterator") {
            CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 20}));
        }

        SECTION("size") {
            CHECK(t.size() == 3);
        }
    }

    SECTION("modifiers") {
        SECTION("replace") {
            SECTION("right") {
                t.replace<side::right>(int_tree(30));
                REQUIRE(t.has_child<side::right>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 30}));
            }

            SECTION("left") {
                t.replace<side::left>(int_tree(7));
                REQUIRE(t.has_child<side::left>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {7, 10, 20}));
            }

            SECTION("both") {
                t.replace<side::left>(int_tree(8));
                t.replace<side::right>(int_tree(21));
                CHECK(range_eq(t.inlbegin(), t.inlend(), {8, 10, 21}));
            }
        }

        SECTION("emplace") {
            SECTION("right") {
                t.emplace<side::right>(30);
                REQUIRE(t.has_child<side::right>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 30}));
            }

            SECTION("left") {
                t.emplace<side::left>(7);
                REQUIRE(t.has_child<side::left>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {7, 10, 20}));
            }

            SECTION("both") {
                t.emplace<side::left>(8);
                t.emplace<side::right>(21);
                CHECK(range_eq(t.inlbegin(), t.inlend(), {8, 10, 21}));
            }
        }

        SECTION("reshape") {
            SECTION("left-left") {
                t.reshape<side::left, side::left>();
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 20}));
            }

            SECTION("left-right") {
                t.reshape<side::left, side::right>();
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 20}));
            }

            SECTION("right-left") {
                t.reshape<side::right, side::left>();
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 20}));
            }

            SECTION("right-right") {
                t.reshape<side::right, side::right>();
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 20}));
            }

            SECTION("all") {
                t.reshape<side::left, side::left>();
                t.reshape<side::right, side::right>();
                t.reshape<side::right, side::left>();
                t.reshape<side::left, side::left>();
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 20}));
            }
        }

        SECTION("detach") {
            SECTION("left") {
                auto child = t.detach<side::left>();
                CHECK(!t.has_child<side::left>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {10, 20}));
                CHECK(*child == 5);
            }

            SECTION("both") {
                auto child = t.detach<side::right>();
                CHECK(!t.has_child<side::right>());
                CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10}));
                CHECK(*child == 20);

                auto left_child = t.detach<side::left>();
                CHECK(t.size() == 1);
                CHECK(range_eq(t.inlbegin(), t.inlend(), {10}));
                CHECK(*left_child == 5);
            }
        }

        SECTION("move") {
            int_tree child(t.detach<side::left>());
            CHECK(!t.has_child<side::left>());
            CHECK(range_eq(t.inlbegin(), t.inlend(), {10, 20}));
            CHECK(*child == 5);
        }

        SECTION("swap") {
            int_tree t2(15);
            t2.emplace<side::left>(7);
            swap(t.child<side::right>(), t2.child<side::left>());
            CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10, 7}));
            CHECK(range_eq(t2.inlbegin(), t2.inlend(), {20, 15}));
        }

        SECTION("flip") {
            t.flip();
            CHECK(range_eq(t.inlbegin(), t.inlend(), {20, 10, 5}));
            t.detach<side::left>();
            t.flip();
            CHECK(range_eq(t.inlbegin(), t.inlend(), {5, 10}));
            t.detach<side::left>();
            CHECK(range_eq(t.inlbegin(), t.inlend(), {10}));
            t.flip();
            CHECK(range_eq(t.inlbegin(), t.inlend(), {10}));
        }
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

    SECTION("rotate and reshape") {
        t.rotate<side::right>();
        test_tree_invariants(t);
        CHECK(range_eq(t.inlbegin(), t.inlend(), inl_values));

        t.rotate<side::left>();
        CHECK(range_eq(t.inlbegin(), t.inlend(), inl_values));
        CHECK(range_eq(t.prelbegin(), t.prelend(), prel_values));

        t.rotate<side::left>();
        CHECK(range_eq(t.inlbegin(), t.inlend(), inl_values));

        t.reshape<side::right, side::left>();
        CHECK(range_eq(t.inlbegin(), t.inlend(), inl_values));

        t.reshape<side::right, side::left>();
        CHECK(range_eq(t.inlbegin(), t.inlend(), inl_values));

        t.reshape<side::left, side::right>();
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

    /*
    SECTION("swap") {
        std::swap(t.child<side::left>(),
                  t.child<side::right>().child<side::left>());
        std::copy(t.inlbegin(), t.inlend(),
                  std::ostream_iterator<uint32_t>(std::cout, " "));
        std::cout << "\n";
    }
    */
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
    CHECK(t.template has_child<side::right>());

    SECTION("reshape") {
        t.reshape<side::left, side::right>();
        test_bst_invariants(t);
    }

    SECTION("reshape") {
        t.reshape<side::right, side::left>();
        test_bst_invariants(t);
    }

    SECTION("reshape") {
        t.reshape<side::right, side::right>();
        test_bst_invariants(t);
    }
}
