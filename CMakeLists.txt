# This file is a part of MRNIU/cpu_io (https://github.com/MRNIU/cpu_io).
#
# CMakeLists.txt for MRNIU/cpu_io.

# 设置最小 cmake 版本
cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

# 设置项目名与版本
project(cpu_io VERSION 0.0.1)

# 禁止原地编译
if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  # 如果你看到这句话，cmake 此时已经在根目录下生成了一些临时文件，你需要删除它们 CMakeFiles, CMakeCache.txt
  message(
    FATAL_ERROR
      "In-source builds not allowed."
      "Please make a new directory (called a build directory) "
      "and run CMake from there.")
endif()

configure_file(${CMAKE_SOURCE_DIR}/.pre-commit-config.yaml.in
               ${CMAKE_SOURCE_DIR}/.pre-commit-config.yaml @ONLY)

add_executable(
  cpu_io_test
  # operator.cpp
  test.cpp)
