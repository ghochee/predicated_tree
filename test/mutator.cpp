#include "tree.h"
#include "predicated_tree/util/predicates.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <algorithm>
#include <random>
#include <utility>

using namespace detangled;

TEST_CASE("make_heap", "[make_heap]") {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(100,
                                                                  10000000);
    std::vector<uint32_t> elements;
    for (uint32_t i = 0; i < 10000; ++i) {
        elements.push_back(dist(rng));
    }

    std::less<uint32_t> l;
    std::sort(elements.begin(), elements.end(), l);

    raw_tree<uint32_t> t(elements[0]);
    accessor<raw_tree<uint32_t>> ptr(t);
    for (auto it = elements.begin() + 1; it != elements.end(); ++it) {
        ptr->template replace<side::right>(raw_tree<uint32_t>(*it));
        ptr.template down<side::right>();
    }
    auto c = make_comparator<uint32_t, more_even, std::less<uint32_t>>();
    mutator<uint32_t, decltype(c)> m(c);
    m.stable_make_heap(t);
    CHECK(std::is_sorted(t.inlbegin(), t.inlend(), std::less<uint32_t>()));

    accessor<const raw_tree<uint32_t>> a(t);
}
