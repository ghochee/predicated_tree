#include <benchmark/benchmark.h>
#include <iostream>

#include "tree.h"

#include <random>

using namespace std;

template <typename T>
struct more_even {
    bool operator()(const T a, const T b) const {
        return (a ^ (a - 1)) > (b ^ (b - 1));
    }
};

static void Insert(benchmark::State &state) {
    mt19937 rng(0);
    uniform_int_distribution<std::mt19937::result_type> dist(100, 100'000);
    predicated_tree<uint32_t, comparator<uint32_t, more_even<uint32_t>,
                                         std::less<uint32_t>>> p;

        for (auto _ : state) {
        state.PauseTiming();
        raw_tree<uint32_t> t(1 << 29);
        accessor<raw_tree<uint32_t>> root(t);
        for (uint32_t i = 0; i < state.range(0); ++i) {
            p.insert(dist(rng));
        }
        state.ResumeTiming();

        for (uint32_t i = 0; i < 1 << 6; ++i) {
            p.insert(dist(rng));
        }
    }
}
BENCHMARK(Insert)->DenseRange(0, 1 << 15, 1024)->ThreadPerCpu();

static void Erase(benchmark::State &state) {
    mt19937 rng(0);
    uniform_int_distribution<std::mt19937::result_type> dist(100, 100'000);
    predicated_tree<uint32_t, comparator<uint32_t, more_even<uint32_t>,
                                         std::less<uint32_t>>> p;
    p.insert(1 << 29);

    for (auto _ : state) {
        state.PauseTiming();
        set<uint32_t> erasables;
        for (uint32_t i = 0; i < state.range(0); ++i) {
            uint32_t value = dist(rng);
            if (erasables.size() < 1 << 6) { erasables.insert(value); }
            p.insert(std::move(value));
        }
        state.ResumeTiming();

        for (auto it = erasables.begin(); it != erasables.end(); ++it) {
            p.erase(p.find(*it));
        }
    }
}
BENCHMARK(Erase)->DenseRange(0, 1 << 15, 1024)->ThreadPerCpu();

BENCHMARK_MAIN();
