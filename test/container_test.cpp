#include <iostream>

#include "tree/tree.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <algorithm>
#include <iterator>
#include <random>

using int_node = tree<int>::Node;
using namespace std;

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

TEST_CASE("tree", "[constructors, insert]") {
    std::mt19937 rng;
    SECTION("simple") {
        tree<uint32_t> t((std::less<uint32_t>()));
        std::generate_n(std::inserter(t, t.end()), 20, std::ref(rng));
        std::copy(t.begin(), t.end(),
                  std::ostream_iterator<uint32_t>(std::cout, " "));
        std::cout << "\n";
        std::copy(t.begin(), t.end(),
                  std::ostream_iterator<uint32_t>(std::cout, " "));
    }
}
