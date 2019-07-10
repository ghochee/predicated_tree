template <typename T>
void build_bst(raw_tree<T> &root, uint32_t levels, uint32_t min_value = 0);

template <typename T>
void test_tree_invariants(raw_tree<T> &root);

template <typename T>
void test_bst_invariants(raw_tree<T> &root);

#include "raw_tree_test.hh"
