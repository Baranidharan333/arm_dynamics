# generated from rosidl_cmake/cmake/rosidl_cmake_aggregate_target-extras.cmake.in

# Create a convenience aggregate target hw_interface::hw_interface
# that links all generated interface targets, so downstream packages can use
# a single modern CMake target name instead of ${hw_interface_TARGETS}.
if(hw_interface_TARGETS AND NOT TARGET hw_interface::hw_interface)
  add_library(hw_interface::hw_interface INTERFACE IMPORTED)
  set_target_properties(hw_interface::hw_interface PROPERTIES
    INTERFACE_LINK_LIBRARIES "${hw_interface_TARGETS}")
endif()
