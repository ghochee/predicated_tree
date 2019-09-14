#ifndef PREDICATED_TREE_UTIL_PREDICATES_H
#define PREDICATED_TREE_UTIL_PREDICATES_H

namespace detangled {

/// @addtogroup predicates Predicates
///
/// Contains some simple predicates which are used either in the library or in
/// the test / example code.
/// @{

/// Indifferent always returns false.
template <typename T>
struct indifferent {
    bool operator()(const T &, const T &) const { return false; }
};

/// Returns a random ordering but one which follows SWO principles.
template <typename T>
struct stable_random {
    bool operator()(const T &, const T &) const;

  private:
    static std::hash<T> hash_computer_;
    static size_t seed_;
};

/// Returns `true` iff `a` is divisible by a greater power of 2 than `b`.
template <typename T>
bool more_even(const T &a, const T &b);

/// @}

}  // namespace detangled

#include "impl/predicates.hh"

#endif  // PREDICATED_TREE_UTIL_PREDICATES_H
