if(NOT SUPPORT_LIBRARY)
  message(FATAL_ERROR "${SUPPORT_LIBRARY} is not set!")
else(NOT SUPPORT_LIBRARY)
  message("Support library: ${SUPPORT_LIBRARY}")
endif(NOT SUPPORT_LIBRARY)
link_libraries("${SUPPORT_LIBRARY}")

if(SPT_ENABLE_LOG_CONTEXT)
  add_executable(cmlog-test cmlog-test.c)
endif(SPT_ENABLE_LOG_CONTEXT)

add_executable(dllist-test dllist-test.c)

#add_executable(matrix-test matrix-test.cc)
#add_executable(meta-test meta-test.c)
