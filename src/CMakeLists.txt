set(STATIC_TARGET_SUFFIX "-static")
set(SHARED_TARGET_SUFFIX "-shared")

if(SPT_BUILD_STATIC AND NOT SPT_BUILD_SHARED)
  set(STATIC_TARGET_SUFFIX "")
elseif(SPT_BUILD_SHARED AND NOT SPT_BUILD_STATIC)
  set(SHARED_TARGET_SUFFIX "")
endif(SPT_BUILD_STATIC AND NOT SPT_BUILD_SHARED)

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

# Static library
if(SPT_BUILD_STATIC)
  add_library(support${STATIC_TARGET_SUFFIX} STATIC ${support_SOURCES})
  set_target_properties(support${STATIC_TARGET_SUFFIX}
    PROPERTIES
    OUTPUT_NAME support
    )
  list(APPEND library_targets support${STATIC_TARGET_SUFFIX})
endif(SPT_BUILD_STATIC)

# Shared library
if(SPT_BUILD_SHARED)
  add_library(support${SHARED_TARGET_SUFFIX} SHARED ${support_SOURCES})
  set_target_properties(support${SHARED_TARGET_SUFFIX}
    PROPERTIES
    OUTPUT_NAME support
    )
  list(APPEND library_targets support${SHARED_TARGET_SUFFIX})
endif(SPT_BUILD_SHARED)

install(TARGETS ${library_targets}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  )
