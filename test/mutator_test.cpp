#include "tree.h"
#include "tree/util/predicates.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <algorithm>
#include <random>
#include <utility>

template <typename F, typename L, typename I>
bool range_eq(F f, L l, const std::initializer_list<I> &i) {
    return std::equal(f, l, i.begin(), i.end());
}

template <typename T>
struct more_even {
    bool operator()(const T &a, const T &b) const {
        return (a ^ (a - 1)) > (b ^ (b - 1));
    }
};

template <typename T>
static bool more_even_f(const T &a, const T &b) {
    return (a ^ (a - 1)) > (b ^ (b - 1));
}

#define LIST_PREDICATES                                              \
    (std::pair<more_even<uint32_t>, more_even<uint32_t>>), \
    (std::pair<more_even<uint32_t>, std::less<uint32_t>>), \
    (std::pair<more_even<uint32_t>, indifferent<uint32_t>>), \
    (std::pair<more_even<uint32_t>, stable_random<uint32_t>>), \
 \
    (std::pair<std::less<uint32_t>, more_even<uint32_t>>), \
    (std::pair<std::less<uint32_t>, std::less<uint32_t>>), \
    (std::pair<std::less<uint32_t>, indifferent<uint32_t>>), \
    (std::pair<std::less<uint32_t>, stable_random<uint32_t>>), \
 \
    (std::pair<indifferent<uint32_t>, more_even<uint32_t>>), \
    (std::pair<indifferent<uint32_t>, std::less<uint32_t>>), \
    (std::pair<indifferent<uint32_t>, indifferent<uint32_t>>), \
    (std::pair<indifferent<uint32_t>, stable_random<uint32_t>>), \
 \
    (std::pair<stable_random<uint32_t>, more_even<uint32_t>>), \
    (std::pair<stable_random<uint32_t>, std::less<uint32_t>>), \
    (std::pair<stable_random<uint32_t>, indifferent<uint32_t>>), \
    (std::pair<stable_random<uint32_t>, stable_random<uint32_t>>)

TEMPLATE_TEST_CASE("insert", "[insert]", LIST_PREDICATES) {
    raw_tree<uint32_t> t(10);

    auto c = make_comparator<uint32_t, typename TestType::first_type,
                             typename TestType::first_type>();
    predicated_tree<uint32_t, decltype(c)> p(c);

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
            CHECK(std::is_sorted(p->inlbegin(), p->inlend(), c.left));
        }
    }

    SECTION("medium") {
        for (uint32_t i = 0; i < 10000; ++i) {
            p.insert(dist(rng));
        }
        CHECK(std::is_sorted(p->inlbegin(), p->inlend(), c.left));
    }
}

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
    auto c = make_comparator<uint32_t, more_even_f, std::less<uint32_t>>();
    mutator<uint32_t, decltype(c)> m(c);
    m.stable_make_heap(t);
    CHECK(std::is_sorted(t.inlbegin(), t.inlend(), std::less<uint32_t>()));

    accessor<const raw_tree<uint32_t>> a(t);
}
