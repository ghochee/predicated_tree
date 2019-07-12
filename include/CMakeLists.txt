add_library(tree INTERFACE)

target_include_directories(tree INTERFACE .)
target_compile_features(tree
    INTERFACE
        cxx_auto_type
        cxx_std_17)
