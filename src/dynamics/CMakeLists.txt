cmake_minimum_required(VERSION 3.22)
project(dynamics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# -----------------------------
# External packages
# -----------------------------
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)
find_package(orocos_kdl REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(kdl_parser REQUIRED)
find_package(urdfdom REQUIRED)

if(NOT TARGET kdl_parser::kdl_parser)
  # For old kdl-parser such as libkdl-parser-dev 1.14.2 in Debian.
  add_library(kdl_parser::kdl_parser INTERFACE IMPORTED)
  target_link_libraries(kdl_parser::kdl_parser
                        INTERFACE ${kdl_parser_LIBRARIES})
  target_include_directories(kdl_parser::kdl_parser
                             INTERFACE ${kdl_parser_INCLUDE_DIRS})
endif()

# -----------------------------
# Library
# -----------------------------
add_library(dynamics_lib STATIC src/chain_model.cpp src/gravity_model.cpp src/inertia_model.cpp
                             src/coriolis_model.cpp)
set_target_properties(dynamics_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(dynamics_lib
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(
  dynamics_lib
  PUBLIC ${orocos_kdl_LIBRARIES} kdl_parser::kdl_parser urdfdom::urdfdom_model Eigen3::Eigen)

# -----------------------------
# Node
# -----------------------------
# /robot_description + /joint_states + /joint_desired -> /feedforward_torque, /gravity_torque
add_executable(gravity_compensation_node src/gravity_compensation_node.cpp)
target_link_libraries(
  gravity_compensation_node PRIVATE dynamics_lib rclcpp::rclcpp ${sensor_msgs_TARGETS}
                                    ${std_msgs_TARGETS} ${trajectory_msgs_TARGETS})

install(TARGETS gravity_compensation_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY config DESTINATION share/${PROJECT_NAME})

# -----------------------------
# Tests
# -----------------------------
if(BUILD_TESTING)
  find_package(ament_cmake_gmock REQUIRED)
  find_package(single_joint_description REQUIRED)

  set(TEST_URDF_DIR "${single_joint_description_DIR}/../urdf")

  ament_add_gmock(test_gravity_model test/test_gravity_model.cpp)
  target_link_libraries(test_gravity_model dynamics_lib)
  target_compile_definitions(test_gravity_model
                             PRIVATE TEST_URDF="${TEST_URDF_DIR}/single_joint_1kg.urdf")

  ament_add_gmock(test_dynamics_models test/test_dynamics_models.cpp)
  target_link_libraries(test_dynamics_models dynamics_lib)
  target_compile_definitions(
    test_dynamics_models
    PRIVATE TEST_URDF="${TEST_URDF_DIR}/single_joint_1kg.urdf"
            TEST_DUAL_URDF="${TEST_URDF_DIR}/dual_joint_1kg.urdf")
endif()

ament_package()
