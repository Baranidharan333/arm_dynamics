cmake_minimum_required(VERSION 3.8)
project(posveleff_controllers)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(controller_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)


add_library(
  posveleff_controller SHARED
  src/posveleff_controller.cpp
)

target_include_directories(
  posveleff_controller
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(
  posveleff_controller
  controller_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
  sensor_msgs
  trajectory_msgs
)

pluginlib_export_plugin_description_file(
  controller_interface
  controller_plugins.xml
)
install(
  TARGETS posveleff_controller
  EXPORT export_posveleff_controller
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(
  DIRECTORY include/
  DESTINATION include
)
install(
  FILES controller_plugins.xml
  DESTINATION share/${PROJECT_NAME}
)
install(
  DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)
# /move_home and /set_home_position services (ros2 run posveleff_controllers home_move_service.py)
install(
  PROGRAMS scripts/home_move_service.py
  DESTINATION lib/${PROJECT_NAME}
)
ament_export_include_directories(include)

ament_export_dependencies(
  controller_interface
  pluginlib
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_motion_limiter test/test_motion_limiter.cpp)
  target_include_directories(test_motion_limiter PRIVATE include)
endif()

ament_package()
