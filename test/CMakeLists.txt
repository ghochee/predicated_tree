add_executable(container_test container_test.cpp)
target_link_libraries(container_test PRIVATE tree)
add_test(NAME tree_container_test COMMAND container_test)

add_executable(mutator_test mutator_test.cpp)
target_link_libraries(mutator_test PRIVATE tree)
add_test(NAME tree_mutator_test COMMAND mutator_test)

add_executable(predicated_tree_bm_test predicated_tree_bm_test.cpp)
target_link_libraries(predicated_tree_bm_test PRIVATE tree benchmark pthread)
add_test(NAME tree_predicated_tree_bm_test COMMAND predicated_tree_bm_test)

add_executable(raw_tree_test raw_tree_test.cpp)
target_link_libraries(raw_tree_test PRIVATE tree)
add_test(NAME tree_raw_tree_test COMMAND raw_tree_test)
