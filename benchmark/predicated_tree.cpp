#include <benchmark/benchmark.h>
#include <iostream>

#include "predicated_tree/predicated_tree.h"
#include "predicated_tree/util/predicates.h"

#include <random>

using namespace std;
using namespace detangled;

static void Find(benchmark::State &state) {
    std::random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<std::mt19937::result_type> dist(100, 100'000'000);
    predicated_tree p{wrap(more_even<uint32_t>)};
    set<uint32_t> inserted;
    for (uint32_t i = 0; i < state.range(0); ++i) {
        uint32_t value = dist(rng);
        inserted.insert(value);
        p.insert(uint32_t(value));
    }

    for (auto _ : state) {
        state.PauseTiming();
        for (auto it = inserted.begin(); it != inserted.end(); ++it) {
            state.ResumeTiming();
            benchmark::DoNotOptimize(p.find(*it));
            state.PauseTiming();
        }
        state.ResumeTiming();
    }
}
BENCHMARK(Find)->Range(1, 1 << 18)->ThreadPerCpu()->Iterations(20);

static void Insert(benchmark::State &state) {
    std::random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<std::mt19937::result_type> dist(100, 100'000);

    for (auto _ : state) {
        predicated_tree p{wrap(more_even<uint32_t>)};
        for (uint32_t i = 0; i < state.range(0); ++i) { p.insert(dist(rng)); }
    }
}
BENCHMARK(Insert)->Range(1, 1 << 18)->ThreadPerCpu()->Iterations(20);

static void Erase(benchmark::State &state) {
    std::random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<std::mt19937::result_type> dist(100, 100'000);
    predicated_tree p{wrap(more_even<uint32_t>)};

    for (auto _ : state) {
        state.PauseTiming();
        set<uint32_t> inserted;
        for (uint32_t i = 0; i < state.range(0); ++i) {
            uint32_t value = dist(rng);
            inserted.insert(value);
            p.insert(std::move(value));
        }

        state.ResumeTiming();
        for (auto it = inserted.begin(); it != inserted.end(); ++it) {
            p.erase(p.find(*it));
        }
    }
}
BENCHMARK(Erase)->Range(1, 1 << 18)->ThreadPerCpu()->Iterations(20);

BENCHMARK_MAIN();
