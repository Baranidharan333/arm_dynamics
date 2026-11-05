# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_dynamics_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED dynamics_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(dynamics_FOUND FALSE)
  elseif(NOT dynamics_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(dynamics_FOUND FALSE)
  endif()
  return()
endif()
set(_dynamics_CONFIG_INCLUDED TRUE)

# output package information
if(NOT dynamics_FIND_QUIETLY)
  message(STATUS "Found dynamics: 0.1.0 (${dynamics_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'dynamics' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT dynamics_DEPRECATED_QUIET)
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(dynamics_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${dynamics_DIR}/${_extra}")
endforeach()
