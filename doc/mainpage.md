@mainpage Overview

Predicate tree is a library for constructing and navgating [Cartesian
Trees](https://en.wikipedia.org/wiki/Cartesian_tree).

It is a container library (similar to `std::multiset`). It contains elements of
a fixed type and guarantees *orderings* over contained elements to make it
faster to perform certain operations over.

# Notation

* `label` denotes programmatic labels (class, function, variable name) *not*
  introduced in this library.
* `detangled::accessor` denotes programmatic labels which are introduced in
  this library. These are clickable.

# Before Predicate Trees

Before attaching predicates to trees it's useful to understand a family of
classes used as foundations to build predicated trees. These are simple binary
trees which can be used for any general purpose and are not linked to any
predicates or resulting morphological behaviour.

These are described in some detail at [Basic Trees](@ref basic).

# Predicates

Predicates are binary predicates as described through C++ literature and
documentation; for example at
[BinaryPredicates](https://en.cppreference.com/w/cpp/named_req/BinaryPredicate).
When attached to a tree they can govern ordering of elements in them or the
morphology of subtrees or both.

# With Predicates

Trees attached with predicates are described in more detail at
[Predicated Trees](@ref predicated). There is primarily one asset
`detangled::predicated_tree` which is built on top of all the *basic trees*
elements.

Some useful predicates are deined in [Predicates](@ref predicates).
[Special cases](@ref special) arise from using certain predicates and from the
relation *between* the predicates.
