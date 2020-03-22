#include "predicated_tree/algorithm.h"
#include "predicated_tree/heap_iterator.h"
#include "predicated_tree/predicated_tree.h"
#include "predicated_tree/util/predicates.h"
#include "tree/accessor.h"

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
    std::copy(heap_iterator(p), heap_iterator_end(p),
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

TEST_CASE("indifferent_heap_rotate", "[indifferent_heap, rotate]") {
    predicated_tree ptree{indifferent<uint32_t>()};

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(100, 1000000);
    for (uint32_t i = 0; i < 100000; ++i) { ptree.insert(dist(rng)); }

    CHECK(std::is_sorted(ptree->inlbegin(), ptree->inlend(), ptree.left));
    auto it = ptree->prelbegin();
    for (uint32_t num_rotations = 0;
         it != ptree->prelend() && num_rotations < 5; ++it, ++num_rotations) {
        ptree.rotate(it, (num_rotations % 3) ? side::left : side::right);
    }
    CHECK(std::is_sorted(ptree->inlbegin(), ptree->inlend(), ptree.left));
}
