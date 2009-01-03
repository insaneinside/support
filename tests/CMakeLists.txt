link_libraries(support)
link_directories("${Support_BINARY_DIR}/src")

add_executable(cmlog-test cmlog-test.c)
add_executable(dllist-test dllist-test.c)
#add_executable(matrix-test matrix-test.cc)
#add_executable(meta-test meta-test.c)
