add_executable(container_test container_test.cpp)
target_link_libraries(container_test PUBLIC tree)
add_test(NAME tree_container_test COMMAND container_test)

add_executable(raw_tree_test raw_tree_test.cpp)
target_link_libraries(raw_tree_test PUBLIC tree)
add_test(NAME tree_raw_tree_test COMMAND raw_tree_test)
