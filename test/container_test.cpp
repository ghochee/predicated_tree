#include "tree/tree.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace std;

using int_node = tree<int>::Node;

TEST_CASE("constructors", "[constructors]") {
    SECTION("single") { unique_ptr<int_node> n(make_unique<int_node>(5)); }
}

TEST_CASE("inserts", "[insert]") {
    SECTION("single") {
        unique_ptr<int_node> n(make_unique<int_node>(5));
        n = int_node::merge(std::move(n), make_unique<int_node>(2));
    }

    SECTION("multiple") {
        unique_ptr<int_node> n(make_unique<int_node>(5));
        n = int_node::merge(std::move(n), make_unique<int_node>(10));
        n = int_node::merge(std::move(n), make_unique<int_node>(14));
    }
}

TEST_CASE("comparators", "[insert, constructor]") {
    SECTION("simple") {
        unique_ptr<int_node> n(make_unique<int_node>(5));
        n = int_node::merge(std::move(n), make_unique<int_node>(10),
                            std::less<int>());
        n = int_node::merge(std::move(n), make_unique<int_node>(15),
                            std::greater<int>());
    }
}
