add_executable(predicated_tree_bm predicated_tree.cpp)
target_link_libraries(predicated_tree_bm PRIVATE tree benchmark pthread)

add_executable(raw_tree_bm raw_tree.cpp)
target_link_libraries(raw_tree_bm PRIVATE tree benchmark pthread)
