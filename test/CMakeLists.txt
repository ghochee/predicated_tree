enable_testing()

add_executable(mutator_test mutator.cpp)
target_link_libraries(mutator_test PRIVATE tree)
add_test(NAME tree_mutator_test COMMAND mutator_test)

add_executable(predicated_tree_test predicated_tree.cpp)
target_link_libraries(predicated_tree_test PRIVATE tree)
add_test(NAME tree_predicated_tree_test COMMAND predicated_tree_test)

add_executable(raw_tree_test raw_tree.cpp)
target_link_libraries(raw_tree_test PRIVATE tree)
add_test(NAME tree_raw_tree_test COMMAND raw_tree_test)
