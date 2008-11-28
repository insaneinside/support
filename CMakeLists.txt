project(Support)
cmake_minimum_required(VERSION 2.4 FATAL_ERROR)
mark_as_advanced(FORCE CMAKE_BACKWARDS_COMPATIBILITY)

################################################################
# CPack
include(${Support_SOURCE_DIR}/cmake/package.cmake)
#
################################################################

################################################################
# Options
option(OPT_ENABLE_MLOG_CONTEXT "Enable support for logging contexts" ON)
option(OPT_VECT_CACHE_MAGNITUDE "Cache the calculated magnitude of vectors, when possible" ON)
option(SPT_BUILD_SHARED "Build a shared version of the library" OFF)
option(SPT_BUILD_STATIC "Build a static version of the library" ON)

if(OPT_ENABLE_MLOG_CONTEXT)
  set(ENABLE_MLOG_CONTEXT 1)
else(OPT_ENABLE_MLOG_CONTEXT)
  set(ENABLE_MLOG_CONTEXT)
endif(OPT_ENABLE_MLOG_CONTEXT)
if(OPT_VECT_CACHE_MAGNITUDE)
  set(VECT_CACHE_MAGNITUDE 1)
else(OPT_VECT_CACHE_MAGNITUDE)
  set(VECT_CACHE_MAGNITUDE 0)
endif(OPT_VECT_CACHE_MAGNITUDE)

mark_as_advanced(FORCE ENABLE_MLOG_CONTEXT VECT_CACHE_MAGNITUDE)
#
################################################################

################################################################
# Includes
set(CONFIG_HEADER_SOURCE
  "${Support_SOURCE_DIR}/cmake/support-config.h.in")
set(CONFIG_HEADER_DEST
  "${Support_BINARY_DIR}/include/support/support-config.h")
mark_as_advanced(FORCE CONFIG_HEADER_SOURCE CONFIG_HEADER_DEST)
configure_file(${CONFIG_HEADER_SOURCE} ${CONFIG_HEADER_DEST})
include_directories(
  "${Support_SOURCE_DIR}/include"
  "${Support_BINARY_DIR}/include")


install(DIRECTORY
  "${Support_SOURCE_DIR}/include/support"
  "${Support_BINARY_DIR}/include/support"
  DESTINATION include)
#
################################################################

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

# Create a CMake export file for the build tree
export(TARGETS ${library_targets} FILE support.cmake)
