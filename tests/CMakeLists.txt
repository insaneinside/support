set(SPT_BUILD_STATIC 1)
link_libraries(${SUPPORT_LIBRARY})

if(SPT_ENABLE_MLOG_CONTEXT)
  add_executable(cmlog-test cmlog-test.c)
endif(SPT_ENABLE_MLOG_CONTEXT)

add_executable(dllist-test dllist-test.c)

#add_executable(matrix-test matrix-test.cc)
#add_executable(meta-test meta-test.c)
