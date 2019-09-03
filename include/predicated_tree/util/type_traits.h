#ifndef PREDICATED_TREE_UTIL_TYPE_TRAITS_H
#define PREDICATED_TREE_UTIL_TYPE_TRAITS_H

namespace detangled {

/// This file contains utility functions for easily building the usable
/// scaffolding of `predicated_tree` and it's ancillary asset classes.

/// Class template which tests if `T` is defined for `operator<`.
///
/// We need to be able to perform this test to determine if `std::less` can be
/// used as a default for `T`.
template <typename T>
class has_lt {
  private:
    /// Weakest template overload, only fires if all else fails.
    template <typename>
    static constexpr ::std::false_type check(...);

    /// Template overload which is successfully chosen if `*ptr < *ptr` is a
    /// valid expression.
    template <typename U>
    static constexpr auto check(U *ptr) ->
        typename ::std::is_same<decltype(*ptr < *ptr), bool>::type;

  public:
    static const bool value =
        decltype(check<T>(static_cast<T *>(nullptr)))::value;
};

/// Class template which tests if `P` is a valid BinaryPredicate which can work
/// with arguments of type `T`.
///
/// We need this test to getter better compile time failure outputs.
template <typename T, typename P>
class valid_predicate {
  private:
    /// Weakest template overload, only fires if all else fails.
    template <typename>
    static constexpr ::std::false_type check(...);

    /// Template overload which is successfully chosen if `P(T, T)` is a valid
    /// expression.
    template <typename U>
    static constexpr auto check(U *ptr) -> typename ::std::is_convertible<
        decltype((::std::declval<P>())(::std::declval<U>(),
                                       ::std::declval<U>())),
        bool>::type;

  public:
    static const bool value =
        decltype(check<T>(static_cast<T *>(nullptr)))::value;
};

}  // namespace detangled

#endif  // PREDICATED_TREE_UTIL_TYPE_TRAITS_H
