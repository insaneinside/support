# Configure library names
if(SPT_BUILD_SHARED AND NOT SPT_BUILD_STATIC OR SPT_BUILD_STATIC AND NOT SPT_BUILD_SHARED)
  # Building only one library.
  set(SUPPORT_LIBRARY "support")
  mark_as_advanced(SUPPORT_LIBRARY FORCE)
  if(SPT_BUILD_STATIC AND NOT SPT_BUILD_SHARED)
    set(SPT_STATIC_TARGET_SUFFIX "") # static library only
  elseif(SPT_BUILD_SHARED AND NOT SPT_BUILD_STATIC)
    set(SPT_SHARED_TARGET_SUFFIX "") # shared library only
  endif(SPT_BUILD_STATIC AND NOT SPT_BUILD_SHARED)

elseif(SPT_BUILD_SHARED AND SPT_BUILD_STATIC)
  # Building both.  Use suffixes.
  set(SPT_STATIC_TARGET_SUFFIX "-static")
  set(SPT_SHARED_TARGET_SUFFIX "-shared")
  set(SUPPORT_LIBRARY "support")
else(SPT_BUILD_STATIC AND NOT SPT_BUILD_SHARED)
  # Building nothing?  Yell at the user.
  message(FATAL_ERROR "Neither SPT_BUILD_SHARED nor SPT_BUILD_STATIC is set!")
endif(SPT_BUILD_SHARED AND NOT SPT_BUILD_STATIC OR SPT_BUILD_STATIC AND NOT SPT_BUILD_SHARED)

set(support_SOURCES
  ref.cc
  dllist.c
  hash_table.c
  mlog.c
  strutils.c
  vector.c
  matrix.c
  )

if(SPT_ENABLE_MLOG_CONTEXT)
  list(APPEND support_SOURCES mlog-context.c)
endif(SPT_ENABLE_MLOG_CONTEXT)

# Static library
if(SPT_BUILD_STATIC)
  add_library(support${SPT_STATIC_TARGET_SUFFIX} STATIC ${support_SOURCES})
  set_target_properties(support${SPT_STATIC_TARGET_SUFFIX}
    PROPERTIES
    OUTPUT_NAME support
    )
  list(APPEND SPT_LIBRARY_TARGETS support${SPT_STATIC_TARGET_SUFFIX})
endif(SPT_BUILD_STATIC)

# Shared library
if(SPT_BUILD_SHARED)
  add_library(support${SPT_SHARED_TARGET_SUFFIX} SHARED ${support_SOURCES})
  set_target_properties(support${SPT_SHARED_TARGET_SUFFIX}
    PROPERTIES
    OUTPUT_NAME support
    )
  list(APPEND SPT_LIBRARY_TARGETS support${SPT_SHARED_TARGET_SUFFIX})
endif(SPT_BUILD_SHARED)

install(TARGETS ${SPT_LIBRARY_TARGETS}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  )
