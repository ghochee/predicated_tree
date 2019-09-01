#ifndef TREE_COMPARATOR_H
#define TREE_COMPARATOR_H

#include <functional>
#include "util/predicates.h"

namespace detangled {

/// Class template which tests if `T` is defined for `operator<`.
///
/// We need to be able to perform this test to determine if `std::less` can be
/// used as a default for `T`.
template <typename T>
class has_lt {
  private:
    /// Weakest template overload, only fires if all else fails.
    template <typename>
    static constexpr std::false_type check(...);

    /// Template overload which is successfully chosen if `*ptr < *ptr` is a
    /// valid expression.
    template <typename U>
    static constexpr auto check(U *ptr) ->
        typename std::is_same<decltype(*ptr < *ptr), bool>::type;

  public:
    static const bool value =
        decltype(check<T>(static_cast<T *>(nullptr)))::value;
};

/// Class template which is instantiated for creating comparator objects.
///
/// Instantiated classes of this template *define* the properties in
/// `detangled::predicated_tree` that should be upheld. Instantiated classes
/// and their objects serve the same purpose as `Comparator` does for
/// `std::set`.
///
/// Both predicates are:
/// * Binary predicates meeting the strict-weak-ordering requirements.
/// * Not necessarily related.
///
/// Both the predicates have to support:
///
/// ```
/// bool operator()(const T &first, const T &second) const;
/// ```
///
/// @tparam T is the type of the object which this comparator object will be
///     comparing.
/// @tparam H predicate which governs `Height` comparison operations in the
///     tree. `Height`, `H`, *tall*, *short* are nouns used in documentation to
///     refer to this property and outcomes.
///     Default: `indifferent`.
/// @tparam L predicate which goversn `Left` comparison in the tree. `Left`,
///     `L`, *left*, *right*, *side*, *wing* are nouns used in documentation and
///     code to describe notes related to it. This defaulted to `std::less<T>`
///     if `T` has `operator<` defined on it else it becomes `indifferent`.
template <class T, class H = indifferent<T>,
          class L = typename ::std::conditional<
              has_lt<T>::value, ::std::less<T>, indifferent<T>>::type>
class comparator {
  public:
    comparator(H height_comparator = H(), L left_comparator = L());

    /// Directly callable `tall()` and `left()` member *functions*.
    const H tall;
    const L left;

    /// Two values (a, b) are *equal* if the predicate P comparing them returns
    /// false both ways. i.e. P(a, b) == P(b, a) == false.
    bool equal_tall(const T &first, const T &second) const;
    bool equal_left(const T &first, const T &second) const;

    /// True iff equal_tall and equal_left.
    bool equal(const T &first, const T &second) const;
};

// A very tiny lightweight wrapper around a function reference.
template <class T, bool (*F)(const T &, const T &)>
struct wrapper {
    inline bool operator()(const T &first, const T &second) const {
        return F(first, second);
    }
};

// Function template for easier creation of comparator family objects.
template <class T, class H, class L>
auto make_comparator(const H h = H(), const L l = L()) {
    return comparator<T, H, L>(h, l);
}

template <class T, class H, bool (*F)(const T &, const T &)>
auto make_comparator(const H h = H()) {
    return comparator<T, H, wrapper<T, F>>(h);
}

template <class T, bool (*F)(const T &, const T &), class L>
auto make_comparator(const L l = L()) {
    return comparator<T, wrapper<T, F>, L>(wrapper<T, F>(), l);
}

template <class T, bool (*F1)(const T &, const T &),
          bool (*F2)(const T &, const T &)>
auto make_comparator() {
    return comparator<T, wrapper<T, F1>, wrapper<T, F2>>();
}

}  // namespace detangled

#include "impl/comparator.hh"

#endif  // TREE_COMPARATOR_H
