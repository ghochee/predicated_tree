#ifndef TREE_COMPARATOR_H
#define TREE_COMPARATOR_H

#include <functional>
#include "util/predicates.h"

/// Class template which tests if `T` is defined for `operator<`.
///
/// We need to be able to perform this test to determine if `::std::less` can be
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
/// `::predicated_tree` that should be upheld. Instantiated classes and their
/// objects serve the same purpose as `Comparator` does for `std::set`.
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
/// It is useful to have *unambiguity* when determining height and left. This is
/// because ambiguity would mean that lookup operations would have to traverse
/// multiple paths. As such we use the predicates in a special way (using side
/// enum) and sometimes using the other predicate when we have to break ties
/// (equal nodes). This extension is done minimally to ensure we have an
/// effiently operable tree. More details on this are available on the
/// `horizontal()` and `vertical()` methods.
///
/// @tparam T is the type of the object which this comparator object will be
///     comparing.
/// @tparam H predicate which governs `Height` comparison operations in the
///     tree. `Height`, `H`, *tall*, *short* are nouns used in documentation to
///     refer to this property and outcomes.
///     Default: `::indifferent`.
/// @tparam L predicate which goversn `Left` comparison in the tree. `Left`,
///     `L`, *left*, *right*, *side*, *wing* are nouns used in documentation and
///     code to describe notes related to it. This defaulted to `::std::less<T>`
///     if `T` has `operator<` defined on it else it becomes `::indifferent`.
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

    /// Should be read as, 'Is first vertically higher than second if it's to
    /// the wing-side'. So for example vertical<side::left>(a, b) == true would
    /// mean a should be taller than b when it's to the left.
    ///
    /// Identical to isTall_ except when first and second are equal(for height
    /// and direction), then returns true iff wing is right.
    template <side wing>
    bool vertical(const T &first, const T &second) const;
    /// If `wing == side::right` return `left(first, second)`
    /// else return `!left(second, first)`.
    ///
    /// Converts `L` from a SWO into a total ordering by treating the arguments
    /// as different. First value is assumed lefty so
    /// `comparator::horizontal<side::left>(v, v)` is always true.
    ///
    /// Equivalent formal statement:
    /// ```
    /// horizontal<!wing>(a, b) == !horizontal<wing>(a, b)
    /// ```
    ///
    /// Client code should treat the two arguments to the function as
    /// different.  The first argument should be the incoming value which is
    /// not already part of a tree or is the one which is being moved. The
    /// second value is treated as extant which is already part of the
    /// structure we are visiting (at the client code site). This is important
    /// to effect tree predicate guarantees. For example since
    /// `comparator::horizontal<side::left>(v, v)` is always true it would make
    /// sense to call this method only when there is a difference between
    /// `first` and `second`. To repeat, client should call this function as
    /// `comparator::horizontal<wing>(incoming, existing)`.
    template <side wing>
    bool horizontal(const T &first, const T &second) const;
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

#include "impl/comparator.hh"

#endif  // TREE_COMPARATOR_H
