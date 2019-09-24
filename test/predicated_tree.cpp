#include "tree.h"
#include "predicated_tree/algorithm.h"
#include "predicated_tree/heap_iterator.h"
#include "predicated_tree/util/predicates.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <algorithm>
#include <random>
#include <utility>
#include <vector>

using namespace detangled;
using namespace std;

// clang-format off
#define LIST_PREDICATES                                                    \
    (wrapper<uint32_t, more_even<uint32_t>>,                               \
     wrapper<uint32_t, more_even<uint32_t>>),                              \
    (wrapper<uint32_t, more_even<uint32_t>>, std::less<uint32_t>),         \
    (wrapper<uint32_t, more_even<uint32_t>>, indifferent<uint32_t>),       \
    (wrapper<uint32_t, more_even<uint32_t>>, stable_random<uint32_t>),     \
                                                                           \
    (std::less<uint32_t>, wrapper<uint32_t, more_even<uint32_t>>),         \
    (std::less<uint32_t>, std::less<uint32_t>),                            \
    (std::less<uint32_t>, indifferent<uint32_t>),                          \
    (std::less<uint32_t>, stable_random<uint32_t>),                        \
                                                                           \
    (indifferent<uint32_t>, wrapper<uint32_t, more_even<uint32_t>>),       \
    (indifferent<uint32_t>, std::less<uint32_t>),                          \
    (indifferent<uint32_t>, indifferent<uint32_t>),                        \
    (indifferent<uint32_t>, stable_random<uint32_t>),                      \
                                                                           \
    (stable_random<uint32_t>, wrapper<uint32_t, more_even<uint32_t>>),     \
    (stable_random<uint32_t>, std::less<uint32_t>),                        \
    (stable_random<uint32_t>, indifferent<uint32_t>),                      \
    (stable_random<uint32_t>, stable_random<uint32_t>)
// clang-format on

TEMPLATE_PRODUCT_TEST_CASE("insert", "[insert]", (::std::pair),
                           (LIST_PREDICATES)) {
    raw_tree<uint32_t> t(10);

    predicated_tree<uint32_t, typename TestType::first_type,
                    typename TestType::second_type>
        p;

    SECTION("single") {
        p.insert(5);
        CHECK(p->size() == 1);
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(100, 1000000);

    SECTION("small") {
        for (uint32_t i = 0; i < 100; ++i) {
            p.insert(dist(rng));
            CHECK(std::is_sorted(p->inlbegin(), p->inlend(), p.left));
        }
    }

    SECTION("medium") {
        for (uint32_t i = 0; i < 10000; ++i) { p.insert(dist(rng)); }
        CHECK(std::is_sorted(p->inlbegin(), p->inlend(), p.left));
    }

    vector<uint32_t> values;
    std::copy(heap_iterator<decltype(p)>(p), heap_iterator<decltype(p)>(),
              std::back_inserter(values));
    CHECK(std::is_sorted(values.begin(), values.end(), p.tall));
}

TEMPLATE_PRODUCT_TEST_CASE("convert", "[convert]", (::std::pair),
                           (LIST_PREDICATES)) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(100, 1000000);

    predicated_tree ptree{typename TestType::first_type(),
                          typename TestType::second_type()};
    for (uint32_t i = 0; i < 10000; ++i) { ptree.insert(dist(rng)); }
    CHECK(std::is_sorted(ptree->inlbegin(), ptree->inlend(),
                         typename TestType::second_type()));

    predicated_tree converted{std::move(ptree),
                              typename TestType::second_type(),
                              typename TestType::first_type()};
    CHECK(std::is_sorted(converted->inlbegin(), converted->inlend(),
                         typename TestType::first_type()));
    auto tree = converted.release();
    CHECK(is_heap(accessor<raw_tree<uint32_t>>(tree), converted.tall));
}
