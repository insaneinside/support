set(support_SOURCES
  ref.cc
  dllist.c
  hash_table.c
  mlog.c
  strutils.c
  vector.c
  matrix.c
  )

if(ENABLE_MLOG_CONTEXT)
  list(APPEND support_SOURCES mlog-context.c)
endif(ENABLE_MLOG_CONTEXT)

if(SPT_BUILD_STATIC)
  add_library(support-static STATIC ${support_SOURCES})
  set_target_properties(support-static
    PROPERTIES
    OUTPUT_NAME support
    )
  list(APPEND library_targets support-static)
endif(SPT_BUILD_STATIC)

if(SPT_BUILD_SHARED)
  add_library(support-shared SHARED ${support_SOURCES})
  set_target_properties(support-shared
    PROPERTIES
    OUTPUT_NAME support
    )
  list(APPEND library_targets support-shared)
endif(SPT_BUILD_SHARED)

install(TARGETS ${library_targets}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  )
