#include "tree.h"
#include "predicated_tree/algorithm.h"
#include "predicated_tree/util/predicates.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <random>
#include <utility>

using namespace detangled;

TEST_CASE("LEqual", "[lequal]") {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(100,
                                                                  10000000);

    predicated_tree ptree{indifferent<uint32_t>()};
    for (uint32_t i = 0; i < 10000; ++i) { ptree.insert(dist(rng)); }

    auto converted = gardener<uint32_t, indifferent<uint32_t>,
                              std::less<uint32_t>, wrapper<uint32_t, more_even>,
                              std::less<uint32_t>>::craft(ptree);

    auto tree = converted.release();
    CHECK(is_heap(accessor<raw_tree<uint32_t>>(tree), converted.tall));
}
