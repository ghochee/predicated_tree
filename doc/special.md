@page special Special Cases

This page lists out certain special cases in predicate trees which arise as a
result of using certain predicates or from the relations between the
predicates. These special cases are *special* only in the sense of
understanding. Available operations and behaviour of the tree container objects
remains the same.

| Height              | Left             | Result             |
|---------------------|------------------|--------------------|
|  No                 |  Indifferent     | Max heap           |
|  Indifferent        |  No              | Binary search tree |
|  Yes                |  Yes             | Simple tree        |
|  Strict Total Order |  !H or H         | Multiset / List    |

[indifferent](@ref indifferent) predicates would return false for all
comparisons (i.e. not affect ordering in any way).

If viewed in the above sense, the dual predicate tree seems like a
subsumation of the properties of heaps, binary search trees and linked lists.
It is however a little more powerful in that heap modifications are effected
keeping in mind the binary search tree property and vice a versa.

Put another way, all insertions and deletions of elements in a predicated tree
would be done keeping *both* predicates intact. The special cases are because
either a predicate is too liberal (Indifferent) or it is determined by the
other predicate.
