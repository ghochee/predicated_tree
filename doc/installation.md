@page installation Installation

This is a header only library. It is used by doing direct `include` in the
source / header files.

See [Tutorial](@ref tutorial) for usage after correct installation.

# Conan

**NOTE:** Conan package installation is done inside a subdirectory for proper
namespacing. All header includes have to be prefixed with `detangled/`.

Using [conan](https://conan.io) for setup. After setting up conan, the
following steps are required.

Setup the conan remote repository from which this header only library will be
pulled.
```console
$ conan remote add ghochee https://api.bintray.com/conan/ghochee/conan
$
```

To add dependency to the package
```bash
project_dir$ cat conanfile.txt
[requires]
predicated_tree/0.2.2@ghochee/alpha
project_dir$
```

To install the package
```console
project_dir$ mkdir .conan/
project_dir$ cd .conan
project_dir$ conan install .. -r ghochee
project_dir$
```

## CMake example

```cmake
include(${CMAKE_SOURCE_DIR}/.conan/conanbuildinfo.cmake)
conan_basic_setup()
```

# Git

```console
$ git clone https://github.com/ghochee/predicated_tree
```
Then add `<project-clone-path>/include` to the include paths during
compilation.

## CMake example
[Reference](https://cmake.org/cmake/help/latest/command/target_include_directories.html)
```cmake
target_include_directories(<target>
    PRIVATE <project-clone-path>/include)
```
