#include "tree/raw_tree.h"

template <typename T>
void build_bst(::detangled::raw_tree<T> &root, uint32_t levels,
               uint32_t min_value = 0);

#include "raw_tree.hh"
