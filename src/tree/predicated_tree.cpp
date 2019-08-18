#include "tree.h"
#include "tree/util/format.h"
#include "predicated_tree/util/predicates.h"

#include <iostream>
#include <random>

using namespace std;

int main(int, char **) {
    mt19937 rng(0);
    uniform_int_distribution<std::mt19937::result_type> dist(1, 100'000'000);

    auto c = make_comparator<uint32_t, more_even, std::less<uint32_t>>();
    predicated_tree<uint32_t, decltype(c)> p(c);

    vector<uint32_t> inserted_values;
    for (uint32_t i = 0; i < 100; ++i) {
        uint32_t value = dist(rng);
        inserted_values.push_back(value);
        p.insert(std::move(value));
    }
    cout << "Built a tree of size: " << p->size() << endl;
    cout << p.node() << endl;

    return 0;
}