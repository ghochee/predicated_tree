#include "tree.h"
#include "predicated_tree/util/predicates.h"
#include "predicated_tree/heap_iterator.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <algorithm>
#include <random>
#include <utility>
#include <vector>

using namespace detangled;
using namespace std;

// clang-format: off
#define LIST_PREDICATES                                                                          \
    (std::pair<wrapper<uint32_t, more_even<uint32_t>>, wrapper<uint32_t, more_even<uint32_t>>>), \
    (std::pair<wrapper<uint32_t, more_even<uint32_t>>, std::less<uint32_t>>),                    \
    (std::pair<wrapper<uint32_t, more_even<uint32_t>>, indifferent<uint32_t>>),                  \
    (std::pair<wrapper<uint32_t, more_even<uint32_t>>, stable_random<uint32_t>>),                \
                                                                                                 \
    (std::pair<std::less<uint32_t>, wrapper<uint32_t, more_even<uint32_t>>>),                    \
    (std::pair<std::less<uint32_t>, std::less<uint32_t>>),                                       \
    (std::pair<std::less<uint32_t>, indifferent<uint32_t>>),                                     \
    (std::pair<std::less<uint32_t>, stable_random<uint32_t>>),                                   \
                                                                                                 \
    (std::pair<indifferent<uint32_t>, wrapper<uint32_t, more_even<uint32_t>>>),                  \
    (std::pair<indifferent<uint32_t>, std::less<uint32_t>>),                                     \
    (std::pair<indifferent<uint32_t>, indifferent<uint32_t>>),                                   \
    (std::pair<indifferent<uint32_t>, stable_random<uint32_t>>),                                 \
                                                                                                 \
    (std::pair<stable_random<uint32_t>, wrapper<uint32_t, more_even<uint32_t>>>),                \
    (std::pair<stable_random<uint32_t>, std::less<uint32_t>>),                                   \
    (std::pair<stable_random<uint32_t>, indifferent<uint32_t>>),                                 \
    (std::pair<stable_random<uint32_t>, stable_random<uint32_t>>)
// clang-format: on

TEMPLATE_TEST_CASE("insert", "[insert]", LIST_PREDICATES) {
    raw_tree<uint32_t> t(10);

    predicated_tree<uint32_t, typename TestType::first_type,
                    typename TestType::first_type>
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
        for (uint32_t i = 0; i < 10000; ++i) {
            p.insert(dist(rng));
        }
        CHECK(std::is_sorted(p->inlbegin(), p->inlend(), p.left));
    }

    vector<uint32_t> values;
    std::copy(heap_iterator<decltype(p)>(p), heap_iterator<decltype(p)>(),
              std::back_inserter(values));
    CHECK(std::is_sorted(values.begin(), values.end(), p.tall));
}
