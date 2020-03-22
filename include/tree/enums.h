#ifndef TREE_ENUMS_H
#define TREE_ENUMS_H

namespace detangled {

/// traversal_order is an enum listing supported traversal orders.
///
/// For example we have `traversal_order::pre` (which is preorder traversal).
enum class traversal_order : uint8_t { pre, in, post };

/// Returns a `traversal_order` that complements `order`. Complementarity is
/// fully decided by `side` as well, but this function does the
/// `traversal_order` part of things.
///
/// `Complementary (order, wing) == (~order, !wing)`
///
/// - ~in == in
/// - ~pre == post
/// - ~post == pre
///
/// @see `operator!(const side wing)`
constexpr traversal_order operator~(const traversal_order order);

/// side enum lists the side (left or right) we refer to at code sites.
///
/// @note The values are specified here because they may be used in array
/// listing and indexing.
enum class side : uint8_t { left = 0, right = 1 };
constexpr side operator!(const side wing);

// There are a large number of wrapper functions of the following type
//     returnType functionName(side wing, args...);
//
// which have analogous
//     template <side wing>
//     returnType function(args...);
//
// templated functions. The definition of such wrapper functions is as seen for
// the SWITCH_ON_SIDE macro. Basically it involves looking at the wing argument
// and choosing to call a specific template version (side == left or right).
// This macro does this and makes is more reliable to define the wrapper
// functions with much lower probability of errors creeping in.
//
// TODO(ghochee): Try to template instantiate the entire definition or MACRO
// define the whole function instead of just the body.
#define SWITCH_ON_SIDE(function_name, ...)              \
    if (wing == side::left) {                           \
        return function_name<side::left>(__VA_ARGS__);  \
    } else {                                            \
        return function_name<side::right>(__VA_ARGS__); \
    }

}  // namespace detangled

#include "impl/enums.hh"

#endif  // TREE_ENUMS_H
