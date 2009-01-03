project(Support)
cmake_minimum_required(VERSION 2.4 FATAL_ERROR)
mark_as_advanced(FORCE CMAKE_BACKWARDS_COMPATIBILITY)

list(APPEND CMAKE_MODULE_PATH "${Support_SOURCE_DIR}/cmake")
################################################################
# CPack
include(${Support_SOURCE_DIR}/cmake/package.cmake)
#
################################################################

################################################################
# Options

# Define an option only if its variable is not yet defined.
macro(option_maybe var doc default)
  if(NOT DEFINED "${var}")
    option("${var}" "${doc}" "${default}")
  endif(NOT DEFINED "${var}")
endmacro(option_maybe)

# Compile-time options
option_maybe(SPT_ENABLE_MLOG_CONTEXT "Enable support for logging contexts" ON)
option_maybe(SPT_VECT_CACHE_MAGNITUDE "Cache the calculated magnitude of vectors, when possible" ON)
option_maybe(SPT_ENABLE_CONSISTENCY_CHECKS "Enable run-time consistency checks" ON)

# Build components
option_maybe(SPT_BUILD_SHARED "Build a shared version of the library" OFF)
option_maybe(SPT_BUILD_STATIC "Build a static version of the library" ON)

if(SPT_ENABLE_MLOG_CONTEXT)
  set(SPT_ENABLE_MLOG_CONTEXT 1)
else(SPT_ENABLE_MLOG_CONTEXT)
  set(SPT_ENABLE_MLOG_CONTEXT)
endif(SPT_ENABLE_MLOG_CONTEXT)

if(SPT_VECT_CACHE_MAGNITUDE)
  set(SPT_VECT_CACHE_MAGNITUDE 1)
else(SPT_VECT_CACHE_MAGNITUDE)
  set(SPT_VECT_CACHE_MAGNITUDE)
endif(SPT_VECT_CACHE_MAGNITUDE)

if(SPT_ENABLE_CONSISTENCY_CHECKS)
  set(SPT_ENABLE_CONSISTENCY_CHECKS 1)
else(SPT_ENABLE_CONSISTENCY_CHECKS)
  set(SPT_ENABLE_CONSISTENCY_CHECKS)
endif(SPT_ENABLE_CONSISTENCY_CHECKS)
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
