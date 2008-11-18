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

add_library(support STATIC ${support_SOURCES})

install(TARGETS support
  ARCHIVE DESTINATION lib
  )
