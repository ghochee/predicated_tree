add_executable(container_test container_test.cpp)
target_link_libraries(container_test PUBLIC tree)
add_test(NAME tree_container_test COMMAND container_test)
