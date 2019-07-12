#ifndef TREE_UTIL_PREDICATES_H
#define TREE_UTIL_PREDICATES_H

// Some simple useful predicates which can be used with predicated_tree.

// Indifferent always returns false.
template <typename T>
struct indifferent {
    bool operator()(const T &, const T &) const { return false; }
};

// Returns a random ordering but one which follows SWO principles.
template <typename T>
struct stable_random {
    bool operator()(const T &, const T &) const;

  private:
    static std::hash<T> hash_computer_;
    static size_t seed_;
};

#include "tree/util/predicates.hh"

#endif  // TREE_UTIL_PREDICATES_H
