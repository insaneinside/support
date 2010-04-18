set(SPT_SHARED_TARGET_SUFFIX "" CACHE INTERNAL "Suffix to use on the shared target")
set(SPT_STATIC_TARGET_SUFFIX "" CACHE INTERNAL "Suffix to use on the static target")
set(SUPPORT_LIBRARY "support"
  CACHE STRING "Name of the default library target for linking")

# Configure library names
if(SPT_BUILD_SHARED AND SPT_BUILD_STATIC)
  # Building both.  Use suffixes.
  set(SPT_STATIC_TARGET_SUFFIX "-static")
  set(SPT_SHARED_TARGET_SUFFIX "-shared")
  set(SUPPORT_LIBRARY "support-static") # default to the static library
  set(SPT_LIBRARY_TARGETS "support-static" "support-shared" PARENT_SCOPE)
elseif(NOT SPT_BUILD_STATIC AND NOT SPT_BUILD_SHARED)
  # Building nothing?  Yell at the user.
  message(FATAL_ERROR "Neither SPT_BUILD_SHARED nor SPT_BUILD_STATIC is set!")
else(SPT_BUILD_SHARED AND SPT_BUILD_STATIC)
  # Building only one library.
  set(SUPPORT_LIBRARY "support")
  set(SPT_LIBRARY_TARGETS "${SUPPORT_LIBRARY}" PARENT_SCOPE)
  mark_as_advanced(SUPPORT_LIBRARY FORCE)
endif(SPT_BUILD_SHARED AND SPT_BUILD_STATIC)

set(support_SOURCES
  ref.cc
  dllist.c
  hash_table.c
  mlog.c
  strutils.c
  vector.c
  matrix.c
  )

if(SPT_ENABLE_LOG_CONTEXT)
  list(APPEND support_SOURCES spt-context.c spt-context-parse-spec.c)
endif(SPT_ENABLE_LOG_CONTEXT)

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

if(SPT_INSTALL)
  install(TARGETS ${SPT_LIBRARY_TARGETS}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    )
endif(SPT_INSTALL)
