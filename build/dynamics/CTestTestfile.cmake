# CMake generated Testfile for 
# Source directory: /home/baranidharan/gravity/src/dynamics
# Build directory: /home/baranidharan/gravity/build/dynamics
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[test_robot_dynamics]=] "/usr/bin/python3" "-u" "/opt/ros/jazzy/share/ament_cmake_test/cmake/run_test.py" "/home/baranidharan/gravity/build/dynamics/test_results/dynamics/test_robot_dynamics.gtest.xml" "--package-name" "dynamics" "--output-file" "/home/baranidharan/gravity/build/dynamics/ament_cmake_gtest/test_robot_dynamics.txt" "--command" "/home/baranidharan/gravity/build/dynamics/test_robot_dynamics" "--gtest_output=xml:/home/baranidharan/gravity/build/dynamics/test_results/dynamics/test_robot_dynamics.gtest.xml")
set_tests_properties([=[test_robot_dynamics]=] PROPERTIES  LABELS "gtest" REQUIRED_FILES "/home/baranidharan/gravity/build/dynamics/test_robot_dynamics" TIMEOUT "60" WORKING_DIRECTORY "/home/baranidharan/gravity/build/dynamics" _BACKTRACE_TRIPLES "/opt/ros/jazzy/share/ament_cmake_test/cmake/ament_add_test.cmake;125;add_test;/opt/ros/jazzy/share/ament_cmake_gtest/cmake/ament_add_gtest_test.cmake;95;ament_add_test;/opt/ros/jazzy/share/ament_cmake_gtest/cmake/ament_add_gtest.cmake;93;ament_add_gtest_test;/home/baranidharan/gravity/src/dynamics/CMakeLists.txt;51;ament_add_gtest;/home/baranidharan/gravity/src/dynamics/CMakeLists.txt;0;")
subdirs("gtest")
