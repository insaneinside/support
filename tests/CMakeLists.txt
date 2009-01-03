add_definitions(-Wall -pedantic -Wno-variadic-macros --std=c99)
set(SPT_BUILD_STATIC 1)
link_libraries(${SUPPORT_LIBRARY})

add_executable(cmlog-test cmlog-test.c)
add_executable(dllist-test dllist-test.c)

#add_executable(matrix-test matrix-test.cc)
#add_executable(meta-test meta-test.c)
