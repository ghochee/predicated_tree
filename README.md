# Overview

Predicate tree is a library for constructing and navgating tree objects.

It is a container library (similar to `std::multiset`). It contains elements of
a fixed type and guarantees *orderings* over contained elements to make it
faster to perform certain operations over.

## Notation

* `label` denotes programmatic labels (class, function, variable name) *not*
  introduced in this library.
* `detangled::accessor` denotes programmatic labels which are introduced in
  this library. These are clickable.

## Before Predicates

Before attaching predicates to trees it's useful to understand the following
family of classes used as foundations to build predicated trees.

### Raw Tree

`raw_tree`s allows client code to create and manipulate templated tree
objects.

### Accessor

`detangled::accessor`s are pointers to nodes in a tree. These are used
throughout the codebase and allow client code to navigate through tree
elements. These should be thought of as equivalent to `iterator`s from STL
*especially* when used for specifying `pos` style arguments. A related class
family is `virtual_accessor`s which are pointers to nodes limited to a
subtree.

### Iterator

`detangled::iterator` classes are configured to perform *iteration*s on a
virtual tree of nodes in a specific order. This allows client code to
efficiently navigate through the container of nodes that they create. Iterators
are additionally configurable on the type of iterator (e.g. `left` sided
`preorder`) and also on the subtree on which to run the iteration.

## With Predicates

On top of standard trees we add the notion of *predicates*. This container
makes ordering guarantees under these predicates. This would be analogous to
giving a predicate to a `std::vector` to make it a `std::multiset` where the
`std::multiset` makes ordering guarantees under the predicate.

`predicated_tree`s effect this notion by allowing clients to build containers
which would maintain upto two predicates. The first predicate is linked to the
tree in a *heap tree*-like fashion and the second predicate is linked to the
tree in a *binary search tree*-like arrangement.
