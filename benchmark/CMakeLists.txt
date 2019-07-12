add_executable(predicated_tree predicated_tree.cpp)
target_link_libraries(predicated_tree PRIVATE tree benchmark pthread)

add_executable(raw_tree raw_tree.cpp)
target_link_libraries(raw_tree PRIVATE tree benchmark pthread)
