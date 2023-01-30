cmake_minimum_required(VERSION 3.23)
project(METIDATelemetry) #PROJECT METIDA

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} main.cpp cpuState.cpp cpuState.h memoryState.cpp memoryState.h thermalState.cpp thermalState.h)

target_link_libraries(${PROJECT_NAME} libtinyLogger.so libtinyConfig.so libtinyUtils.so)