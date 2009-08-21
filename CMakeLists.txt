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
option_maybe(SPT_ENABLE_LOG_CONTEXT "Enable support for logging contexts" ON)
option_maybe(SPT_VECT_CACHE_MAGNITUDE "Cache the calculated magnitude of vectors, when possible" ON)
option_maybe(SPT_ENABLE_CONSISTENCY_CHECKS "Enable run-time consistency checks" ON)
option_maybe(SPT_CONTEXT_ENABLE_DESCRIPTION "Enable context descriptions" OFF)
set(SPT_DEFAULT_SCALAR_TYPE "float" CACHE STRING "Default type for scalar values: one of \"float\", \"double\", or \"long double\".")

# Build components
option_maybe(SPT_BUILD_SHARED "Build a shared version of the library" OFF)
option_maybe(SPT_BUILD_STATIC "Build a static version of the library" ON)

if(SPT_ENABLE_LOG_CONTEXT)
  set(SPT_ENABLE_LOG_CONTEXT 1)
else(SPT_ENABLE_LOG_CONTEXT)
  set(SPT_ENABLE_LOG_CONTEXT)
endif(SPT_ENABLE_LOG_CONTEXT)

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

if(SPT_CONTEXT_ENABLE_DESCRIPTION)
  set(SPT_CONTEXT_ENABLE_DESCRIPTION 1)
else(SPT_CONTEXT_ENABLE_DESCRIPTION)
  set(SPT_CONTEXT_ENABLE_DESCRIPTION)
endif(SPT_CONTEXT_ENABLE_DESCRIPTION)

set(SPT_SCALAR_TYPE_DEFINE "" CACHE STRING "Symbol to define for scalar_t type.")

if(SPT_DEFAULT_SCALAR_TYPE STREQUAL "float")
  set(SPT_SCALAR_TYPE_DEFINE "S_TYPE_FLOAT")
elseif(SPT_DEFAULT_SCALAR_TYPE STREQUAL "double")
  set(SPT_SCALAR_TYPE_DEFINE "S_TYPE_DOUBLE")
elseif(SPT_DEFAULT_SCALAR_TYPE STREQUAL "long double")
  set(SPT_SCALAR_TYPE_DEFINE "S_TYPE_LONG_DOUBLE")
else(SPT_DEFAULT_SCALAR_TYPE STREQUAL "float")
  message(FATAL_ERROR "Bad value for SPT_DEFAULT_SCALAR_TYPE.  Must be one of \"float\", \"double\", or \"long double\".")
endif(SPT_DEFAULT_SCALAR_TYPE STREQUAL "float")

mark_as_advanced(SPT_SCALAR_TYPE_DEFINE FORCE)
#
################################################################

################################################################
# Includes
set(CONFIG_HEADER_SOURCE
  "${Support_SOURCE_DIR}/cmake/support-config.h.in")
set(CONFIG_HEADER_DEST
  "${Support_BINARY_DIR}/include/support/support-config.h")
mark_as_advanced(FORCE CONFIG_HEADER_SOURCE CONFIG_HEADER_DEST)
configure_file(${CONFIG_HEADER_SOURCE} ${CONFIG_HEADER_DEST} @ONLY)

set(SUPPORT_INCLUDE_DIRS
  "${Support_SOURCE_DIR}/include" "${Support_BINARY_DIR}/include"
  CACHE STRING "libsupport include directories"
  )
include_directories(${SUPPORT_INCLUDE_DIRS})

install(DIRECTORY
  "${Support_SOURCE_DIR}/include/support"
  "${Support_BINARY_DIR}/include/support"
  DESTINATION include)
#
################################################################

set(SPT_WARN_FLAGS
  -Wall
  -Wconversion
  -Wextra
  -Wfloat-equal
  -Wno-long-long
  -Wno-variadic-macros
  -Wpointer-arith
  -Wsequence-point
  -Wstrict-aliasing
  -Wswitch
  -Wunused
  -pedantic
  )

macro(append_flags VARIABLE)
  # Convert from a string to a list
  separate_arguments("${VARIABLE}")

  # Append the flags
  list(APPEND "${VARIABLE}" ${ARGN})
  list(REMOVE_DUPLICATES "${VARIABLE}")

  # Convert back to a string
  string(REPLACE ";" " " "${VARIABLE}" "${${VARIABLE}}")
  set("${VARIABLE}" "${${VARIABLE}}")
endmacro(append_flags)

append_flags(CMAKE_C_FLAGS ${SPT_WARN_FLAGS} --std=c99 -finline)
append_flags(CMAKE_CXX_FLAGS ${SPT_WARN_FLAGS})

add_subdirectory(src)

if("${CMAKE_HOME_DIRECTORY}" STREQUAL "${Support_SOURCE_DIR}")
  set(BUILD_TESTING 1)
  add_subdirectory(tests)
endif("${CMAKE_HOME_DIRECTORY}" STREQUAL "${Support_SOURCE_DIR}")

# Create a CMake export file for the build tree
export(TARGETS ${SPT_LIBRARY_TARGETS} FILE support.cmake)
