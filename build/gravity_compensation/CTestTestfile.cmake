# CMake generated Testfile for 
# Source directory: /home/baranidharan/gravity/src/gravity_compensation
# Build directory: /home/baranidharan/gravity/build/gravity_compensation
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[test_gravity_model]=] "/usr/bin/python3" "-u" "/opt/ros/jazzy/share/ament_cmake_test/cmake/run_test.py" "/home/baranidharan/gravity/build/gravity_compensation/test_results/gravity_compensation/test_gravity_model.gtest.xml" "--package-name" "gravity_compensation" "--output-file" "/home/baranidharan/gravity/build/gravity_compensation/ament_cmake_gmock/test_gravity_model.txt" "--command" "/home/baranidharan/gravity/build/gravity_compensation/test_gravity_model" "--gtest_output=xml:/home/baranidharan/gravity/build/gravity_compensation/test_results/gravity_compensation/test_gravity_model.gtest.xml")
set_tests_properties([=[test_gravity_model]=] PROPERTIES  LABELS "gmock" REQUIRED_FILES "/home/baranidharan/gravity/build/gravity_compensation/test_gravity_model" TIMEOUT "60" WORKING_DIRECTORY "/home/baranidharan/gravity/build/gravity_compensation" _BACKTRACE_TRIPLES "/opt/ros/jazzy/share/ament_cmake_test/cmake/ament_add_test.cmake;125;add_test;/opt/ros/jazzy/share/ament_cmake_gmock/cmake/ament_add_gmock_test.cmake;98;ament_add_test;/opt/ros/jazzy/share/ament_cmake_gmock/cmake/ament_add_gmock.cmake;90;ament_add_gmock_test;/home/baranidharan/gravity/src/gravity_compensation/CMakeLists.txt;67;ament_add_gmock;/home/baranidharan/gravity/src/gravity_compensation/CMakeLists.txt;0;")
subdirs("gmock")
subdirs("gtest")
