#ifndef TREE_COMPARATOR_H
#define TREE_COMPARATOR_H

// Class template which is instantiated for creating comparator objects. The
// template arguments are:
// - T: is the type of the object which this comparator object will be
//      comparing.
// - H, L: (Height and Left) are SWO binary predicate functors used for
//      comparing.
//
// Objects of this class are used for implementing properties which have to be
// upheld when building a predicated tree. Their functional use for a
// predicated tree is analogous to that served by predicates supplied to
// std::set.
//
// Properties are specified by two predicates which are:
// - Binary predicates meeting the strict-weak-ordering requirements.
// - Not necessarily related.
//
// Both the predicates have to support:
//
//     bool operator()(const T &first, const T &second) const;
//
// To effect (or test) properties this wrapper object should be used instead of
// the underlying predicates. This is because some tweaks are made to the SWOs
// to enable full and efficient use in implementing the predicated tree.
//
// In the documentation the two predicates are described differently and each
// serves a different purpose. These are described below.
//
// Height Predicate
//
// The first predicate is called the 'Height' predicate in documentation and
// variable naming. 'Height', 'H', 'tall', 'short' are nouns used in
// documentation to refer to this property and outcomes. This predicate is the
// primary criterion for deciding the height of a node in a predicated tree.
//
// Left Predicate
//
// The second predicate describes the total ordering relation. 'Left', 'L',
// 'left', 'right', 'side', 'wing' are nouns used in documentation and code to
// describe notes related to it. This predicate is the primary criterion for
// deciding the left-ness of a node in a predicated tree.
//
// It is useful to have 'unambiguity' when determining height and left. This is
// because ambiguity would mean that lookup operations would have to traverse
// multiple paths. As such we use the predicates in a special way (using side
// enum) and sometimes using the other predicate when we have to break ties
// (equal nodes). This extension is done minimally to ensure we have an
// effiently operable tree. More details on this are available on the
// 'horizontal' and 'vertical' methods.
template <class T, class H, class L>
class comparator {
  public:
    comparator(H height_comparator = H(), L left_comparator = L());

    // Directly callable 'tall()' and 'left()' member 'functions'.
    const H tall;
    const L left;

    // Two values (a, b) are 'equal' if the predicate P comparing them returns
    // false both ways. i.e. P(a, b) == P(b, a) == false.
    bool equal_tall(const T &first, const T &second) const;
    bool equal_left(const T &first, const T &second) const;

    // True iff equal_tall and equal_left.
    bool equal(const T &first, const T &second) const;

    // Should be read as, 'Is first vertically higher than second if it's to
    // the wing-side'. So for example vertical<side::left>(a, b) == true would
    // mean a should be taller than b when it's to the left.
    //
    // Identical to isTall_ except when first and second are equal(for height
    // and direction), then returns true iff wing is right.
    template <side wing>
    bool vertical(const T &first, const T &second) const;
    // If wing == side::right left(first, second) else !left(second, first).
    //
    // Converts 'isLeft' from a SWO into a total ordering by treating the
    // arguments as different. First value is assumed lefty so
    // horizontal<side::left>(v, v) is always true.
    //
    // Equivalent formal statement:
    // horizontal<!wing>(a, b) = !horizontal<wing>(a, b)
    //
    // Client code should treat the two arguments to the function as different.
    // The first argument should be the incoming value which is not already
    // part of a tree or is the one which is being moved. The second value is
    // treated as extant which is already part of the structure we are visiting
    // (at the client code site). This is important to effect tree predicate
    // guarantees. For example since horizontal<side::left>(v, v) is always
    // true it would make sense to call this method only when there is a
    // difference between 'first' and 'second'. To repeat, client should call
    // this function as horizontal<wing>(incoming, existing).
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

#include "tree/comparator.hh"

#endif  // TREE_COMPARATOR_H
