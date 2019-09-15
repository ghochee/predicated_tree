@page installation Installation

This is a header only library. It is used by doing direct `include` in the
source / header files.

# Git Clone

```sh
git clone https://github.com/ghochee/predicated_tree
```
Then add `<project-clone-path>/include` to the include paths during
compilation.

## CMake example
[Reference](https://cmake.org/cmake/help/latest/command/target_include_directories.html)
```cmake
target_include_directories(<target>
    PRIVATE <project-clone-path>/include)
```

See [Tutorial](@ref tutorial) for usage after correct installation.
