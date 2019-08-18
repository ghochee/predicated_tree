#include <benchmark/benchmark.h>

#include "tree.h"

using namespace detangled;

static void NodeConstruct(benchmark::State& state) {
    for (auto _ : state)
        raw_tree<uint32_t> t(1002);
}
BENCHMARK(NodeConstruct);

static void NodeMoveConstruct(benchmark::State& state) {
    raw_tree<uint32_t> t(1002);
    for (auto _ : state)
        raw_tree<uint32_t> tcopy(std::move(t));
}
BENCHMARK(NodeMoveConstruct);

static void NodeMoveAssign(benchmark::State& state) {
    raw_tree<uint32_t> t(1002);
    raw_tree<uint32_t> t2(1002);
    for (auto _ : state)
        t2 = std::move(t);
}
BENCHMARK(NodeMoveAssign);

static void NodeReplace(benchmark::State& state) {
    raw_tree<uint32_t> t(1002);
    raw_tree<uint32_t> t2(1002);
    for (auto _ : state)
        t.replace<side::right>(std::move(t2));
}
BENCHMARK(NodeReplace);

static void NodeReplaceValue(benchmark::State& state) {
    raw_tree<uint32_t> t(1002);
    raw_tree<uint32_t> t2(1002);
    for (auto _ : state)
        t.replace(side::right, std::move(t2));
}
BENCHMARK(NodeReplaceValue);

static void NodeEmplace(benchmark::State& state) {
    raw_tree<uint32_t> t(1002);
    for (auto _ : state)
        t.emplace<side::right>(1003);
}
BENCHMARK(NodeEmplace);

static void NodeSwap(benchmark::State& state) {
    raw_tree<uint32_t> t1(1001);
    raw_tree<uint32_t> t2(1002);
    for (auto _ : state)
        std::swap(t1, t2);
        std::swap(t2, t1);
}
BENCHMARK(NodeSwap);

BENCHMARK_MAIN();
