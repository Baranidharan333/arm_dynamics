cmake_minimum_required(VERSION 3.8)
project(single_joint_description NONE)

find_package(ament_cmake REQUIRED)

install(DIRECTORY urdf launch rviz DESTINATION share/${PROJECT_NAME})
# meshes/src holds the CAD sources (STEP, drawing); only the STL is needed at runtime.
install(DIRECTORY meshes DESTINATION share/${PROJECT_NAME} PATTERN src EXCLUDE)

ament_package()
