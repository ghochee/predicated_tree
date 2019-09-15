@page tutorial Tutorial

After correct [Installation](@ref installation) you can use the headers in the
source file. Client code has to be compiled with `--std=c++17`

```cpp

#incluee "tree.h"

raw_tree tree((uint32_t)30);
accessor<raw_tree<uint32_t>> pos(tree);

for (...) {
  uint32_t value = getValue();
  side wing = determineSideToInsert(value);
  ...
  pos->emplace(wing, value);
  pos->rotate(!wing);
  pos.down(!wing);
  ...
}

...

for (auto it = tree.inlbegin(); it != tree.inlend(); ++it) {
  cout << *it;
}

...

```

See `test/predicated_tree.cpp` for example on how to build and use
`predicated_tree`.
