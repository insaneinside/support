if(NOT SUPPORT_LIBRARY)
  message(FATAL_ERROR "${SUPPORT_LIBRARY} is not set!")
endif(NOT SUPPORT_LIBRARY)
link_libraries("${SUPPORT_LIBRARY}")

add_executable(timeutil-test timeutil-test.c)
target_link_libraries(timeutil-test m)

if(SPT_ENABLE_LOG_CONTEXT)
  add_executable(cmlog-test cmlog-test.c)
endif(SPT_ENABLE_LOG_CONTEXT)

add_executable(dllist-test dllist-test.c)

#add_executable(matrix-test matrix-test.cc)
#add_executable(meta-test meta-test.c)
